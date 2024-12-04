/** 
 * @addtogroup IFJ2024
 * @file expression.c
 * @brief Kontrola datových typů ve výrazech.
 * @author xsidlil00; Lukáš Šidlík
 * 
 * Knihovna pro kontrolu datových typů ve výrazech.
 */

#include "expression.h"
#include "errors.h"
#include "memory_table.h"

/**
 * @brief Kontrola jestli se vyskytl error pro funkce bez void.
 */
#define OK if (*error != err_none) return NULL

/**
 * @brief Kontrola jestli se vyskytl error pro funkce void.
 */
#define OK2 if (*error != err_none) return 

// nalezení prvku ze symbolické tabulky
data_t *find(data_t *result_data, DLList sym_list, Token_ptr popToken, symtable_t *symtable, err_codes *error){
    DLL_Last(&sym_list);
    while(sym_list.current != NULL){
        symtable = DLL_GetCurrent(&sym_list);
        result_data = symtable_get_item(symtable, popToken->attribute, error); OK;
        if(result_data != NULL){
            break;
        }
        DLL_Prev(&sym_list);
    }

    if(result_data == NULL){
        fprintf(stderr, "Semantic error: %s is undefined\n", popToken->attribute);
        *error = err_undef;
        return NULL;
    }
    return result_data;
}

// návrat chyby typové nekombality
err_codes error_found(){
    fprintf(stderr, "Semantic error: incompatible data types\n");
    return err_dt_invalid;
}

// kontrola datovéhoho typu
void check_type(data_t *result_data, data_type_t type, err_codes *error){
    if(result_data->type != type){
        *error = error_found();
        return;
    }
}

// kontrola dvou datových typů
void check_two_types(data_t *result_data, data_type_t type, data_type_t type2, err_codes *error){
    if(result_data->type != type && result_data->type != type2){
        *error = error_found();
        return;
    }
}

// daná proměnná nesmí nabývat NULL
void check_not_canNull(data_t *result_data, err_codes *error){
    if(result_data->canNull){
        *error = error_found();
        return;
    }
}

// daná proměnná musí nabývat NULL
void check_canNull(data_t *result_data, err_codes *error){
    if(!result_data->canNull){
        *error = error_found();
        return;
    }
}

// kontrola prměnné pro aritmetické operace
void check_operator_calc(data_t *result_data, err_codes *error){
    check_not_canNull(result_data, error); OK2;
    check_two_types(result_data, DATA_TYPE_INT, DATA_TYPE_DOUBLE, error); OK2;
    result_data->used = true;
}

// kontrola dvou proměnných, jestli mají stejný datový typ
void check_second_symbol(data_t *result_data, data_t *result_data2, err_codes *error){
    if(result_data->type != result_data2->type){
        *error = error_found();
        return;
    }  
}

// kontrola validního datového typu pro logické operace
void check_operator_bool(data_t *result_data, data_t *result, err_codes *error){
    if(result_data->type != DATA_TYPE_INT && result_data->type != DATA_TYPE_DOUBLE && result_data->type != DATA_TYPE_BOOLEAN){
        *error = error_found();
        return;
    }
    result_data->used = true;
}

// kontrola prvního symbolu v ligickém výrazu
data_t *check_bool_first(data_t *result_data, Token_ptr popToken, DLList sym_list, symtable_t *symtable, err_codes *error){
    result_data = find(result_data, sym_list, popToken, symtable, error); OK;
    check_type(result_data, DATA_TYPE_BOOLEAN, error); OK;
    result_data->used = true;
    return result_data;
}

// kontrola druheho symbolu v logickém výrazu
void check_bool_second(data_t *result_data, Token_ptr popToken, DLList sym_list, symtable_t *symtable, err_codes *error){
    if(popToken->type == tok_t_sym){
        result_data = find(result_data, sym_list, popToken, symtable, error); OK2;

        check_type(result_data, DATA_TYPE_BOOLEAN, error); OK2;
        result_data->used = true;
    }
    else if(popToken->type != tok_t_bool && popToken->type != tok_t_true && popToken->type != tok_t_false){
        *error = error_found();
        return;
    }
}

