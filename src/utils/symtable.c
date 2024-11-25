/** 
 * Projekt IFJ2024
 * 
 * @brief Symbolická tabulka implementovaná hashovací tabulkou s implicitním zřetězením
 * 
 * @author xsidlil00; Lukáš Šidlík
 */

#include <stdbool.h>
#include <string.h>
#include "symtable.h"
#include "memory_table.h"

/**
 * @brief Vypočítá hash pro řetězec pomocí algoritmu djb2.
 *
 * @param str řetězec, pro který chceme vypočítat hash
 * @return hash jako unsigned int
 */
unsigned int getHash(char *str)
{
    unsigned int hash = 5381;
    for (int i = 0; str[i] != '\0'; i++)
    {
        unsigned char c = (unsigned char)str[i];
        hash = ((hash << 5) + hash) + c;
    }
    return hash % SYMTABLE_SIZE;
}

void symtable_init(symtable_t *symtable, err_codes *error){
    if(symtable == NULL){
        *error = err_internal;
        return;
    }

    for (int i = 0; i < SYMTABLE_SIZE; i++)
    {
        (*symtable)[i] = NULL;
    }
}

data_t *symtable_get_item(symtable_t *symtable, char *name, err_codes *error){
    if(symtable == NULL || name == NULL){
        *error = err_internal;
        return NULL;
    }

    unsigned int hash = getHash(name);
    symtable_item_t *item = (*symtable)[hash];
    while (item != NULL)
    {
        if (strcmp(item->key, name) == 0)
        {
            return &item->data;
        }
        item = item->next;
    }
    return NULL;
}

data_t *symtable_insert(symtable_t *symtable, char *name, err_codes *error){
    if(symtable == NULL || name == NULL){
        *error = err_internal;
        return NULL;
    }
    if(symtable_get_item(symtable, name, error) != NULL){
        *error = err_redef;
        return NULL;
    }

    symtable_item_t *new_item = (symtable_item_t *)imalloc(sizeof(symtable_item_t));
    if (new_item == NULL){
        *error = err_internal;
        return NULL;
    }

    new_item->data.parameters = (dynamic_array_t *)imalloc(sizeof(dynamic_array_t));
    if (new_item->data.parameters == NULL)
    {
        *error = err_internal;
        free(new_item);
        return NULL;
    }

    err_codes aloc = dynamic_array_init(new_item->data.parameters);
    if (aloc != err_none)
    {
        *error = err_internal;
        free(new_item->data.parameters);
        free(new_item);
        return NULL;
    }

    new_item->key = (char *)imalloc((strlen(name) + 1) * sizeof(char));
    if (new_item->key == NULL)
    {
        *error = err_internal;
        free(new_item->data.parameters);
        free(new_item);
        return NULL;
    }

    strcpy(new_item->key, name);
    new_item->data.id = new_item->key;
    new_item->data.type = DATA_TYPE_UND;
    new_item->data.as_func = false;
    new_item->data.modified = false;
    new_item->data.canNull = false;
    new_item->data.isConst = false;
    new_item->data.used = false;
    new_item->next = NULL;

    unsigned int hash = getHash(name);

    if ((*symtable)[hash] == NULL)
    {
        (*symtable)[hash] = new_item;
    }
    else
    {
        symtable_item_t *prev = (*symtable)[hash];
        hash = (hash + 1) % SYMTABLE_SIZE;
        while ((*symtable)[hash] != NULL)
        {
            if (prev->next == NULL)
            {
                prev->next = (*symtable)[hash];
            }
            hash = (hash + 1) % SYMTABLE_SIZE;
            prev = prev->next;
        }
        (*symtable)[hash] = new_item;
        prev->next = new_item;
    }
    return &new_item->data;
}

void symtable_insert_params(data_t *data, token_type type, err_codes *error){
    if(data == NULL){
        *error = err_internal;
    }

    if (type == tok_t_i32 || type == tok_t_i32_opt || type == tok_t_f64 || type == tok_t_f64_opt || type == tok_t_u8 || type == tok_t_u8_opt || type == tok_t_str || type == tok_t_bool)  {
        *error = dynamic_array_insert(data->parameters, type);
    } else {
        *error = dynamic_array_insert(data->parameters, tok_t_unused);
    }
}

void symtable_destroy(symtable_t *symtable, err_codes *error, bool isFirst){
    if(symtable == NULL){
        *error = err_internal;
    }

    symtable_item_t *item;
    for (int i = 0; i < SYMTABLE_SIZE; i++)
    {
        item = (*symtable)[i];
        if (item == NULL)
            continue;

        if(!isFirst){
            if (!item->data.used || !item->data.modified){
                if(*error == err_none)
                    *error = err_dt_unused;
            }
        }
        
        free(item->key);
        if (item->data.parameters != NULL)
        {
            dynamic_array_destroy(item->data.parameters);
            free(item->data.parameters);
        }
        
        free(item);
        (*symtable)[i] = NULL;
    }
}

