/** IFJ2024
 * xsidlil00; Lukáš Šidlík
 */

#include "expresion.h"

data_t *find(data_t *result_data, DLList sym_list, Token_ptr popToken, symtable_t *symtable){
    DLL_Last(&sym_list);
    while(sym_list.current != NULL){
        symtable = DLL_GetCurrent(&sym_list);
        result_data = symtable_get_item(symtable, popToken->attribute);
        if(result_data != NULL){
            break;
        }
        DLL_Prev(&sym_list);
    }

    if(result_data == NULL){
        fprintf(stderr, "ERROR: nenalezen v symtable\n");
        exit(2);
        // chyba -> nenalezení v symtable
    }
    return result_data;
}

void check_operator(data_t *result_data, data_t *result){
    if(result_data->canNull){
        fprintf(stderr, "ERROR: +,-,*,/ nemůže být null\n");
        exit(2);
    }

    if(result_data->type != DATA_TYPE_INT && result_data->type != DATA_TYPE_DOUBLE){
       fprintf(stderr, "ERROR: +,-,*,/ nelze emplicitně přetypovat\n");
        exit(2);
    }

    result_data->used = true;
}

void check_operator2(data_t *result_data, data_t *result){
    if(result_data->canNull){
        fprintf(stderr, "ERROR: ==, !=, <, <=, >, >= nemůže být null\n");
        exit(2);
    }

    if(result_data->type != DATA_TYPE_INT && result_data->type != DATA_TYPE_DOUBLE && result_data->type != DATA_TYPE_BOOLEAN){
        fprintf(stderr, "ERROR: ==, !=, <, <=, >, >= nelze emplicitně přetypovat\n");
        exit(2);
    }
    result_data->used = true;
}

data_t* resultType(data_t *result, Token_ptr popToken, symtable_t *symtable, DLList sym_list){
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
        result = find(result, sym_list, popToken, symtable);
        return result;
    }
}

