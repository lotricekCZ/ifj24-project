/** IFJ2024
 * xsidlil00; Lukáš Šidlík
 */

#include "expresion.h"
#include "errors.h"
#include "memory_table.h"

data_t *find(data_t *result_data, DLList sym_list, Token_ptr popToken, symtable_t *symtable, err_codes *error){
    DLL_Last(&sym_list);
    while(sym_list.current != NULL){
        symtable = DLL_GetCurrent(&sym_list);
        result_data = symtable_get_item(symtable, popToken->attribute, error);
        if(result_data != NULL){
            break;
        }
        DLL_Prev(&sym_list);
    }

    if(*error != err_none){
        return NULL;
    }

    if(result_data == NULL){
        fprintf(stderr, "ERROR: nenalezen v symtable\n");
        exit(2);
        // chyba -> nenalezení v symtable
    }
    return result_data;
}

void check_operator(data_t *result_data, data_t *result, err_codes *error){
    if(result_data->canNull){
        *error = err_dt_invalid;
        fprintf(stderr, "ERROR: +,-,*,/ nemůže být null\n");
        return;
    }

    if(result_data->type != DATA_TYPE_INT && result_data->type != DATA_TYPE_DOUBLE){
        *error = err_dt_invalid;
        fprintf(stderr, "ERROR: +,-,*,/ nelze emplicitně přetypovat\n");
        return;
    }

    result_data->used = true;
}

void check_operator2(data_t *result_data, data_t *result, err_codes *error){
    if(result_data->type != DATA_TYPE_INT && result_data->type != DATA_TYPE_DOUBLE && result_data->type != DATA_TYPE_BOOLEAN){
        *error = err_dt_invalid;
        fprintf(stderr, "ERROR: ==, !=, <, <=, >, >= nelze emplicitně přetypovat\n");
        return;
    }
    result_data->used = true;
}

data_t* resultType(data_t *result, Token_ptr popToken, symtable_t *symtable, DLList sym_list, err_codes *error){
    symtable = DLL_GetLast(&sym_list);
    data_t *sym;
    sym = imalloc(sizeof(data_t));
    if (sym == NULL){
        *error = err_internal;
        return NULL;
    }

    if(popToken->type == tok_t_int){
        result->type = DATA_TYPE_INT;
        return result;
    }
    else if(popToken->type == tok_t_flt){
        result->type = DATA_TYPE_DOUBLE;
        return result;
    }
    else if(popToken->type == tok_t_true || popToken->type == tok_t_false){
        result->type = DATA_TYPE_BOOLEAN;
        return result;
    }
    else if(popToken->type == tok_t_null){
        result->canNull = true;
        return result;
    }
    else if(popToken->type == tok_t_sym){
        sym = find(result, sym_list, popToken, symtable, error);
        if(*error != err_none)
            return NULL;
        
        sym->used = true;
        result->type = sym->type;
        result->canNull = sym->canNull;
        return result;
    }
}

