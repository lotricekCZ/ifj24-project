/** IFJ2024
 * xsidlil00; Lukáš Šidlík
 */

#include "symtable.h"
#include <stdbool.h>
#include <string.h>

/**
 * @brief Vypočítá hash pro řetězec pomocí algoritmu djb2.
 *
 * @param str řetězec, pro který chceme vypočítat hash
 * @return hash jako unsigned int
 */
unsigned int getHash(char* str){
    unsigned int hash = 5381;
    for (int i = 0; str[i] != '\0'; i++){
        unsigned char c = (unsigned char)str[i];
        hash = ((hash << 5) + hash) + c;
    }
    return hash % SYMTABLE_SIZE;
}

bool symtable_init(symtable_t *symtable, int *err){
    if(symtable == NULL){
        // vnitřní chyba
        return false;
    }

    for(int i = 0; i < SYMTABLE_SIZE; i++){
        (*symtable)[i] = NULL;
    }
    return true;
}

data_t *symtable_get_item(symtable_t *symtable, char *name, int *err){
    if(symtable == NULL || name == NULL){
        //vnitřní chyba
        return NULL;
    }
    
    unsigned int hash = getHash(name);
    symtable_item_t *item = (*symtable)[hash];
    while(item != NULL){
        if(strcmp(item->key, name) == 0){
            return &item->data;
        }
        item = item->next;
    }
    return NULL;
}

data_t *symtable_insert(symtable_t *symtable, char *name, int *err){
    if(symtable == NULL || name == NULL){
        //vnitřní chyba
        return NULL;
    }
    if(symtable_get_item(symtable, name, err) != NULL){
        //jmeno jiz existuje
        return NULL;
    }

    symtable_item_t *new_item = (symtable_item_t *)malloc(sizeof(symtable_item_t));
    if(new_item == NULL){
        //vnitřní chyba
        return NULL;
    }

    new_item->data.parameters = (DymString *)malloc(sizeof(DymString));
    if(new_item->data.parameters == NULL){
        //vnitřní chyba
        free(new_item);
        return NULL;
    }

    bool aloc = DymString_Init(new_item->data.parameters);
    if(!aloc){
        //vnitřní chyba
        free(new_item->data.parameters);
        free(new_item);
        return NULL;
    }
    
    new_item->key = (char *)malloc((strlen(name) + 1) * sizeof(char));
    if(new_item->key == NULL){
        //vnitřní chyba
        free(new_item->data.parameters);
        free(new_item);
        return NULL;
    }

    strcpy(new_item->key, name);
    new_item->data.id = new_item->key;
    new_item->data.type = DATA_TYPE_UND;
    new_item->data.init = false;
    new_item->data.canNull = false;
    new_item->data.isConst = false;
    new_item->data.used = false;
    new_item->next = NULL;

    unsigned int hash = getHash(name);
    printf("Hash: %d\n", hash);

    if((*symtable)[hash] == NULL){
        (*symtable)[hash] = new_item;
    }
    else{
        symtable_item_t *prev = (*symtable)[hash];
        hash = (hash + 1) % SYMTABLE_SIZE;
        while((*symtable)[hash] != NULL){
            if(prev->next == NULL){
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

bool symtable_insert_params(data_t *data, int type, int *err){
    if(data == NULL){
        return false;
    }
    
    bool result = false;
    if(type == DATA_TYPE_INT && !data->canNull){
        result = DymString_Insert_Char(data->parameters, 'i');
    }
    else if(type == DATA_TYPE_INT && data->canNull){
        result = DymString_Insert_Char(data->parameters, 'I');
    }
    else if(type == DATA_TYPE_DOUBLE && !data->canNull){
        result = DymString_Insert_Char(data->parameters, 'd');
    }
    else if(type == DATA_TYPE_DOUBLE && data->canNull){
        result = DymString_Insert_Char(data->parameters, 'D');
    }
    else if(type == DATA_TYPE_U8 && !data->canNull){
        result = DymString_Insert_Char(data->parameters, 'u');
    }
    else if(type == DATA_TYPE_U8 && data->canNull){
        result = DymString_Insert_Char(data->parameters, 'U');
    }
    else if(type == DATA_TYPE_STRING){
        result = DymString_Insert_Char(data->parameters, 's');
    }
    else if(type == DATA_TYPE_BOOLEAN){
        result = DymString_Insert_Char(data->parameters, 'b');
    }
    
    if(!result){
        //vnitřní chyba
        return false;
    }
    return true;
}

bool symtable_destroy(symtable_t *symtable, int *err){
    if(symtable == NULL){
        //vnitřní chyba
        return false;
    }

    symtable_item_t *item;
    for (int i = 0; i < SYMTABLE_SIZE; i++){
        item = (*symtable)[i];
        if(item == NULL)
            continue;

        if(!item->data.used){
            //chyba nepoužita promněná
        }
            
        
        free(item->key);
        if(item->data.parameters != NULL){
            DymString_Destroy(item->data.parameters);
            free(item->data.parameters);
        }
        
        free(item);
        (*symtable)[i] = NULL;
    }
    return true;
}

bool symtable_insert_builtin(symtable_t *symtable, int *err){
    return true;
}