void symtable_insert_builtin(symtable_t *symtable, err_codes *error){
    data_t * data;

    //ifj.readstr() ?[]u8
    data = symtable_insert(symtable, "ifj.readstr", error);
    if(*error != err_none)
        return;
    
    data->canNull = true;
    data->type = DATA_TYPE_U8;
    data->used = true;
    data->modified = true;
    
    //ifj.readi32() ?i32
    data = symtable_insert(symtable, "ifj.readi32", error);
    if(*error != err_none)
        return;

    data->canNull = true;
    data->type = DATA_TYPE_INT;
    data->used = true;
    data->modified = true;

    //ifj.readf64() ?f64
    data = symtable_insert(symtable, "ifj.readf64", error);
    if(*error != err_none)
        return;
    
    data->canNull = true;
    data->type = DATA_TYPE_DOUBLE;
    data->used = true;
    data->modified = true;

    //ifj.write(term) void
    data = symtable_insert(symtable, "ifj.write", error);
    if(*error != err_none)
        return;

    data->type = DATA_TYPE_VOID;
    symtable_insert_params(data, tok_t_null, error);
    data->used = true;
    data->modified = true;

    if(*error != err_none)
        return;

    //ifj.i2f(term: i32) f64
    data = symtable_insert(symtable, "ifj.i2f", error);
    if(*error != err_none)
        return;

    data->type = DATA_TYPE_DOUBLE;
    symtable_insert_params(data, tok_t_i32, error);
    data->used = true;
    data->modified = true;

    if(*error != err_none)
        return;

    //ifj.f2i(term: f64) i32
    data = symtable_insert(symtable, "ifj.f2i", error);
    if(*error != err_none)
        return;

    data->type = DATA_TYPE_INT;
    symtable_insert_params(data, tok_t_f64, error);
    data->used = true;
    data->modified = true;

    if(*error != err_none)
        return;

    //ifj.string(term) []u8
    data = symtable_insert(symtable, "ifj.string", error);
    if(*error != err_none)
        return;

    data->type = DATA_TYPE_U8;
    symtable_insert_params(data, tok_t_str, error);
    data->used = true;
    data->modified = true;

    if(*error != err_none)
        return;

    //ifj.length(s: []u8) i32
    data = symtable_insert(symtable, "ifj.length", error);
    if(*error != err_none)
        return;
    
    data->type = DATA_TYPE_INT;
    symtable_insert_params(data, tok_t_u8, error);
    data->used = true;
    data->modified = true;

    if(*error != err_none)
        return;
    
    //ifj.concat(s1: []u8, s2: []u8) []u8
    data = symtable_insert(symtable, "ifj.concat", error);
    if(*error != err_none)
        return;

    data->type = DATA_TYPE_U8;
    data->used = true;
    data->modified = true;
    symtable_insert_params(data, tok_t_u8, error);
    if(*error != err_none)
        return;

    symtable_insert_params(data, tok_t_u8, error);
    if(*error != err_none)
        return;

    //ifj.substring(s: []u8, i: i32, j: i32) ?[]u8
    data = symtable_insert(symtable, "ifj.substring", error);
    if(*error != err_none)
        return;

    data->type = DATA_TYPE_U8;
    data->used = true;
    data->modified = true;
    symtable_insert_params(data, tok_t_u8, error);
    if(*error != err_none)
        return;

    symtable_insert_params(data, tok_t_i32, error);
    if(*error != err_none)
        return;

    symtable_insert_params(data, tok_t_i32, error);
    if(*error != err_none)
        return;

    data->canNull = true;

    //ifj.strcmp(s1: []u8, s2: []u8) i32
    data = symtable_insert(symtable, "ifj.strcmp", error);
    if(*error != err_none)
        return;

    data->type = DATA_TYPE_INT;
    data->used = true;
    data->modified = true;
    symtable_insert_params(data, tok_t_u8, error);
    if(*error != err_none)
        return;
    
    symtable_insert_params(data, tok_t_u8, error);
    if(*error != err_none)
        return;

    //ifj.ord(s: []u8, i: i32) i32
    data = symtable_insert(symtable, "ifj.ord", error);
    if(*error != err_none)
        return;

    data->type = DATA_TYPE_INT;
    data->used = true;
    data->modified = true;
    symtable_insert_params(data, tok_t_u8, error);
    if(*error != err_none)
        return;
    
    symtable_insert_params(data, tok_t_i32, error);
    if(*error != err_none)
        return;
    
    //ifj.chr(i: i32) []u8
    data = symtable_insert(symtable, "ifj.chr", error);
    if(*error != err_none)
        return;

    data->type = DATA_TYPE_U8;
    data->used = true;
    data->modified = true;
    symtable_insert_params(data, tok_t_i32, error);
    if(*error != err_none)
        return;

    //@as(i32, temp : i32) i32
    data = symtable_insert(symtable, "@as", error);
    if(*error != err_none)
        return;
    
    data->type = DATA_TYPE_INT;
    data->used = true;
    data->modified = true;
    symtable_insert_params(data, tok_t_i32, error);
    if(*error != err_none)
        return;
}