data_t* postfix_semantic(Token_ptr *postfix, int postfix_index, DLList sym_list, symtable_t *symtable, err_codes *error){
    Stack stack;
    data_t *result;
    data_t *result_data;
    data_t *result_data2;
    Token_ptr popToken;
    Token_ptr popToken2;

    result = imalloc(sizeof(data_t));
    if (result == NULL)
    {
        *error = err_internal;
        return NULL;
    }
    result->canNull = false;

    result_data2 = imalloc(sizeof(data_t));
    if (result_data2 == NULL)
    {
        *error = err_internal;
        return NULL;
    }

    result->type = DATA_TYPE_UND;
    result = resultType(result, postfix[0], symtable, sym_list, error);

    if (*error != err_none)
        return NULL;

    if(result == NULL){
        *error = err_internal;
        return NULL;
    }

    init(&stack);
    for (int i = 0; i < postfix_index; i++)
    {
        if(postfix[i]->type == tok_t_sym || postfix[i]->type == tok_t_int || postfix[i]->type == tok_t_flt || 
           postfix[i]->type == tok_t_true || postfix[i]->type == tok_t_false || postfix[i]->type == tok_t_null ||
           postfix[i]->type == tok_t_unreach)
        {
            push(&stack, postfix[i]);
            continue;
        }

        if(postfix[i]->type == tok_t_plus || postfix[i]->type == tok_t_minus || postfix[i]->type == tok_t_times){
            popToken2 = pop(&stack);
            if(isEmpty(&stack)){
                *error = err_semantic;
                fprintf(stderr, "ERROR: +,-,* není unární operátor\n");
                return NULL;
            }
            popToken = pop(&stack);

            switch (popToken->type)
            {
            case tok_t_sym:
                result_data = find(result_data, sym_list, popToken, symtable, error);
                if(*error != err_none)
                    return NULL;
                check_operator(result_data, result, error);
                if(*error != err_none)
                    return NULL;

                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error);
                    if(*error != err_none)
                        return NULL;
                    check_operator(result_data2, result, error);
                    if(*error != err_none)
                        return NULL;

                    if(result_data->type != result_data2->type){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_int){
                    if(result_data->type != DATA_TYPE_INT){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_flt){
                    if(result_data->type != DATA_TYPE_DOUBLE){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    push(&stack, popToken);
                }
                else{
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: +, -, * nepodporuje dané datové typy\n");
                    return NULL;
                }
                
                break;

            case tok_t_int:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error);
                    if(*error != err_none)
                        return NULL;
                    check_operator(result_data2, result, error);
                    if(*error != err_none)
                        return NULL;

                    if(result_data2->type != DATA_TYPE_INT){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    push(&stack, popToken2);
                }
                else if(popToken2->type == tok_t_flt){
                    result->type = DATA_TYPE_DOUBLE;
                    push(&stack, popToken2);
                }
                else if(popToken2->type == tok_t_int){
                    push(&stack, popToken);
                }
                else{
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: +, -, * nepodporuje dané datové typy\n");
                    return NULL;
                }

                break;

            case tok_t_flt:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error);
                    if(*error != err_none)
                        return NULL;
                    check_operator(result_data2, result, error);
                    if(*error != err_none)
                        return NULL;

                    if(result_data2->type != DATA_TYPE_DOUBLE){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    push(&stack, popToken2);
                }
                else if(popToken2->type == tok_t_int || popToken2->type == tok_t_flt){
                    push(&stack, popToken);
                }
                else{
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: +, -, * nepodporuje dané datové typy\n");
                    return NULL;
                }
                break;
            
            default:
                *error = err_dt_invalid;
                fprintf(stderr, "ERROR: +, -, * nepodporuje dané datové typy\n");
                return NULL;
                break;
            }
            continue;
        }

        if(postfix[i]->type == tok_t_divide){
            popToken2 = pop(&stack);
            if(isEmpty(&stack)){
                *error = err_semantic;
                fprintf(stderr, "ERROR: / není unární operátor\n");
                return NULL;
            }
            popToken = pop(&stack);
            switch (popToken->type)
            {
            case tok_t_sym:
                result_data = find(result_data, sym_list, popToken, symtable, error);
                if(*error != err_none)
                    return NULL;
                check_operator(result_data, result, error);
                if(*error != err_none)
                    return NULL;

                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error);
                    if(*error != err_none)
                        return NULL;
                    check_operator(result_data2, result, error);
                    if(*error != err_none)
                        return NULL;

                    if(result_data->type != result_data2->type){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_int){
                    if(result_data->type != DATA_TYPE_INT){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_flt){
                    if(result_data->type != DATA_TYPE_DOUBLE){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    push(&stack, popToken);
                }
                else{
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: +, -, * nepodporuje dané datové typy\n");
                    return NULL;
                }
                break;
            
            case tok_t_int:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error);
                    if(*error != err_none)
                        return NULL;
                    check_operator(result_data2, result, error);
                    if(*error != err_none)
                        return NULL;

                    if(result_data2->type != DATA_TYPE_INT){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    push(&stack, popToken2);
                }
                else if(popToken2->type == tok_t_flt){
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                    return NULL;
                }
                else if(popToken2->type == tok_t_int){
                    push(&stack, popToken);
                }
                else{
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: / nepodporuje dané datové typy\n");
                    return NULL;
                }
                break;

            case tok_t_flt:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error);
                    if(*error != err_none)
                        return NULL;
                    check_operator(result_data2, result, error);
                    if(*error != err_none)
                        return NULL;

                    if(result_data2->type != DATA_TYPE_DOUBLE){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    push(&stack, popToken2);
                }
                else if(popToken2->type == tok_t_int){
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                    return NULL;
                }
                else if(popToken2->type == tok_t_flt){
                    push(&stack, popToken);
                }
                else{
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: / nepodporuje dané datové typy\n");
                    return NULL;
                }
                break;
            
            default:
                *error = err_dt_invalid;
                fprintf(stderr, "ERROR: / nepodporuje dané datové typy\n");
                return NULL;
                break;
            }
            continue;
        }

        if(postfix[i]->type == tok_t_neq || postfix[i]->type == tok_t_eq || postfix[i]->type == tok_t_lt || postfix[i]->type == tok_t_gt || postfix[i]->type == tok_t_leq || postfix[i]->type == tok_t_geq){
            popToken2 = pop(&stack);
            if(isEmpty(&stack)){
                *error = err_semantic;
                fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= není unární operátor\n");
                return NULL;
            }
            popToken = pop(&stack);

            switch (popToken->type)
            {
            case tok_t_sym:
                result_data = find(result_data, sym_list, popToken, symtable, error);
                if(*error != err_none)
                    return NULL;
                check_operator2(result_data, result, error);
                if(*error != err_none)
                    return NULL;

                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error);
                    if(*error != err_none)
                        return NULL;
                    check_operator2(result_data2, result, error);
                    if(*error != err_none)
                        return NULL;
                    if(result_data->type != result_data2->type){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n"); 
                        return NULL;
                    }
                    result->type = DATA_TYPE_BOOLEAN;
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else if(1){
                    if(popToken2->type != tok_t_int && popToken2->type != tok_t_flt && popToken2->type != tok_t_bool){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    result->type = DATA_TYPE_BOOLEAN;
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_flt){
                    if(result_data->type != DATA_TYPE_DOUBLE){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    result->type = DATA_TYPE_BOOLEAN;
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_true || popToken2->type == tok_t_false){
                    if(result_data->type != DATA_TYPE_BOOLEAN){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    result->type = DATA_TYPE_BOOLEAN;
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_null){
                    if(!result_data->canNull){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    result->type = DATA_TYPE_BOOLEAN;
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else{
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nepodporuje dané datové typy\n");
                    return NULL;
                }
                break;
            
            case tok_t_int:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error);
                    if(*error != err_none)
                        return NULL;
                    check_operator2(result_data2, result, error);
                    if(*error != err_none)
                        return NULL;

                    if(result_data2->type != DATA_TYPE_INT){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    result->type = DATA_TYPE_BOOLEAN;
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_flt || popToken2->type == tok_t_int){
                    result->type = DATA_TYPE_BOOLEAN;
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else{
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nepodporuje dané datové typy\n");
                    return NULL;
                }
                break;
            
            case tok_t_flt:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error);
                    if(*error != err_none)
                        return NULL;
                    check_operator2(result_data2, result, error);
                    if(*error != err_none)
                        return NULL;

                    if(result_data2->type != DATA_TYPE_DOUBLE){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    result->type = DATA_TYPE_BOOLEAN;
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_flt || popToken2->type == tok_t_int){
                    result->type = DATA_TYPE_BOOLEAN;
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else{
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nepodporuje dané datové typy\n");
                    return NULL;
                }
                break;

            case tok_t_true:
            case tok_t_false:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error);
                    if(*error != err_none)
                        return NULL;
                    check_operator2(result_data2, result, error);
                    if(*error != err_none)
                        return NULL;

                    if(result_data2->type != DATA_TYPE_BOOLEAN){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_true || popToken2->type == tok_t_false){
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else{
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nepodporuje dané datové typy\n");
                    return NULL;
                }
                break;
            case tok_t_null:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error);
                    if(*error != err_none)
                        return NULL;
                    check_operator2(result_data2, result, error);
                    if(*error != err_none)
                        return NULL;

                    if(!result_data2->canNull){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    result->type = DATA_TYPE_BOOLEAN;
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else{
                    if(popToken2->type != tok_t_null){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nepodporuje dané datové typy\n");
                        return NULL;
                    }
                }    
                break;
            default:
                *error = err_dt_invalid;
                fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nepodporuje dané datové typy\n");
                return NULL;
                break;
            }
            result->canNull = false;
            continue;
        }

        if(postfix[i]->type == tok_t_and || postfix[i]->type == tok_t_or){
            popToken2 = pop(&stack);
            if(isEmpty(&stack)){
                *error = err_semantic;
                fprintf(stderr, "ERROR: and, or není unární operátor\n");
                return NULL;
            }
            popToken = pop(&stack);

            if(popToken->type != tok_t_bool || popToken2->type != tok_t_bool){
                if((popToken->type != tok_t_true && popToken->type != tok_t_false) || (popToken2->type != tok_t_true && popToken2->type != tok_t_false)){
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: and, or nepodporuje dané datové typy\n");
                    return NULL;
                }
            }

            if(popToken->type == tok_t_sym){
                result_data = find(result_data, sym_list, popToken, symtable, error);
                if(*error != err_none)
                    return NULL;

                if(result_data->type != DATA_TYPE_BOOLEAN){
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: and, or nelze emplicitně přetypovat\n");
                    return NULL;
                }
                result_data->used = true;
            }

            if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error);
                    if(*error != err_none)
                        return NULL;

                    if(result_data2->type != DATA_TYPE_BOOLEAN){
                        *error = err_dt_invalid;
                        fprintf(stderr, "ERROR: and, or nelze emplicitně přetypovat\n");
                        return NULL;
                    }
                    result_data2->used = true;
                }

            popToken->type = tok_t_bool;
            push(&stack, popToken);
            result->canNull = false;
            continue;
        }

        if(postfix[i]->type == tok_t_not){
            popToken = pop(&stack);

            if(popToken->type != tok_t_bool){
                // opravit
                if((popToken->type != tok_t_true && popToken->type != tok_t_false) || (popToken2->type != tok_t_true && popToken2->type != tok_t_false)){
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: not nepodporuje dané datové typy\n");
                    return NULL;
                }
            }

            if(popToken->type == tok_t_sym){
                result_data = find(result_data, sym_list, popToken, symtable, error);
                if(*error != err_none)
                    return NULL;

                if(result_data->type != DATA_TYPE_BOOLEAN){
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: not nelze emplicitně přetypovat\n");
                    return NULL;
                }
                result_data->used = true;
            }

            push(&stack, popToken);
            result->canNull = false;
            continue;
        }

        if(postfix[i]->type == tok_t_orelse){
            popToken2 = pop(&stack);
            if(isEmpty(&stack)){
                *error = err_semantic;
                fprintf(stderr, "ERROR: orElse není unární operátor\n");
                return NULL;
            }
            popToken = pop(&stack);

            if(popToken->type != tok_t_sym){
                *error = err_dt_invalid;
                fprintf(stderr, "ERROR: orElse nepodporuje dané datové typy\n");
                return NULL;
            }

            result_data = find(result_data, sym_list, popToken, symtable, error);
            if(*error != err_none)
                return NULL;
            result_data->used = true;

            if(!result_data->canNull){
                *error = err_dt_invalid;
                fprintf(stderr, "ERROR: ORELSE musí být null\n");
                return NULL;
            }

            if(popToken2->type == tok_t_null){
                result->canNull = true;
                result->type = result_data->type;
            }
            else if(popToken2->type == tok_t_int){
                if(result_data->type != DATA_TYPE_INT){
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: orElse nepodporuje dané datové typy\n");
                    return NULL;
                }

                result->canNull = false;
                result->type = DATA_TYPE_INT;
            }
            else if(popToken2->type == tok_t_flt){
                if(result_data->type != DATA_TYPE_DOUBLE){
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: orElse nepodporuje dané datové typy\n");
                    return NULL;
                }

                result->canNull = false;
                result->type = DATA_TYPE_DOUBLE;
            }
            else if(popToken2->type == tok_t_unreach){
                result->canNull = false;
                result->type = result_data->type;
            }
            else if (popToken2->type == tok_t_sym){
                if(result_data->type != DATA_TYPE_INT && result_data->type != DATA_TYPE_DOUBLE && result_data->type != DATA_TYPE_U8){
                    *error = err_dt_invalid;
                    fprintf(stderr, "ERROR: orElse nepodporuje dané datové typy\n");
                    return NULL;
                }
                result_data->used = true;
                result->type = result_data->type;
                result->canNull = result_data->canNull;
            }
            continue;
        }

        if(postfix[i]->type == tok_t_orelse_un){  //zmněnit token
            popToken = pop(&stack);

            if(popToken->type != tok_t_sym){
                *error = err_dt_invalid;
                fprintf(stderr, "ERROR: unreachable nepodporuje dané datové typy\n");
                return NULL;
            }

            result_data = find(result_data, sym_list, popToken, symtable, error);
            if(*error != err_none)
                return NULL;
            result_data->used = true;

            if(!result_data->canNull){
                *error = err_dt_invalid;
                fprintf(stderr, "ERROR: UNREACHABLE musí být nullovatelný typ\n");
                return NULL;
            }

            result->canNull = false;
            result->type = result_data->type;
            continue;
        }
    }
    return result;
}