// kontrola logického výrazu
void check_bool(data_t *result_data, data_t *result_data2, Token_ptr popToken, Token_ptr popToken2, DLList sym_list, symtable_t *symtable, data_t *result, err_codes *error){
    switch (popToken->type)
    {
    case tok_t_sym:
        result_data = find(result_data, sym_list, popToken, symtable, error); OK2;
        check_operator_bool(result_data, result, error); OK2;

        if(popToken2->type == tok_t_sym){
            result_data2 = find(result_data2, sym_list, popToken2, symtable, error); OK2;
            check_operator_bool(result_data2, result, error); OK2;
            check_second_symbol(result_data, result_data2, error); OK2;
        }
        else if(popToken2->type == tok_t_int){
            check_two_types(result_data, DATA_TYPE_INT, DATA_TYPE_DOUBLE, error); OK2;
        }
        else if(popToken2->type == tok_t_flt){     
            check_two_types(result_data, DATA_TYPE_INT, DATA_TYPE_DOUBLE, error); OK2;
        }
        else if(popToken2->type == tok_t_true || popToken2->type == tok_t_false || popToken2->type == tok_t_bool){
            check_type(result_data, DATA_TYPE_BOOLEAN, error); OK2;
        }
        else if(popToken2->type == tok_t_null){
            check_canNull(result_data, error); OK2;
        }
        else{
            *error = error_found();
            return;
        }
        break;
    
    case tok_t_int:
        if(popToken2->type == tok_t_sym){
            result_data2 = find(result_data2, sym_list, popToken2, symtable, error); OK2;
            check_operator_bool(result_data2, result, error); OK2;
            check_two_types(result_data2, DATA_TYPE_INT, DATA_TYPE_DOUBLE, error); OK2;
        }
        else if(popToken2->type != tok_t_flt && popToken2->type != tok_t_int){
            *error = error_found();
            return;
        }
        break;
    
    case tok_t_flt:
        if(popToken2->type == tok_t_sym){
            result_data2 = find(result_data2, sym_list, popToken2, symtable, error); OK2;
            check_operator_bool(result_data2, result, error); OK2;
            check_two_types(result_data2, DATA_TYPE_INT, DATA_TYPE_DOUBLE, error); OK2;
        }
        else if(popToken2->type != tok_t_flt && popToken2->type != tok_t_int){
            *error = error_found();
            return;
        }
        break;

    case tok_t_true:
    case tok_t_false:
        if(popToken2->type == tok_t_sym){
            result_data2 = find(result_data2, sym_list, popToken2, symtable, error); OK2;
            check_operator_bool(result_data2, result, error); OK2;
            check_type(result_data2, DATA_TYPE_BOOLEAN, error); OK2;
        }
        else if(popToken2->type != tok_t_true && popToken2->type != tok_t_false){
            *error = error_found();
            return;
        }
        break;
    case tok_t_null:
        if(popToken2->type == tok_t_sym){
            result_data2 = find(result_data2, sym_list, popToken2, symtable, error); OK2;
            check_operator_bool(result_data2, result, error); OK2;
            check_canNull(result_data2, error);
        }
        else if(popToken2->type != tok_t_null){
            *error = error_found();
            return;
        }    
        break;
    case tok_t_bool:
        if(popToken2->type == tok_t_sym){
            result_data2 = find(result_data2, sym_list, popToken2, symtable, error); OK2;
            check_operator_bool(result_data2, result, error); OK2;
            check_type(result_data2, DATA_TYPE_BOOLEAN, error); OK2;
        }
        else if(popToken2->type != tok_t_bool && popToken2->type != tok_t_true && popToken2->type != tok_t_false){
            *error = error_found();
            return;
        }
        break;
    default:
        *error = error_found();
        return;
        break;
    }
}

// kontrola prvního symbolu v logickém výrazu
Token_ptr check_and_or(Token_ptr popToken, Token_ptr popToken2, data_t *result_data, DLList sym_list, symtable_t *symtable, err_codes *error){
    switch (popToken->type)
    {
    case tok_t_sym:
        result_data = find(result_data, sym_list, popToken, symtable, error); OK;
        result_data = check_bool_first(result_data, popToken, sym_list, symtable, error); OK;
        check_bool_second(result_data, popToken2, sym_list, symtable, error); OK;
        break;

    case tok_t_bool:
    case tok_t_true:
    case tok_t_false:
        check_bool_second(result_data, popToken2, sym_list, symtable, error); OK;
        break;
    
    default:
        *error = error_found();
        return NULL;
        break;
    }
    return popToken;
}

// kontrola prvního symbolu v orelse operace
data_t *check_first_orelse(Token_ptr popToken, data_t *result_data, DLList sym_list, symtable_t *symtable, err_codes *error){
    if(popToken->type != tok_t_sym){
        *error = error_found();
        return NULL;
    }

    result_data = find(result_data, sym_list, popToken, symtable, error); OK;
    result_data->used = true;

    check_canNull(result_data, error); OK;
    return result_data;
}

// kontrola druheho symbolu v orelse operace
data_t *check_second_orelse(data_t *result_data, data_t *result_data2, Token_ptr popToken2, DLList sym_list, symtable_t *symtable, err_codes *error){
    if(result_data->type != DATA_TYPE_INT && result_data->type != DATA_TYPE_DOUBLE && result_data->type != DATA_TYPE_U8){
        *error = error_found();
        return NULL;
    }

    result_data2 = find(result_data2, sym_list, popToken2, symtable, error); OK;
    check_second_symbol(result_data, result_data2, error); OK;
    return result_data2;
}