data_t* postfix_semantic(Token_ptr *postfix, int postfix_index, DLList sym_list, symtable_t *symtable, bool *convert){
    Stack stack;
    data_t *result;
    data_t *result_data;
    data_t *result_data2;
    Token_ptr popToken;
    Token_ptr popToken2;
    *convert = false;

    result = malloc(sizeof(data_t));
    if (result == NULL)
    {
        // vnitřní chyba
    }

    result_data2 = malloc(sizeof(data_t));
    if (result_data2 == NULL)
    {
        // vnitřní chyba
    }

    result->type = DATA_TYPE_UND;
    result = resultType(result, postfix[0], symtable, sym_list);

    if(result->type == DATA_TYPE_INT){
        *convert = true;
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
                fprintf(stderr, "ERROR: +,-,* není unární operátor\n");
                exit(2);
            }
            popToken = pop(&stack);

            switch (popToken->type)
            {
            case tok_t_sym:
                result_data = find(result_data, sym_list, popToken, symtable);
                check_operator(result_data, result);

                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable);
                    check_operator(result_data2, result);

                    if(result_data->type != result_data2->type){
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        exit(2);
                    }
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_int){
                    if(result_data->type != DATA_TYPE_INT){
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        exit(2);
                    }
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_flt){
                    if(result_data->type != DATA_TYPE_DOUBLE){
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        exit(2);
                    }
                    push(&stack, popToken);
                }
                else{
                    fprintf(stderr, "ERROR: +, -, * nepodporuje dané datové typy\n");
                    exit(2);
                }
                
                break;

            case tok_t_int:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable);
                    check_operator(result_data2, result);

                    if(result_data2->type != DATA_TYPE_INT){
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        exit(2);
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
                    fprintf(stderr, "ERROR: +, -, * nepodporuje dané datové typy\n");
                    exit(2);
                }

                break;

            case tok_t_flt:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable);
                    check_operator(result_data2, result);

                    if(result_data2->type != DATA_TYPE_DOUBLE){
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        exit(2);
                    }
                    push(&stack, popToken2);
                }
                else if(popToken2->type == tok_t_int || popToken2->type == tok_t_flt){
                    push(&stack, popToken);
                }
                else{
                    fprintf(stderr, "ERROR: +, -, * nepodporuje dané datové typy\n");
                    exit(2);
                }
                break;
            
            default:
                fprintf(stderr, "ERROR: +, -, * nepodporuje dané datové typy\n");
                exit(2);
                break;
            }
            continue;
        }

        if(postfix[i]->type == tok_t_divide){
            popToken2 = pop(&stack);
            if(isEmpty(&stack)){
                fprintf(stderr, "ERROR: / není unární operátor\n");
                exit(2);
            }
            popToken = pop(&stack);
            switch (popToken->type)
            {
            case tok_t_sym:
                result_data = find(result_data, sym_list, popToken, symtable);
                check_operator(result_data, result);

                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable);
                    check_operator(result_data2, result);

                    if(result_data->type != result_data2->type){
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        exit(2);
                    }
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_int){
                    if(result_data->type != DATA_TYPE_INT){
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        exit(2);
                    }
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_flt){
                    if(result_data->type != DATA_TYPE_DOUBLE){
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        exit(2);
                    }
                    push(&stack, popToken);
                }
                else{
                    fprintf(stderr, "ERROR: +, -, * nepodporuje dané datové typy\n");
                    exit(2);
                }
                break;
            
            case tok_t_int:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable);
                    check_operator(result_data2, result);

                    if(result_data2->type != DATA_TYPE_INT){
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        exit(2);
                    }
                    push(&stack, popToken2);
                }
                else if(popToken2->type == tok_t_flt){
                    fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                    exit(2);
                }
                else if(popToken2->type == tok_t_int){
                    push(&stack, popToken);
                }
                else{
                    fprintf(stderr, "ERROR: / nepodporuje dané datové typy\n");
                    exit(2);
                }
                break;

            case tok_t_flt:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable);
                    check_operator(result_data2, result);

                    if(result_data2->type != DATA_TYPE_DOUBLE){
                        fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                        exit(2);
                    }
                    push(&stack, popToken2);
                }
                else if(popToken2->type == tok_t_int){
                    fprintf(stderr, "ERROR: nelze emplicitně přetypovat\n");
                    exit(2);
                }
                else if(popToken2->type == tok_t_flt){
                    push(&stack, popToken);
                }
                else{
                    fprintf(stderr, "ERROR: / nepodporuje dané datové typy\n");
                    exit(2);
                }
                break;
            
            default:
                fprintf(stderr, "ERROR: / nepodporuje dané datové typy\n");
                exit(2);
                break;
            }
            continue;
        }

        if(postfix[i]->type == tok_t_neq || postfix[i]->type == tok_t_eq || postfix[i]->type == tok_t_lt || postfix[i]->type == tok_t_gt || postfix[i]->type == tok_t_leq || postfix[i]->type == tok_t_geq){
            popToken2 = pop(&stack);
            if(isEmpty(&stack)){
                fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= není unární operátor\n");
                exit(2);
            }
            popToken = pop(&stack);

            switch (popToken->type)
            {
            case tok_t_sym:
                result_data = find(result_data, sym_list, popToken, symtable);
                check_operator2(result_data, result);

                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable);
                    check_operator2(result_data2, result);
                    if(result_data->type != result_data2->type){
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n"); 
                        exit(2);
                    }
                    result->type = DATA_TYPE_BOOLEAN;
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_int){
                    if(result_data->type != DATA_TYPE_INT){
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n");
                        exit(2);
                    }
                    result->type = DATA_TYPE_BOOLEAN;
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_flt){
                    if(result_data->type != DATA_TYPE_DOUBLE){
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n");
                        exit(2);
                    }
                    result->type = DATA_TYPE_BOOLEAN;
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_true || popToken2->type == tok_t_false){
                    if(result_data->type != DATA_TYPE_BOOLEAN){
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n");
                        exit(2);
                    }
                    result->type = DATA_TYPE_BOOLEAN;
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else{
                    fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nepodporuje dané datové typy\n");
                    exit(2);
                }
                break;
            
            case tok_t_int:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable);
                    check_operator2(result_data2, result);

                    if(result_data2->type != DATA_TYPE_INT){
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n");
                        exit(2);
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
                    fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nepodporuje dané datové typy\n");
                    exit(2);
                }
                break;
            
            case tok_t_flt:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable);
                    check_operator2(result_data2, result);

                    if(result_data2->type != DATA_TYPE_DOUBLE){
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n");
                        exit(2);
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
                    fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nepodporuje dané datové typy\n");
                    exit(2);
                }
                break;

            case tok_t_true:
            case tok_t_false:
                if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable);
                    check_operator2(result_data2, result);

                    if(result_data2->type != DATA_TYPE_BOOLEAN){
                        fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nelze emplicitně přetypovat\n");
                        exit(2);
                    }
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else if(popToken2->type == tok_t_true || popToken2->type == tok_t_false){
                    popToken->type = tok_t_bool;
                    push(&stack, popToken);
                }
                else{
                    fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nepodporuje dané datové typy\n");
                    exit(2);
                }
                break;
            default:
                fprintf(stderr, "ERROR: ==, !=, <, >, <=, >= nepodporuje dané datové typy\n");
                exit(2);
                break;
            }
            result->canNull = false;
            continue;
        }

        if(postfix[i]->type == tok_t_and || postfix[i]->type == tok_t_or){
            popToken2 = pop(&stack);
            if(isEmpty(&stack)){
                fprintf(stderr, "ERROR: and, or není unární operátor\n");
                exit(2);
            }
            popToken = pop(&stack);

            if(popToken->type != tok_t_bool || popToken2->type != tok_t_bool){
                if((popToken->type != tok_t_true && popToken->type != tok_t_false) || (popToken2->type != tok_t_true && popToken2->type != tok_t_false)){
                    fprintf(stderr, "ERROR: and, or nepodporuje dané datové typy\n");
                    exit(2);
                }
            }

            if(popToken->type == tok_t_sym){
                result_data = find(result_data, sym_list, popToken, symtable);

                if(result_data->type != DATA_TYPE_BOOLEAN){
                    fprintf(stderr, "ERROR: and, or nelze emplicitně přetypovat\n");
                    exit(2);
                }
            }

            if(popToken2->type == tok_t_sym){
                    result_data2 = find(result_data2, sym_list, popToken2, symtable);

                    if(result_data2->type != DATA_TYPE_BOOLEAN){
                        fprintf(stderr, "ERROR: and, or nelze emplicitně přetypovat\n");
                        exit(2);
                    }
                }

            popToken->type = tok_t_bool;
            push(&stack, popToken);
            result->canNull = false;
            continue;
        }

        if(postfix[i]->type == tok_t_not){
            popToken = pop(&stack);

            if(popToken->type != tok_t_bool){
                if((popToken->type != tok_t_true && popToken->type != tok_t_false) || (popToken2->type != tok_t_true && popToken2->type != tok_t_false)){
                    fprintf(stderr, "ERROR: not nepodporuje dané datové typy\n");
                    exit(2);
                }
            }

            if(popToken->type == tok_t_sym){
                result_data = find(result_data, sym_list, popToken, symtable);

                if(result_data->type != DATA_TYPE_BOOLEAN){
                    fprintf(stderr, "ERROR: not nelze emplicitně přetypovat\n");
                    exit(2);
                }
            }

            push(&stack, popToken);
            result->canNull = false;
            continue;
        }

        if(postfix[i]->type == tok_t_orelse){
            popToken2 = pop(&stack);
            if(isEmpty(&stack)){
                fprintf(stderr, "ERROR: orElse není unární operátor\n");
                exit(2);
            }
            popToken = pop(&stack);

            if(popToken->type != tok_t_sym){
                fprintf(stderr, "ERROR: orElse nepodporuje dané datové typy\n");
                exit(1);
            }

            result_data = find(result_data, sym_list, popToken, symtable);
            result_data->used = true;

            if(!result_data->canNull){
                fprintf(stderr, "ERROR: ORELSE musí být null\n");
                exit(2);
            }

            if(popToken2->type == tok_t_null){
                result->canNull = true;
                result->type = result_data->type;
            }
            else if(popToken2->type == tok_t_int){
                if(result_data->type != DATA_TYPE_INT){
                    fprintf(stderr, "ERROR: orElse nepodporuje dané datové typy\n");
                    exit(2);
                }

                result->canNull = false;
                result->type = DATA_TYPE_INT;
            }
            else if(popToken2->type == tok_t_flt){
                if(result_data->type != DATA_TYPE_DOUBLE){
                    fprintf(stderr, "ERROR: orElse nepodporuje dané datové typy\n");
                    exit(2);
                }

                result->canNull = false;
                result->type = DATA_TYPE_DOUBLE;
            }
            else if(popToken2->type == tok_t_unreach){
                result->canNull = false;
                result->type = result_data->type;
            }
            else if (popToken2->type == tok_t_sym){
                if(result_data->type != DATA_TYPE_INT || result_data->type != DATA_TYPE_DOUBLE || result_data->type != DATA_TYPE_U8){
                    fprintf(stderr, "ERROR: orElse nepodporuje dané datové typy\n");
                    exit(2);
                }
                result->type = result_data->type;
                result->canNull = result_data->canNull;
            }
            continue;
        }

        if(postfix[i]->type == tok_t_orelse_un){  //zmněnit token
            popToken = pop(&stack);

            if(popToken->type != tok_t_sym){
                fprintf(stderr, "ERROR: unreachable nepodporuje dané datové typy\n");
                exit(2);
            }

            result_data = find(result_data, sym_list, popToken, symtable);
            result_data->used = true;

            if(!result_data->canNull){
                fprintf(stderr, "ERROR: UNREACHABLE musí být nullovatelný typ\n");
                exit(2);
            }

            result->canNull = false;
            result->type = result_data->type;
            continue;
        }
    }
    return result;
}