// odvození typu z unreachable
Token_ptr unreachble_type(Token_ptr popToken, data_t *result, err_codes *error){
    if(result->type == DATA_TYPE_INT){
        popToken->type = tok_t_int;
    }
    else if(result->type == DATA_TYPE_DOUBLE){
        popToken->type = tok_t_flt;
    }
    else if(result->type == DATA_TYPE_U8){
        popToken->type = tok_t_u8;
    }
    else{
        *error = error_found();
        return NULL;
    }
    return popToken;
}

// odvození potenciálního výsledku
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
        sym = find(result, sym_list, popToken, symtable, error); OK;
        sym->used = true;
        result->type = sym->type;
        result->canNull = sym->canNull;
        return result;
    }
}

// funkce s kontrolou datových typů z postfixového výrazu.
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
    // odvození potenciálního výsledku
    result = resultType(result, postfix[0], symtable, sym_list, error); OK;

    if(result == NULL){
        *error = err_internal;
        return NULL;
    }

    stack_init(&stack);
    for (int i = 0; i < postfix_index; i++)
    {
        // operandy
        if(postfix[i]->type == tok_t_sym || postfix[i]->type == tok_t_int || postfix[i]->type == tok_t_flt || 
           postfix[i]->type == tok_t_true || postfix[i]->type == tok_t_false || postfix[i]->type == tok_t_null ||
           postfix[i]->type == tok_t_unreach)
        {
            stack_push(&stack, postfix[i]);
            continue;
        }

        // sčítání, odčítání, násobení
        if(postfix[i]->type == tok_t_plus || postfix[i]->type == tok_t_minus || postfix[i]->type == tok_t_times){
            popToken2 = stack_pop(&stack);
            popToken = stack_pop(&stack);

            switch (popToken->type)
            {
            case tok_t_sym:
                result_data = find(result_data, sym_list, popToken, symtable, error); OK;
                check_operator_calc(result_data, error); OK;

                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error); OK;
                    check_operator_calc(result_data2, error); OK;
                    check_second_symbol(result_data, result_data2, error);
                    stack_push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_int){
                    check_two_types(result_data, DATA_TYPE_INT, DATA_TYPE_DOUBLE, error); OK;
                    stack_push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_flt){
                    check_two_types(result_data, DATA_TYPE_INT, DATA_TYPE_DOUBLE, error); OK;
                    stack_push(&stack, popToken);
                }
                else{
                    *error = error_found();
                    return NULL;
                }
                
                break;

            case tok_t_int:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error); OK;
                    check_operator_calc(result_data2, error); OK;
                    check_two_types(result_data2, DATA_TYPE_INT, DATA_TYPE_DOUBLE, error); OK;
                    stack_push(&stack, popToken2);
                }
                else if(popToken2->type == tok_t_flt){
                    result->type = DATA_TYPE_DOUBLE;
                    stack_push(&stack, popToken2);
                }
                else if(popToken2->type == tok_t_int){
                    stack_push(&stack, popToken);
                }
                else{
                    *error = error_found();
                    return NULL;
                }

                break;

            case tok_t_flt:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error); OK;
                    check_operator_calc(result_data2, error); OK;
                    check_two_types(result_data2, DATA_TYPE_INT, DATA_TYPE_DOUBLE, error); OK;
                    stack_push(&stack, popToken2);
                }
                else if(popToken2->type == tok_t_int || popToken2->type == tok_t_flt){
                    stack_push(&stack, popToken);
                }
                else{
                    *error = error_found();
                    return NULL;
                }
                break;
            
            default:
                *error = error_found();
                return NULL;
                break;
            }
            continue;
        }

        // dělení
        if(postfix[i]->type == tok_t_divide){
            popToken2 = stack_pop(&stack);
            popToken = stack_pop(&stack);
            switch (popToken->type)
            {
            case tok_t_sym:
                result_data = find(result_data, sym_list, popToken, symtable, error); OK;
                check_operator_calc(result_data, error); OK;

                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error); OK;
                    check_operator_calc(result_data2, error); OK;
                    check_second_symbol(result_data, result_data2, error); OK;
                    stack_push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_int){
                    check_type(result_data, DATA_TYPE_INT, error); OK;
                    stack_push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_flt){
                    check_type(result_data, DATA_TYPE_DOUBLE, error); OK;
                    stack_push(&stack, popToken);
                }
                else{
                    *error = error_found();
                    return NULL;
                }
                break;
            
            case tok_t_int:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error); OK;
                    check_operator_calc(result_data2, error); OK;
                    check_type(result_data2, DATA_TYPE_INT, error); OK;
                    stack_push(&stack, popToken2);
                }
                else if(popToken2->type == tok_t_flt){
                    *error = error_found();
                    return NULL;
                }
                else if(popToken2->type == tok_t_int){
                    stack_push(&stack, popToken);
                }
                else{
                    *error = error_found();
                    return NULL;
                }
                break;

            case tok_t_flt:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable, error); OK;
                    check_operator_calc(result_data2, error); OK;
                    check_type(result_data2, DATA_TYPE_DOUBLE, error); OK;
                    stack_push(&stack, popToken2);
                }
                else if(popToken2->type == tok_t_int){
                    *error = error_found();
                    return NULL;
                }
                else if(popToken2->type == tok_t_flt){
                    stack_push(&stack, popToken);
                }
                else{
                    *error = error_found();
                    return NULL;
                }
                break;
            
            default:
                *error = error_found();
                return NULL;
                break;
            }
            continue;
        }

        // relační operátory
        if(postfix[i]->type == tok_t_neq || postfix[i]->type == tok_t_eq || postfix[i]->type == tok_t_lt || 
           postfix[i]->type == tok_t_gt || postfix[i]->type == tok_t_leq || postfix[i]->type == tok_t_geq){
            popToken2 = stack_pop(&stack);
            popToken = stack_pop(&stack);
            check_bool(result_data, result_data2, popToken, popToken2, sym_list, symtable, result, error); OK;
            result->canNull = false;
            result->type = DATA_TYPE_BOOLEAN;
            popToken->type = tok_t_bool;
            stack_push(&stack, popToken);
            continue;
        }

        // logické operátory
        if(postfix[i]->type == tok_t_and || postfix[i]->type == tok_t_or){
            popToken2 = stack_pop(&stack);
            popToken = stack_pop(&stack);
            popToken = check_and_or(popToken, popToken2, result_data, sym_list, symtable, error); OK;
            popToken->type = tok_t_bool;
            stack_push(&stack, popToken);
            result->canNull = false;
            continue;
        }

        // negace
        if(postfix[i]->type == tok_t_not){
            popToken = stack_pop(&stack);
            check_bool_second(result_data, popToken, sym_list, symtable, error); OK;
            stack_push(&stack, popToken);
            result->canNull = false;
            continue;
        }

        // orelse
        if(postfix[i]->type == tok_t_orelse){
            popToken2 = stack_pop(&stack);
            popToken = stack_pop(&stack);
            result_data = check_first_orelse(popToken, result_data, sym_list, symtable, error); OK;

            if(popToken2->type == tok_t_null){
                result->canNull = true;
                result->type = result_data->type;
                stack_push(&stack, popToken);
            }
            else if(popToken2->type == tok_t_int){
                check_type(result_data, DATA_TYPE_INT, error); OK;
                result->canNull = false;
                result->type = DATA_TYPE_INT;
                stack_push(&stack, popToken2);
            }
            else if(popToken2->type == tok_t_flt){
                check_type(result_data, DATA_TYPE_DOUBLE, error); OK;
                result->canNull = false;
                result->type = DATA_TYPE_DOUBLE;
                stack_push(&stack, popToken2);
            }
            else if(popToken2->type == tok_t_unreach){
                result->canNull = false;
                result->type = result_data->type;
                if(result_data->type == DATA_TYPE_INT){
                    popToken->type = tok_t_int;
                    result->type = DATA_TYPE_INT;
                    result->canNull = false;
                    stack_push(&stack, popToken);
                }   
                else if(result_data->type == DATA_TYPE_DOUBLE){
                    popToken->type = tok_t_flt;
                    result->type = DATA_TYPE_DOUBLE;
                    result->canNull = false;
                    stack_push(&stack, popToken);
                }
                else if(result_data->type == DATA_TYPE_U8){
                    popToken->type = tok_t_u8;
                    result->type = DATA_TYPE_U8;
                    result->canNull = false;
                    stack_push(&stack, popToken);
                }
            }
            else if (popToken2->type == tok_t_sym){
                result_data2 = check_second_orelse(result_data, result_data2, popToken2, sym_list, symtable, error); OK;
                result_data2->used = true;
                result->type = result_data2->type;
                result->canNull = result_data2->canNull;
                stack_push(&stack, popToken2);
            }
            continue;
        }

        //unreachble
        if(postfix[i]->type == tok_t_orelse_un){  
            popToken = stack_pop(&stack);
            result_data = check_first_orelse(popToken, result_data, sym_list, symtable, error); OK;
            result->canNull = false;
            result->type = result_data->type;
            popToken = unreachble_type(popToken, result_data, error); OK;
            stack_push(&stack, popToken);
            continue;
        }
    }

    stack_pop(&stack);
    return result;
}

/*** Konec souboru expression.c ***/