/** 
 * @addtogroup IFJ2024
 * @file parser.c
 * @brief Implementace parseru
 * @author xondre16; Jozef Ondrejička
 * @author xsidlil00; Lukáš Šidlík
 * @author xhubacv00; Vojtech Hubacek
 * 
 * Implementace parseru.
 * Zpracování kontrola syntaxe zdrojového kódu.
 * Zpracování výrazů společně s precedenční analýzou.
 * Sémantické kontroly.
 * Částečná generace cílového kódu.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include "parser.h"
#include "../scanner/scanner.h"
#include "../utils/token.h"
#include "../utils/token_types.h"
#include "../utils/errors.h"
#include "../utils/codegen.h"
#include "../utils/stack.h"
#include "../utils/symtable.h"
#include "../utils/symDLList.h"
#include "../utils/str.h"
#include "../utils/dynamic_array.h"
#include "../utils/expression.h"
#include "../utils/memory_table.h"

/**
 * @def OK
 * @brief Makro pro kontrolu chybového kódu
 */
#define OK if (tools->error != err_none) return

/**
 * @def printi
 * @brief Makro pro výpis generovaných instrukcí
 * 
 * Definice proměnných jsou přednostně definovány již na počátku funkcí,
 * jelikož se proměnné nemohou deklarovat uvnitř cyklů.
 */
#define printi(source, ...) if (strcmp(source, format[_defvar]) == 0) \
                                printi_defvar(&tools->string, &tools->string_defvar, source, ##__VA_ARGS__); \
                            else \
                                str_append(&tools->string_tmp, source, ##__VA_ARGS__)

Scanner_ptr scanner = NULL;

/**
 * @brief Precedenční tabulka
*/ 
char precedence_table[12][12] = {
//     .?   !   * / orlse + -   r   and   or   (    )    ID   $
    { '=', ' ', '>', '>', '>', '>', '>', '>', ' ', '>', ' ', '>', }, // .?
    { ' ', '=', ' ', ' ', ' ', ' ', ' ', ' ', '=', ' ', '=', ' ', }, // !
    { ' ', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>', }, // * /
    { ' ', '<', '<', '>', '>', '>', '>', '>', '<', '>', '<', '>', }, // orelse
    { ' ', '<', '<', '<', '>', '>', '>', '>', '<', '>', '<', '>', }, // + -
    { ' ', '<', '<', '<', '<', ' ', '>', '>', '<', '>', '<', '>', }, // relations
    { ' ', '<', '<', '<', '<', '<', '>', '>', '<', '>', '<', '>', }, // and
    { ' ', '<', '<', '<', '<', '<', '<', '>', '<', '>', '<', '>', }, // or
    { ' ', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', ' ', }, // (
    { '=', ' ', '>', '>', '>', '>', '>', '>', ' ', '>', ' ', '>', }, // )
    { '=', ' ', '>', '>', '>', '>', '>', '>', ' ', '>', ' ', '>', }, // ID
    { ' ', '<', '<', '<', '<', '<', '<', '<', '<', ' ', '<', ' ', }  // $
};

/**
 * @brief Funkce pro získání indexu tokenu v precedenční tabulce
 * 
 * @param type Typ tokenu
 * @return Index elementu v precedenční tabulce
 */
int get_precedence_index(token_type type) {
    switch (type) {
        case tok_t_orelse_un: return 0;
        case tok_t_not: return 1; 
        case tok_t_times:
        case tok_t_divide: return 2;
        case tok_t_orelse: return 3;
        case tok_t_plus:
        case tok_t_minus: return 4;
        case tok_t_eq:
        case tok_t_neq:
        case tok_t_lt:
        case tok_t_gt:
        case tok_t_leq:
        case tok_t_geq: return 5;
        case tok_t_and: return 6;
        case tok_t_or: return 7;
        case tok_t_lpa: return 8;
        case tok_t_rpa: return 9;
        case tok_t_sym: return 10;
        case tok_t_eof: return 11;
        default: return type;
    }
}

/**
 * @brief Zpracování precedenční analýzy
 * 
 * Funkce zpracovává precedenční analýzu výrazu za použití precedenční tabulky.
 * 
 * @param tools Ukazatel na strukturu parser_tools_t
 * @param precedence Ukazatel na dynamické pole
 * @param input Typ tokenu
 */
void precedence_analysis(parser_tools_t* tools, dynamic_array_t *precedence, token_type input) {
    int action;
    do {
        // Získání precedence vrcholu zásobníku
        int precedence_top;
        for (int index = precedence->size - 1; index >= 0; index--) {
            if (precedence->data[index] != (int)'E') {
                precedence_top = precedence->data[index];
                break;
            }
        }

        // Získání akce z precedenční tabulky pro daný vstup a vrchol zásobníku
        action = precedence_table[get_precedence_index(precedence_top)][get_precedence_index(input)];
        switch (action) {
            case '<': //Shift
                dynamic_array_insert(precedence, input);
                for (int index = precedence->size - 2; index >= 0; index--) {
                    if (precedence->data[index] == precedence_top) {
                        precedence->data[index + 1] = (int)'<';
                        break;
                    } else {
                        precedence->data[index + 1] = precedence->data[index];
                    }
                }
                break;

            case '>': //Redukce
                if (precedence->data[precedence->size - 1] == (int)'<') {
                    fprintf(stderr, "Syntax error: Wrong expression.\n");
                    tools->error = err_syntax;
                    return;
                }
                bool without_value = true;
                for (int index = precedence->size - 1; index >= 0; index--) {
                    if (precedence->data[index] == (int)'<') {
                        precedence->data[index] = (int)'E';
                        break;
                    }
                    if (precedence->data[index] == tok_t_sym || precedence->data[index] == (int)'E') {
                        without_value = false;
                    } else if (precedence->data[index] == precedence_top && without_value && precedence_top != tok_t_rpa && precedence_top != tok_t_orelse_un) {
                        fprintf(stderr, "Syntax error: tools->Error in expression.\n");
                        tools->error = err_syntax;
                        return;
                    }
                    precedence->size--;
                    if (index == 0) {
                        fprintf(stderr, "Syntax error: tools->Error in expression.\n");
                        tools->error = err_syntax;
                        return;
                    }
                }
                if (without_value) {
                    fprintf(stderr, "Syntax error: tools->Error in expression.\n");
                    tools->error = err_syntax;
                    return;
                }
                break;

            case ' ': //Neexistující pravidlo
                if (action == ' ' && (input != tok_t_eof || precedence_top != tok_t_eof)) {
                    fprintf(stderr, "Syntax error: tools->Error in expression.\n");
                    tools->error = err_syntax;
                    return;
                } else if (precedence->size != 2) {
                    fprintf(stderr, "Syntax error: tools->Error in expression.\n");
                    tools->error = err_syntax;
                    return;
                }
                break;
        }
    } while (action == (int)'>');
    dynamic_array_insert(precedence, input);
}

/**
 * @brief Syntaktické zpracování výrazu a vytváření postfixové notace
 * 
 * Funkce zpracovává jednotlivé tokeny výrazu za použití precedenční analýzy
 * a zároveň vytváří postfixovou notaci výrazu pro náslendé zpracování
 * sémantickou analýzou a generátorem cílového kódu.
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 * @param postfix Ukazatel na pole tokenů
 * @param postfix_index Aktuální index pole tokenů
 * @param stack_functions Ukazatel na zásobník funkcí
 */
void expression_processing(parser_tools_t* tools, Token_ptr (*postfix)[MAX], int* postfix_index, dynamic_array_t* functions_retval) {
    Stack stack_postfix;
    stack_init(&stack_postfix);

    dynamic_array_t precedence;
    dynamic_array_init(&precedence);
    dynamic_array_insert(&precedence, tok_t_eof);

    int paren_count = 0;  // Sledovač počtu otevřených závorek
    bool expression_continue = true;

    // Zpracování jednotlivých tokenů výrazu
    while (tools->current_token->type != tok_t_semicolon && tools->current_token->type != tok_t_com && expression_continue) {
        switch (tools->current_token->type) {
            case tok_t_rpa: // )
                paren_count--;
                // Pokud je počet závorek menší než 0, tak končíme spracování podmínky
                if (paren_count < 0) {
                    expression_continue = false;
                    break;
                }
                precedence_analysis(tools, &precedence, tools->current_token->type); OK;

                // Inak pokračujeme v spracovaní výrazu
                while (!stack_isEmpty(&stack_postfix) && stack_peek(&stack_postfix)->type != tok_t_lpa) {
                    (*postfix)[(*postfix_index)++] = stack_pop(&stack_postfix);
                }
                stack_pop(&stack_postfix); // Discard the left parenthesis
                next_token(tools);

                // Vložení operátorů negace na konec postfixové notace, pokud existuje (avšak .? má přednost)
                while (!stack_isEmpty(&stack_postfix) && stack_peek(&stack_postfix)->type == tok_t_not && tools->current_token->type != tok_t_orelse_un) {
                    (*postfix)[(*postfix_index)++] = stack_pop(&stack_postfix);
                }
                break;

            case tok_t_lpa: // (
                precedence_analysis(tools, &precedence, tools->current_token->type); OK;
                paren_count++;
                stack_push(&stack_postfix, tools->current_token);
                next_token(tools);
                break;

            case tok_t_int: //1
            case tok_t_flt: //1.0
            case tok_t_null: // null
            case tok_t_unreach: // unreachable
            case tok_t_true: // true
            case tok_t_false: // false
                precedence_analysis(tools, &precedence, tok_t_sym); OK;
                (*postfix)[(*postfix_index)++] = tools->current_token;
                next_token(tools);

                // Vložení operátorů negace za operand (pokud existují nebo následujícím tokenem není .?)
                while (!stack_isEmpty(&stack_postfix) && stack_peek(&stack_postfix)->type == tok_t_not && tools->current_token->type != tok_t_orelse_un) {
                    (*postfix)[(*postfix_index)++] = stack_pop(&stack_postfix);
                }
                break;

            case tok_t_sym: // ID
                precedence_analysis(tools, &precedence, tok_t_sym); OK;
                (*postfix)[(*postfix_index)] = tools->current_token;
                stack_push(&tools->stack_codegen, tools->current_token);

                // Uchování informace současného tokenu
                Token_ptr func_token = tok_init(tools->current_token->type);
                strcpy(tools->string_buffer_semantic, tools->current_token->attribute);

                // Zpracování symbolu jakožto funkce nebo proměnné
                next_token(tools);
                id_continue(tools); OK;

                // Vyhledání symbolu v symtable a určení jeho typu
                DLL_First(&tools->sym_list);
                tools->current_symtable = DLL_GetCurrent(&tools->sym_list);
                data_t* data = symtable_get_item(tools->current_symtable, tools->string_buffer_semantic, &tools->error); OK;
                if (data != NULL) { // Jedná se o funkci
                    // Generuje se přesun navrácené hodnoty do unikátní proměnné a uložení pořadového čísla unikátní proměnné
                    sprintf(tools->string_buffer, "LF@%%retval%i", tools->counter_global);
                    printi(format[_defvar], tools->string_buffer);
                    printi(format[_move], tools->string_buffer, "TF@%retval");
                    dynamic_array_insert(functions_retval, tools->counter_global++);
                }
                DLL_Last(&tools->sym_list);
                tools->current_symtable = DLL_GetLast(&tools->sym_list);

                tok_set_attribute(func_token, tools->string_buffer_semantic);
                (*postfix)[(*postfix_index)] = func_token;
                (*postfix_index)++;
                tools->string_buffer_semantic[0] = '\0';

                // Vložení operátorů negace za operand (pokud existují nebo následujícím tokenem není .?)
                while (!stack_isEmpty(&stack_postfix) && stack_peek(&stack_postfix)->type == tok_t_not && tools->current_token->type != tok_t_orelse_un) {
                    (*postfix)[(*postfix_index)++] = stack_pop(&stack_postfix);
                }
                break;
        
            case tok_t_as: // @as
                precedence_analysis(tools, &precedence, tok_t_sym); OK;
                next_token(tools);
                expect_types(tools, 1, tok_t_lpa); OK;

                next_token(tools);
                expect_types(tools, 1, tok_t_i32); OK;

                next_token(tools);
                expect_types(tools, 1, tok_t_com); OK;

                next_token(tools);
                expect_types(tools, 1, tok_t_sym); OK;

                // Vyhledání symbolu v symtable a jeho sémantická kontrola
                tools->current_symtable = DLL_GetLast(&tools->sym_list);
                while(tools->sym_list.current != tools->sym_list.first) {
                    tools->result_data = symtable_get_item(tools->current_symtable, tools->current_token->attribute, &tools->error); OK;
                    if(tools->result_data == NULL){
                        DLL_Prev(&tools->sym_list);
                        tools->current_symtable = DLL_GetCurrent(&tools->sym_list);
                    } else {
                        break;
                    }
                }
                DLL_Last(&tools->sym_list);
                tools->current_symtable = DLL_GetLast(&tools->sym_list);

                if(tools->sym_list.current == tools->sym_list.first || tools->result_data == NULL){ 
                    fprintf(stderr, "Semantic error: undefined id\n");
                    tools->error = err_undef;
                    return;
                }

                if(tools->result_data->type != DATA_TYPE_INT && tools->result_data->canNull){
                    fprintf(stderr, "Semantic error: id can't be null\n");
                    tools->error = err_param;
                    return;
                }

                if(tools->cycle != -1){
                    if(!tools->left_data->as_func){
                        fprintf(stderr, "Semantic error: isn't value |id|\n");
                        tools->error = err_param;
                        return;
                    }
                }
                else{
                    fprintf(stderr, "Semantic error: Function @as isn't in tools->cycle\n");
                    tools->error = err_syntax;
                    return;
                }

                (*postfix)[(*postfix_index)++] = tools->current_token;

                next_token(tools);
                expect_types(tools, 1, tok_t_rpa); OK;

                next_token(tools);

                // Vložení operátorů negace za operand (pokud existují nebo následujícím tokenem není .?)
                while (!stack_isEmpty(&stack_postfix) && stack_peek(&stack_postfix)->type == tok_t_not && tools->current_token->type != tok_t_orelse_un) {
                    (*postfix)[(*postfix_index)++] = stack_pop(&stack_postfix);
                }
                break;

            default: // Operátory
                expect_types(tools, 16, tok_t_plus, tok_t_minus, tok_t_times, tok_t_divide, tok_t_not, tok_t_eq, tok_t_neq, tok_t_lt, tok_t_gt, tok_t_leq, tok_t_geq, tok_t_and, tok_t_or, tok_t_not, tok_t_orelse, tok_t_orelse_un); OK;
                precedence_analysis(tools, &precedence, tools->current_token->type); OK;
                if (tools->current_token->type == tok_t_not) { // Negace
                    //Vložení na zásobník pro pozdější zapsání do postfixové notace
                    stack_push(&stack_postfix, tools->current_token);
                    next_token(tools);
                } else if (tools->current_token->type == tok_t_orelse_un) { // .?
                    (*postfix)[(*postfix_index)++] = tools->current_token;
                    next_token(tools);

                    // Vložení operátorů negace za operátor .? (pokud existují nebo následujícím tokenem není .?)
                    while (!stack_isEmpty(&stack_postfix) && stack_peek(&stack_postfix)->type == tok_t_not && tools->current_token->type != tok_t_orelse_un) {
                        (*postfix)[(*postfix_index)++] = stack_pop(&stack_postfix);
                    }
                } else { // Operátory
                    // Vložení operátorů na zásobník, či ze zásobníku do postfixové notace na základě priority
                    while (!stack_isEmpty(&stack_postfix) && get_precedence_index(stack_peek(&stack_postfix)->type) <= get_precedence_index(tools->current_token->type)) {
                        (*postfix)[(*postfix_index)++] = stack_pop(&stack_postfix);
                    }
                    stack_push(&stack_postfix, tools->current_token);
                    next_token(tools);
                }
                break;
        }
    }
    precedence_analysis(tools, &precedence, tok_t_eof); OK; 

    // Vložení zbylých operátorů do postfixové notace
    while (!stack_isEmpty(&stack_postfix)) {
        (*postfix)[(*postfix_index)++] = stack_pop(&stack_postfix);
    }
}

/**
 * @brief Zpraocvání výrazu
 * 
 * Hlavní funkce řídící zpracování výrazu, sémantickou kontrolu
 * a jeho následné vygenerování do cílového kódu.
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void expression(parser_tools_t* tools) {
    printi(format[_comment], "<expression>");

    context_t save_context = tools->current_context;

    // Syntaktické zpracování výrazu
    Token_ptr postfix[MAX];
    int postfix_index = 0;
    dynamic_array_t functions_retval;
    dynamic_array_init(&functions_retval);
    expression_processing(tools, &postfix, &postfix_index, &functions_retval); OK;

    // Generování výrazu v cílovém kódu
    printi_postfix(&tools->string_tmp, postfix, postfix_index, &functions_retval, &tools->sym_list, tools->current_symtable, &tools->error); OK;

    // Sémantická kontrola výrazu
    tools->result_data = postfix_semantic(postfix, postfix_index, tools->sym_list, tools->current_symtable, &tools->error); OK;
    tools->current_context = save_context;
    switch (tools->current_context)
    {
    case CONTEXT_CONDITION:
        if(tools->result_data->type == DATA_TYPE_STRING || tools->result_data->type == DATA_TYPE_VOID || tools->result_data->type == DATA_TYPE_UND ){
            fprintf(stderr, "Semantic error: Invalid if/while condition type\n");
            tools->error = err_dt_invalid;
            return;
        }
    break;

    case CONTEXT_RETURN:// todo hlídat null
        if((tools->function_data->type != tools->result_data->type || tools->function_data->canNull != tools->result_data->canNull)){
            fprintf(stderr, "Semantic error: Invalid return type\n");
            tools->error = err_param;
            return;
        }    
    break;

    case CONTEXT_SYMBOL:
        if(tools->result_data->type == DATA_TYPE_VOID){
            fprintf(stderr, "Semantic error: Cant assighn void value\n");
            tools->error = err_dt_invalid;
            return;
        }

        if(tools->left_data->type == DATA_TYPE_UND){
            if(tools->result_data->type == DATA_TYPE_UND){
                tools->error = err_dt_unknown;
                fprintf(stderr, "ERROR: nelze do nedefinovaného typu dát null\n");
                return;
            }
            tools->left_data->type = tools->result_data->type;
            tools->left_data->canNull = tools->result_data->canNull;
        }
        else{
            if(!tools->left_data->canNull){
                if(tools->left_data->type != tools->result_data->type){
                    tools->error = err_dt_invalid;
                    fprintf(stderr, "ERROR: neshodné datové typy\n");
                    return;
                }
            }
            
            if(tools->left_data->type != tools->result_data->type && tools->result_data->type != DATA_TYPE_UND){
                tools->error = err_dt_invalid;
                fprintf(stderr, "ERROR: neshodné datové typy\n");
                return;
            }
        }
        break;

        case CONTEXT_CONDITION_FOR:
            if(tools->result_data->type != DATA_TYPE_U8 && tools->result_data->canNull != false){
                fprintf(stderr, "Semantic error: Invalid type in for condition\n");
                tools->error = err_dt_invalid;
                return;
            }
        break;
    }

    printi(format[_comment], "</expression>");
}

/**
 * @brief Kontrola redefinece proměnné
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void check_redefinition(parser_tools_t* tools) {
    DLL_Last(&tools->sym_list);
    tools->current_symtable = DLL_GetCurrent(&tools->sym_list);
    while(tools->sym_list.current != tools->sym_list.first) {
        tools->right_data = symtable_get_item(tools->current_symtable, tools->current_token->attribute, &tools->error); OK;
        if(tools->right_data == NULL){
            DLL_Prev(&tools->sym_list);
            tools->current_symtable = DLL_GetCurrent(&tools->sym_list);
        } else {
            break;
        }
    }
    DLL_Last(&tools->sym_list);
    tools->current_symtable = DLL_GetLast(&tools->sym_list);

    if(tools->right_data != NULL){
        fprintf(stderr, "Semantic error: Redefinition of id_without_null\n");
        tools->error = err_redef;
        return;
    }
}

/**
 * @brief Načítání tokenů prvního průchodu parseru
 * 
 * Funkce načítá tokeny ze scanneru.
 * Uvolňuje paměť po předchozím tokenu při počáteční načtení.
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void next_token_initial(parser_tools_t* tools) {
    do { //Dokud je tokenem komentář
        if (tools->current_token != NULL) {
            tok_free(tools->current_token);
        }   
        tools->current_token = scn_scan(scanner);
        if (tools->current_token->type == tok_t_error) {
            tok_free(tools->current_token);
            tools->current_token = NULL;
            tools->error = err_lexic;
        }
    } while (tools->current_token->type == tok_t_doc);
}

/**
 * @brief Načítání tokenů parseru při druhém průchodu
 * 
 * Funkce pouze načítá tokeny ze scanneru.
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void next_token(parser_tools_t* tools) {
    do { // Dokud je tokenem komentář
        tools->current_token = scn_scan(scanner);
    } while (tools->current_token->type == tok_t_doc);
}

/**
 * @brief Kontrolu typu tokenu
 * 
 * Funkce kontroluje, zda je aktuální token jednoho z očekávaných typů.
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 * @param count Počet očekávaných typů
 * @param ... Očekávané typy
 */
void expect_types(parser_tools_t* tools, int count, ...) {
    va_list args;
    va_start(args, count);
    bool found = false;
    token_type expected_types[count];
    // První průchod - kontrola shody a uložení typů
    for (int i = 0; i < count; i++) {
        token_type type = va_arg(args, token_type);
        expected_types[i] = type;
        
        if (tools->current_token->type == type) {
            found = true;
            break;
        }
    }
    
    va_end(args);
    
    // Pokud nebyla nalezena shoda, vytvoříme chybovou hlášku
    if (!found) {
        fprintf(stderr, "Syntax error: Expected: ");
        // Výpis všech očekávaných typů
        for (int i = 0; i < count; i++) {
            fprintf(stderr, "%s", tok_type_to_str(expected_types[i]));
            if (i < count - 1) {
                fprintf(stderr, ", ");
            }
        }
        fprintf(stderr, ", but got %s\n", tok_type_to_str(tools->current_token->type));
        tools->error = err_syntax;
    }
}

/**
 * @brief Počáteční funkce rekurzivního sestupu
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void program(parser_tools_t* tools) {
    printi(format[_comment], "<program>");

    next_token(tools);
    prolog(tools); OK;

    // Generace prvního rámce
    printi("%s", format[_createframe]);
    printi(format[_call], "$main");
    printi(format[_jump], "&$main");

    next_token(tools);
    function(tools); OK;

    expect_types(tools, 1, tok_t_eof); OK;

    stack_pop(&tools->stack_codegen);

    printi_builtin(&tools->string);

    printi(format[_label], "&$main");

    printi(format[_comment], "</program>");

    str_unify(&tools->string, &tools->string_tmp);
    str_clear(&tools->string_tmp);
}

/**
 * @brief Zpracování prologu programu
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void prolog(parser_tools_t* tools) {
    str_append(&tools->string, format[_comment], "<prolog>");

    expect_types(tools, 1, tok_t_const); OK;  

    next_token(tools);
    expect_types(tools, 1, tok_t_sym); OK;
    if(strcmp(tools->current_token->attribute, "ifj") != 0){
        tools->error = err_syntax;
        return;
    }

    stack_push(&tools->stack_codegen, tools->current_token);

    next_token(tools);
    expect_types(tools, 1, tok_t_ass); OK;

    next_token(tools);
    expect_types(tools, 1, tok_t_import); OK; 

    next_token(tools);
    expect_types(tools, 1, tok_t_lpa); OK;

    next_token(tools);
    expect_types(tools, 1, tok_t_str); OK;

    if(strcmp(tools->current_token->attribute, "ifj24.zig") != 0){
        tools->error = err_syntax;
        return;
    }

    next_token(tools);
    expect_types(tools, 1, tok_t_rpa); OK;

    next_token(tools);
    expect_types(tools, 1, tok_t_semicolon); OK;

    symtable_insert_builtin(tools->current_symtable, &tools->error); OK;

    // Generace hlavičky cílového kódu
    printi(".ifjcode24\n");

    printi(format[_comment], "</prolog>");
}

/**
 * @brief Zpracování funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void function(parser_tools_t* tools) {
    printi(format[_comment], "<function>");
    
    tools->current_symtable = DLL_Insert_last(&tools->sym_list, &tools->error); OK;

    expect_types(tools, 1, tok_t_pub); OK;

    next_token(tools);
    expect_types(tools, 1, tok_t_fn); OK;

    next_token(tools);
    expect_types(tools, 1, tok_t_sym); OK;
    stack_push(&tools->stack_codegen, tools->current_token);

    tools->current_symtable = DLL_GetFirst(&tools->sym_list);
    tools->function_data = symtable_get_item(tools->current_symtable, tools->current_token->attribute, &tools->error); OK;
    tools->current_symtable = DLL_GetLast(&tools->sym_list);

    sprintf(tools->string_buffer, "$%s", tools->current_token->attribute);
    printi(format[_label], tools->string_buffer);
    printi("%s", format[_pushframe]);

    str_unify(&tools->string, &tools->string_tmp);
    str_clear(&tools->string_tmp);

    next_token(tools);
    expect_types(tools, 1, tok_t_lpa); OK;  

    next_token(tools);
    parameter(tools); OK;
    
    expect_types(tools, 1, tok_t_rpa); OK;
    
    next_token(tools);
    return_type(tools); OK;

    next_token(tools);
    expect_types(tools, 1, tok_t_lcbr); OK;

    next_token(tools);

    // Generace speciální proměnné _
    printi(format[_defvar], "LF@_");

    // Zpracování těla funkce
    dynamic_array_insert(&tools->depth_sequence, ++tools->depth);
    body(tools); OK;
    tools->depth--;

    expect_types(tools, 1, tok_t_rcbr); OK;

    DLL_Delete_last(&tools->sym_list, &tools->error); OK;

    DLL_First(&tools->sym_list);
    tools->current_symtable = DLL_GetCurrent(&tools->sym_list);
    data_t* data = symtable_get_item(tools->current_symtable, stack_pop(&tools->stack_codegen)->attribute, &tools->error); OK;

    if(data == NULL) {
        fprintf(stderr, "Semantic error: Undefined function\n");
        tools->error = err_undef;
        return;
    }
    
    // Kontrola, zdali je ve funkci s návratovou hodnotou navracena hodnota
    if (data->type != DATA_TYPE_VOID && tools->depth_sequence.data[0] != -1) {
        fprintf(stderr, "Semantic error: Function must return a value\n");
        tools->error = err_ret_val; OK;
    }
    dynamic_array_clear(&tools->depth_sequence);
    DLL_Last(&tools->sym_list);
    tools->current_symtable = DLL_GetLast(&tools->sym_list);

    // Generace návratu z funkce
    printi("%s", format[_popframe]);
    printi("%s", format[_return]);

    // Spojení hlavního a dočasného řetězce cílového kódu a vyčištění pomocných řetězců
    str_unify(&tools->string, &tools->string_tmp);
    str_clear(&tools->string_tmp);
    str_clear(&tools->string_defvar);

    next_token(tools);
    function_next(tools); OK;

    printi(format[_comment], "</function>");
}

/**
 * @brief Zpracování dalších případných funkcí
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void function_next(parser_tools_t* tools) {
    printi(format[_comment], "<function_next>");

    if (tools->current_token->type != tok_t_eof) {
        function(tools); OK;
    } else {
        expect_types(tools, 1, tok_t_eof); OK;
    }
    printi(format[_comment], "</function_next>");
}

/**
 * @brief Zpracování parametrů funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void parameter(parser_tools_t* tools) {
    printi(format[_comment], "<parameter>");

    if (tools->current_token->type == tok_t_sym) {
        // Generace proměnné pro parametr
        sprintf(tools->string_buffer, "LF@%s", tools->current_token->attribute);
        printi(format[_defvar], tools->string_buffer);
        printi(format[_pops], tools->string_buffer);

        check_redefinition(tools); OK;

        tools->left_data = symtable_insert(tools->current_symtable, tools->current_token->attribute, &tools->error); OK;
        tools->left_data->isConst = true;
        tools->left_data->modified = true;

        next_token(tools);
        expect_types(tools, 1, tok_t_colon); OK;

        next_token(tools);
        tools->varOrFunc = true;
        type(tools); OK;

        next_token(tools);
        parameter_next(tools); OK;
    }
    printi(format[_comment], "</parameter>");
}

/**
 * @brief Zpracování dalších případných parametrů funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void parameter_next(parser_tools_t* tools) {
    printi(format[_comment], "<parameter_next>");

    if (tools->current_token->type == tok_t_com) {
        next_token(tools);
        parameter(tools); OK;
    }

    printi(format[_comment], "</parameter_next>");
}

/**
 * @brief Zpracování těla funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru 
 */
void body(parser_tools_t* tools) {
    printi(format[_comment], "<body>");

    if (tools->current_token->type == tok_t_sym || tools->current_token->type == tok_t_unused || tools->current_token->type == tok_t_const || tools->current_token->type == tok_t_var || tools->current_token->type == tok_t_if ||
        tools->current_token->type == tok_t_while || tools->current_token->type == tok_t_for || tools->current_token->type == tok_t_return || tools->current_token->type == tok_t_break || tools->current_token->type == tok_t_continue) {
        statement(tools); OK;
        body(tools); OK;
    }

    printi(format[_comment], "</body>");
}

/**
 * @brief Zpracování jednotlivých příkazů
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void statement(parser_tools_t* tools) {
    printi(format[_comment], "<statement>");

    bool constFlag = false;
    tools->left_data = NULL;
    tools->params = false;
    DLL_Last(&tools->sym_list);
    tools->current_symtable = DLL_GetCurrent(&tools->sym_list);
    char destination[MAX_STRING_LEN];

    switch (tools->current_token->type) {
    case tok_t_unused: //_
        tools->current_context = CONTEXT_NONE;
        printi(format[_comment], "<id_option>");
        printi(format[_comment], "</id_option>");

        next_token(tools);
        expect_types(tools, 1, tok_t_ass); OK;

        // Získání hodnoty pro přiřazení
        next_token(tools);
        value(tools); OK;

        expect_types(tools, 1, tok_t_semicolon); OK;

        // Přiřazení hodnoty do speciální proměnné unused
        sprintf(tools->string_buffer, "LF@_");
        printi(format[_move], tools->string_buffer, tools->string_buffer_value);

        next_token(tools);
        break;

    case tok_t_sym: //ID
        tools->current_context = CONTEXT_SYMBOL;
        printi(format[_comment], "<id_option>");
        printi(format[_comment], "</id_option>");

        //Uchování potřebných informací
        strcpy(tools->string_buffer_semantic, tools->current_token->attribute);
        stack_push(&tools->stack_codegen, tools->current_token);

        // Provedení akce v závislosti na tom, zda je symbol funkce či proměnná
        next_token(tools);
        id_statement(tools); OK;

        expect_types(tools, 1, tok_t_semicolon); OK;

        next_token(tools);
        break;

    case tok_t_const: //const
        constFlag = true;
    case tok_t_var: // var
        tools->current_context = CONTEXT_SYMBOL;
        printi(format[_comment], "<prefix>");
        printi(format[_comment], "</prefix>");

        next_token(tools);
        expect_types(tools, 1, tok_t_sym); OK;

        check_redefinition(tools); OK;

        tools->left_data = symtable_insert(tools->current_symtable, tools->current_token->attribute, &tools->error); OK;
        
        tools->left_data->isConst = constFlag;
        tools->left_data->modified = constFlag;

        // Generace proměnné / konstanty
        sprintf(tools->string_buffer, "LF@%s", tools->current_token->attribute);
        printi(format[_defvar], tools->string_buffer);
        strcpy(destination, tools->string_buffer);
        
        next_token(tools);
        definition(tools); OK;
        
        expect_types(tools, 1, tok_t_ass); OK;

        next_token(tools);
        value(tools); OK;
        
        // Generace přiřazení hodnoty k dané proměnné
        printi(format[_move], destination, tools->string_buffer_value);

        expect_types(tools, 1, tok_t_semicolon); OK;

        next_token(tools);
        break;

    case tok_t_if: // if
        tools->current_context = CONTEXT_CONDITION;
        next_token(tools);
        expect_types(tools, 1, tok_t_lpa); OK; 

        next_token(tools);
        // Generace proměnné výsledku podmínky
        int if_number = tools->counter_codegen_if++;
        sprintf(tools->string_buffer, "LF@%%if%i", if_number);
        printi(format[_defvar], tools->string_buffer);
        value(tools); OK;
        sprintf(tools->string_buffer, "LF@%%if%i", if_number);
        printi(format[_move], tools->string_buffer, tools->string_buffer_value);

        expect_types(tools, 1, tok_t_rpa); OK;
        
        // Generace pro vyhodnocení podmíněného skoku
        printi_condition_jump(&tools->string_tmp, "if", if_number);

        tools->current_symtable = DLL_Insert_last(&tools->sym_list, &tools->error); OK;
        
        // Případné provedení výrazu podmíněného neprázdnou hodnotou
        next_token(tools);
        sprintf(tools->string_buffer, "LF@%%if%i", if_number);
        not_null_value(tools); OK;

        // Tělo pozitivního výsledku podmínky
        then(tools); OK;

        // Generace skoku za celý if
        sprintf(tools->string_buffer, "*$if%i", if_number);
        printi(format[_jump], tools->string_buffer);

        // Generace návěští pro else
        sprintf(tools->string_buffer, "!$if%i", if_number);
        printi(format[_label], tools->string_buffer);

        DLL_Delete_last(&tools->sym_list, &tools->error); OK;

        // Tšlo negativního výsledku podmínky
        tools->current_symtable = DLL_Insert_last(&tools->sym_list, &tools->error); OK;
        else_then(tools); OK;
        DLL_Delete_last(&tools->sym_list, &tools->error); OK;

        // Generace návěští pro konec if
        sprintf(tools->string_buffer, "*$if%i", if_number);
        printi(format[_label], tools->string_buffer);
        break;

    case tok_t_while: // while
        tools->current_context = CONTEXT_CONDITION;
        int cycle_while = tools->cycle;
        tools->counter_codegen_while += 2;
        int while_number = tools->counter_codegen_while;
        tools->cycle = tools->counter_codegen_while;

        next_token(tools);
        expect_types(tools, 1, tok_t_lpa); OK;

        // Generace proměnné výsledku podmínky
        sprintf(tools->string_buffer, "LF@%%while%i", while_number);
        printi(format[_defvar], tools->string_buffer);

        // Generace návěští pro počátek while
        sprintf(tools->string_buffer, "*$while%i", while_number);
        printi(format[_label], tools->string_buffer);

        // Generace vyhodnocení podmínky
        next_token(tools);
        value(tools); OK;
        sprintf(tools->string_buffer, "LF@%%while%i", while_number);
        printi(format[_move], tools->string_buffer, tools->string_buffer_value);

        expect_types(tools, 1, tok_t_rpa); OK;

        // Generace pro vyhodnocení podmíněného skoku
        printi_condition_jump(&tools->string_tmp, "while", while_number);

        tools->current_symtable = DLL_Insert_last(&tools->sym_list, &tools->error); OK;

        // Případné provedení výrazu podmíněného neprázdnou hodnotou
        next_token(tools);
        sprintf(tools->string_buffer, "LF@%%while%i", while_number);
        not_null_value(tools); OK;

        // Tělo pozitivního výsledku podmínky
        then(tools); OK;
        DLL_Delete_last(&tools->sym_list, &tools->error); OK;

        // Generace skoku na začátek while
        sprintf(tools->string_buffer, "*$while%i", while_number);
        printi(format[_jump], tools->string_buffer);

        // Generace návěští pro konec while
        sprintf(tools->string_buffer, "!$while%i", while_number);
        printi(format[_label], tools->string_buffer);
        tools->cycle = cycle_while;

        tools->current_symtable = DLL_Insert_last(&tools->sym_list, &tools->error); OK;

        // Tělo negativního výsledku podmínky
        else_then(tools); OK;
        DLL_Delete_last(&tools->sym_list, &tools->error); OK;

        // Generace návěští pro konec while
        sprintf(tools->string_buffer, "&$while%i", while_number);
        printi(format[_label], tools->string_buffer);
        break;

    case tok_t_for: //for
        tools->current_context = CONTEXT_CONDITION_FOR;
        int cycle_for = tools->cycle;
        tools->counter_codegen_for += 2;
        int for_number = tools->counter_codegen_for;
        tools->cycle = tools->counter_codegen_for;

        next_token(tools);
        expect_types(tools, 1, tok_t_lpa); OK;

        next_token(tools);
        expect_types(tools, 3, tok_t_sym, tok_t_str, tok_t_mstr); OK;

        strcat(tools->string_buffer_semantic, tools->current_token->attribute);

        // Získání řetězce ve for, generace proměnné pro for a počitadla cyklů for
        for_value(tools); OK;
        sprintf(tools->string_buffer, "LF@%%forcounter%i", for_number);
        printi(format[_defvar], tools->string_buffer);
        printi(format[_move], tools->string_buffer, "int@0");
        sprintf(tools->string_buffer, "LF@%%for%i", for_number);
        printi(format[_defvar], tools->string_buffer);
        sprintf(tools->string_buffer, "LF@%%for%i", for_number);
        printi(format[_move], tools->string_buffer, tools->string_buffer_value);

        expect_types(tools, 1, tok_t_rpa); OK;

        tools->current_symtable = DLL_Insert_last(&tools->sym_list, &tools->error); OK;

        next_token(tools);
        expect_types(tools, 1, tok_t_alias); OK;

        next_token(tools);
        expect_types(tools, 2, tok_t_sym, tok_t_unused); OK;
        printi(format[_comment], "<id_option>");
        printi(format[_comment], "</id_option>");

        // Generace proměnné pro uložeí hodnoty výrazu ve for
        if (tools->current_token->type == tok_t_sym) { // ID
            sprintf(destination, "LF@%s", tools->current_token->attribute);
            printi(format[_defvar], destination);

            check_redefinition(tools); OK;

            tools->left_data = symtable_insert(tools->current_symtable, tools->current_token->attribute, &tools->error); OK;
            tools->left_data->type = DATA_TYPE_INT;
            tools->left_data->canNull = false;
            tools->left_data->modified = true;
        } else { // Specialní proměnná _
            sprintf(destination, "LF@_");
        }

        // Generace přiřazení hodnoty výrazu ve for
        sprintf(tools->string_buffer, "*$for%i", for_number);
        printi(format[_label], tools->string_buffer);

        // Generace získání ordinální hodnoty řetězce ve for na základě počitadla cyklů
        printi("%s", format[_createframe]);
        sprintf(tools->string_buffer, "LF@%%forcounter%i", for_number);
        printi(format[_pushs], tools->string_buffer);
        sprintf(tools->string_buffer, "LF@%%for%i", for_number);
        printi(format[_pushs], tools->string_buffer);
        sprintf(tools->string_buffer, "LF@%%forcounter%i", for_number);
        printi(format[_add], tools->string_buffer, tools->string_buffer, "int@1");
        printi(format[_call], "$$$ord");
        printi(format[_move], destination, "TF@%retval");
        printi("JUMPIFEQ !$for%i %s int@0\n", for_number, destination);

        next_token(tools);
        expect_types(tools, 1, tok_t_alias); OK;

        next_token(tools);

        then(tools); OK;

        // Skok na začátek cyklu for
        sprintf(tools->string_buffer, "*$for%i", for_number);
        printi(format[_jump], tools->string_buffer);

        // Návěští pro konec cyklu for
        sprintf(tools->string_buffer, "!$for%i", for_number);
        printi(format[_label], tools->string_buffer);

        tools->cycle = cycle_for;

        DLL_Delete_last(&tools->sym_list, &tools->error); OK;
        break;

    case tok_t_return: // return
        tools->current_context = CONTEXT_RETURN;
        next_token(tools);
        return_value(tools); OK;

        printi("%s", format[_popframe]);
        printi("%s", format[_return]);

        expect_types(tools, 1, tok_t_semicolon); OK;

        next_token(tools);
        break;

    case tok_t_break: // break
        if (tools->cycle == -1) {  // Kontrola, zda je break mimo cyklus
            fprintf(stderr, "Syntax error: Break statement outside of a loop\n");
            tools->error = err_syntax;
            return;
        } else if (tools->cycle % 2 == 0) { // Kontrola, zda je break ve while
            sprintf(tools->string_buffer, "&$while%i", tools->cycle);
            printi(format[_jump], tools->string_buffer);
        } else { // Kontrola, zda je break ve for
            sprintf(tools->string_buffer, "&$for%i", tools->cycle);
            printi(format[_jump], tools->string_buffer);
        }

        next_token(tools);
        expect_types(tools, 1, tok_t_semicolon); OK;

        next_token(tools);
        break;

    case tok_t_continue: // continue
        if (tools->cycle == -1) { // Kontrola, zda je continue mimo cyklus
            fprintf(stderr, "Syntax error: Continue statement outside of a loop\n");
            tools->error = err_syntax;
            return;
        } else if (tools->cycle % 2 == 0) { // Kontrola, zda je continue ve while
            sprintf(tools->string_buffer, "*$while%i", tools->cycle);
            printi(format[_jump], tools->string_buffer);
        } else { // Kontrola, zda je continue ve for
            sprintf(tools->string_buffer, "*$for%i", tools->cycle);
            printi(format[_jump], tools->string_buffer);
        }

        next_token(tools);
        expect_types(tools, 1, tok_t_semicolon); OK;

        next_token(tools);
        break;
    }

    printi(format[_comment], "</statement>");
}

/**
 * @brief Zpracování příkazu počínající symbolem
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void id_statement(parser_tools_t* tools) {
    printi(format[_comment], "<id_statement>");

    if (tools->current_token->type == tok_t_ass) { // Příkaz je přiřazení
        // Vyhledání symbolu mezi v symtable
        DLL_Last(&tools->sym_list);
        tools->current_symtable = DLL_GetCurrent(&tools->sym_list);
        while(tools->sym_list.current != tools->sym_list.first){
            tools->left_data = symtable_get_item(tools->current_symtable, stack_peek(&tools->stack_codegen)->attribute, &tools->error); OK;
            if (tools->left_data != NULL){   
                break;     
            }
            DLL_Prev(&tools->sym_list);
            tools->current_symtable = DLL_GetCurrent(&tools->sym_list);
        }
        DLL_Last(&tools->sym_list);
        tools->current_symtable = DLL_GetLast(&tools->sym_list);

        if(tools->sym_list.current == tools->sym_list.first || tools->left_data == NULL){
            fprintf(stderr, "Semantic error: %s is undefined variable\n", tools->current_token->attribute);
            tools->error = err_undef;
            return;
        }       
        if(tools->left_data->isConst){
            fprintf(stderr, "Semantic error: %s is constant\n", tools->current_token->attribute);
            tools->error = err_redef;
            return;
        }

        // Získání hodnoty pro přiřazení
        tools->left_data->modified = true;
        next_token(tools);
        tools->string_buffer_semantic[0] = '\0';
        value(tools); OK;

        // Generace přiřazení hodnoty k dané proměnné
        sprintf(tools->string_buffer, "LF@%s", stack_pop(&tools->stack_codegen)->attribute);
        printi(format[_move], tools->string_buffer, tools->string_buffer_value);
    } else { // Příkaz je voláním funkce
        tools->current_context = CONTEXT_NONE;
        tools->right_data = NULL;
        tools->is_left = true;
        call(tools); OK;

        next_token(tools);
    }

    printi(format[_comment], "</id_statement>");
}

/**
 * @brief Zpracování hodnoty
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void value(parser_tools_t* tools) {
    printi(format[_comment], "<value>");

    expect_types(tools, 9, tok_t_null, tok_t_int, tok_t_flt, tok_t_true, tok_t_false, tok_t_as, tok_t_sym, tok_t_lpa, tok_t_not); OK;

    // Zpracování výrazu
    expression(tools); OK;
    sprintf(tools->string_buffer_value, "LF@%%expression%i", tools->counter_global++);
    printi(format[_defvar], tools->string_buffer_value);
    printi(format[_pops], tools->string_buffer_value);

    printi(format[_comment], "</value>");
}

/**
 * @brief Přiřazení hodnoty výrazu podmíněného neprázdnou hodnotou
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void not_null_value(parser_tools_t* tools) {
    printi(format[_comment], "<not_null_value>");

    if (tools->current_token->type == tok_t_alias) { // Pokud se má provést přiřazení neprázdné hodnoty
        if(tools->result_data->type == DATA_TYPE_BOOLEAN){
            fprintf(stderr, "Semantic error: Cannot cast boolean to id_without_null\n");
            tools->error = err_dt_invalid;
            return;
        }

        // Generace proměnné pro uložení hodnoty výrazu
        next_token(tools);
        expect_types(tools, 2, tok_t_sym, tok_t_unused); OK; // ID
        char source[MAX_STRING_LEN];
        strcpy(source, tools->string_buffer);
        if (tools->current_token->type == tok_t_sym) { // ID
            sprintf(tools->string_buffer, "LF@%s", tools->current_token->attribute);
            printi(format[_defvar], tools->string_buffer);

            check_redefinition(tools); OK;

            tools->left_data = symtable_insert(tools->current_symtable, tools->current_token->attribute, &tools->error); OK;
            tools->left_data->type = tools->result_data->type;
            tools->left_data->canNull = !tools->result_data->canNull;
            tools->left_data->modified = true;
        } else { // Specialní proměnná _
            sprintf(tools->string_buffer, "LF@_");
        }
        printi(format[_move], tools->string_buffer, source);

        next_token(tools);
        expect_types(tools, 1, tok_t_alias); OK; // |

        next_token(tools);
    }
    else{ // Pokud se má provést pouze vyhodnocení výrazu
        if(tools->result_data->type != DATA_TYPE_BOOLEAN){
            if(!tools->result_data->canNull){
                fprintf(stderr, "Semantic error: Missing id_without_null\n");
                tools->error = err_dt_invalid;
            }
            return;
        }
    }

    printi(format[_comment], "</not_null_value>");
}

/**
 * @brief Zpracování těla cyklů a podmínek
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void then(parser_tools_t* tools) {
    printi(format[_comment], "<then>");

    // Zvýšení hloubky
    dynamic_array_insert(&tools->depth_sequence, ++tools->depth);
    if (tools->current_token->type == tok_t_lcbr) { // Tělo je blok
        next_token(tools);
        body(tools); OK;

        expect_types(tools, 1, tok_t_rcbr); OK;

        next_token(tools);
    } else { // Tělo je jednoduchý příkaz
        expect_types(tools, 10, tok_t_sym, tok_t_unused, tok_t_const, tok_t_var, tok_t_if, tok_t_while, tok_t_for, tok_t_return, tok_t_break, tok_t_continue); OK;
        statement(tools); OK;
    }
    --tools->depth;

    printi(format[_comment], "</then>");
}

/**
 * @brief Zpracování těla else
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void else_then(parser_tools_t* tools) {
    printi(format[_comment], "<else_then>");

    if (tools->current_token->type == tok_t_else) { // Poku je else
        next_token(tools);
        then(tools); OK;

        bool else_returned = false;
        for (int index = tools->depth_sequence.size - 1; index >= 0; index--) { // Pro každou již procházenou hloubku
            if (tools->depth_sequence.data[index] <= tools->depth + 1 && tools->depth_sequence.data[index] > 0 || tools->depth_sequence.data[index] > -tools->depth - 1 && tools->depth_sequence.data[index] < 0) {
                break;
            } else if (else_returned && tools->depth_sequence.data[index] == -1 - tools->depth) { // pokud else větev obsahuje return a kladná taktéž
                // Označení všech nižších či shodných hloubek do první vyšší, které byly již procházeny, za navrácené
                for (int index2 = tools->depth_sequence.size - 1; index2 >= 0; index2--) { 
                    if (tools->depth_sequence.data[index2] > 0) {
                        tools->depth_sequence.data[index2] *= -1; 
                    }
                    if (tools->depth_sequence.data[index2] <= tools->depth && tools->depth_sequence.data[index2] > 0 || tools->depth_sequence.data[index2] >= -tools->depth && tools->depth_sequence.data[index2] < 0) {
                        break;
                    }
                }
                break;
            } else if (tools->depth_sequence.data[index] == -1 - tools->depth) { // else větev obsahuje return
                else_returned = true;
            }
        }
    }
    printi(format[_comment], "</else_then>");
}

/**
 * @brief Zpracování symbolu
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void id_continue(parser_tools_t* tools) {
    printi(format[_comment], "<id_continue>");

    if (tools->current_token->type == tok_t_dot || tools->current_token->type == tok_t_lpa) { // Symbol je funkce
        char source[MAX_STRING_LEN]; 
        strcpy(source, tools->string_buffer_value);

        tools->is_left = false;
        call(tools); OK;
        strcpy(tools->string_buffer_value, source);

        sprintf(tools->string_buffer_value, "TF@%%retval");

        next_token(tools);
    } else { // Symbol je proměnná
        data_t *data;
        DLL_Last(&tools->sym_list);
        tools->current_symtable = DLL_GetCurrent(&tools->sym_list);
        while(tools->sym_list.current != tools->sym_list.first) {
            data = symtable_get_item(tools->current_symtable, stack_peek(&tools->stack_codegen)->attribute, &tools->error); OK;
            if(data == NULL){
                DLL_Prev(&tools->sym_list);
                tools->current_symtable = DLL_GetCurrent(&tools->sym_list);
            } else {
                break;
            }
        }
        DLL_Last(&tools->sym_list);
        if (tools->params) {
            tools->current_symtable = DLL_GetLast(&tools->sym_list);
        }

        if(tools->sym_list.current == tools->sym_list.first || data == NULL) {
            if (tools->params) {
                fprintf(stderr, "Semantic error: %s is undefined parameter\n", tools->current_token->attribute);
            } else {
                fprintf(stderr, "Semantic error: %s is undefined variable\n", tools->current_token->attribute);
            }
            tools->error = err_undef;
            return;
        }
        data->used = true;
        sprintf(tools->string_buffer_value, "LF@%s", stack_pop(&tools->stack_codegen)->attribute);
    }

    printi(format[_comment], "</id_continue>");
}

/**
 * @brief Zpracování návratové hodnoty funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void return_value(parser_tools_t* tools) {
    printi(format[_comment], "<return_value>");

    if (tools->current_token->type != tok_t_semicolon) {
        if(tools->function_data->type == DATA_TYPE_VOID){
            fprintf(stderr, "Semantic error: void function cannot return value\n");
            tools->error = err_ret_val;
            return;
        }

        value(tools); OK;
        sprintf(tools->string_buffer, "LF@%%retval");
        printi(format[_move], tools->string_buffer, tools->string_buffer_value);
    }
    else {
        if(tools->function_data->type != DATA_TYPE_VOID){
            fprintf(stderr, "Semantic error: not void function must return value\n");
            tools->error = err_ret_val;
            return;
        }
    }

    // Označení všech nižších, které byly již procházeny, za navrácené
    for (int index = tools->depth_sequence.size - 1; index >= 0; index--) {
        if (tools->depth_sequence.data[index] > 0) {
            tools->depth_sequence.data[index] *= -1;
        }
        if (tools->depth_sequence.data[index] <= tools->depth && tools->depth_sequence.data[index] > 0 || tools->depth_sequence.data[index] >= -tools->depth && tools->depth_sequence.data[index] < 0) {
            break;
        }
    }
    printi(format[_comment], "</return_value>");
}

/**
 * @brief Zpracování volání funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void call(parser_tools_t* tools) {
    printi(format[_comment], "<call>");
    int param_count_save = tools->param_count;
    tools->param_count = -1;
    tools->params = true;
    
    printi("%s", format[_createframe]);
    if (tools->current_token->type == tok_t_dot) { // Vestavěná funkce
        stack_pop(&tools->stack_codegen);

        // Získání kompletního názvu vestavěné funkce
        strcat(tools->string_buffer_semantic, ".");
        next_token(tools);
        expect_types(tools, 1, tok_t_sym); OK;
        strcat(tools->string_buffer_semantic, tools->current_token->attribute);
        
        // Získání informací o vestavěné funkci
        tools->current_symtable = DLL_GetFirst(&tools->sym_list);
        tools->right_data = symtable_get_item(tools->current_symtable, tools->string_buffer_semantic, &tools->error); OK;
        tools->current_symtable = DLL_GetLast(&tools->sym_list);
        if(tools->right_data == NULL){
            fprintf(stderr, "Semantic error: %s is undefined function\n", tools->current_token->attribute);
            tools->error = err_undef; 
            return;
        }
        tools->right_data->used = true;
        if (tools->is_left){
            if(tools->right_data->type != DATA_TYPE_VOID){
                fprintf(stderr, "Semantic error: cannot discard the return value");
                tools->error = err_param;
                return;
            }
        }
        
        stack_push(&tools->stack_codegen, tools->current_token);

        next_token(tools);
        expect_types(tools, 1, tok_t_lpa); OK;

        next_token(tools);

        // Zpracování parametrů volání vestavěné funkce
        call_params(tools); OK;

        expect_types(tools, 1, tok_t_rpa); OK;

        // Získání názvu návěští funkce
        sprintf(tools->string_buffer, "$$$%s", stack_pop(&tools->stack_codegen)->attribute);
    } else { // Funkce
        expect_types(tools, 1, tok_t_lpa); OK;

        // Získání informací o funkci
        tools->current_symtable = DLL_GetFirst(&tools->sym_list);
        tools->right_data = symtable_get_item(tools->current_symtable, tools->string_buffer_semantic, &tools->error); OK;
        if(tools->right_data == NULL){
            fprintf(stderr, "Semantic error: %s is undefined function\n", tools->current_token->attribute);
            tools->error = err_undef; 
            return;
        }
        tools->current_symtable = DLL_GetLast(&tools->sym_list);
        if (tools->is_left){
            if(tools->right_data->type != DATA_TYPE_VOID){
                fprintf(stderr, "Semantic error: cannot discard the return value");
                tools->error = err_param;
                return;
            }
        }
        tools->right_data->used = true;

        next_token(tools);
        call_params(tools); OK;

        expect_types(tools, 1, tok_t_rpa); OK;

        // Získání názvu návěští funkce
        sprintf(tools->string_buffer, "$%s", stack_pop(&tools->stack_codegen)->attribute);
    }
    tools->param_count = param_count_save;

    printi(format[_call], tools->string_buffer);
    printi(format[_comment], "</call>");
}

/**
 * @brief Zpracování parametrů volání funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void call_params(parser_tools_t* tools) {
    printi(format[_comment], "<call_params>");
    tools->current_context = CONTEXT_NONE;
    
    if (tools->current_token->type != tok_t_rpa) { // Pokud jsou nějaké parametry
        // Zpracování hodnoty parametru
        tools->param_count++;
        data_t* data = tools->right_data;
        int param_count_save = tools->param_count;
        char string_buffer_semantic_tmp[MAX_STRING_LEN];
        strcpy(string_buffer_semantic_tmp, tools->string_buffer_semantic);
        call_value(tools); OK;
        strcpy(tools->string_buffer_semantic, string_buffer_semantic_tmp);
        tools->right_data = data;

        // Kontrola parametru
        bool found = true;
        if (data->parameters->size <= param_count_save) {
            fprintf(stderr, "Semantic error: Function parameter count mismatch\n");
            tools->error = err_param;
            return;
        } else if(stack_peek(&tools->stack_codegen)->type == tok_t_str || stack_peek(&tools->stack_codegen)->type == tok_t_mstr){
            if (data->parameters->data[param_count_save] != tok_t_str) {
                found = false;
            }
        } else {
            switch (tools->result_data->type) {
                case DATA_TYPE_INT:
                    if (tools->result_data->canNull) {
                        if (data->parameters->data[param_count_save] != tok_t_i32_opt) {
                            tools->error = err_param;
                            found = false;
                        }
                    }
                    break;
                case DATA_TYPE_DOUBLE:
                    if (tools->result_data->canNull) {
                        if (data->parameters->data[param_count_save] != tok_t_f64_opt) {
                            tools->error = err_param;
                            found = false;
                        }
                    }
                    break;
                case DATA_TYPE_U8:
                    if (tools->result_data->canNull) {
                        if (data->parameters->data[param_count_save] != tok_t_u8_opt) {
                            tools->error = err_param;
                            found = false;
                        }
                    } else if (data->parameters->data[param_count_save] != tok_t_u8) {
                        tools->error = err_param;
                        found = false;
                    }
                    break;
                case DATA_TYPE_BOOLEAN:
                    if (data->parameters->data[param_count_save] != tok_t_bool) {
                        tools->error = err_param;
                        found = false;
                    }
                    break;
                default:
                    break;
            }
        }
        if (!found && data->parameters->data[param_count_save] != tok_t_unused) {
            fprintf(stderr, "Semantic error: Function parameter type mismatch\n");
            tools->error = err_param;
            return;
        }
        else{
            tools->error = err_none;
        }

        // Generace přiřazení hodnoty parametru
        char source[MAX_STRING_LEN];
        token_type tmp_type = stack_peek(&tools->stack_codegen)->type;
        strcpy(source, tools->string_buffer_value);
        bool string_value = false;
        if (stack_peek(&tools->stack_codegen)->type == tok_t_str || stack_peek(&tools->stack_codegen)->type == tok_t_mstr) {
            stack_pop(&tools->stack_codegen);
            string_value = true;
        } else if (strstr(source, "TF@%retval") != NULL) {
            sprintf(source, "GF@%%%i", tools->counter_global++);
            printi(format[_defvar], source);
            printi(format[_move], source, "TF@%retval");
            printi("%s" ,format[_createframe]);
        }

        call_params_next(tools); OK;

        if (tools->param_count != data->parameters->size -1 && tools->param_count != -2) {
            fprintf(stderr, "Semantic error: Function parameter count mismatch\n");
            tools->error = err_param;
            return;
        }
        tools->param_count = -2;

        // Přidání parametru do zásobník
        if (string_value) {
            printi("PUSHS string@");
            printi_string(&tools->string_tmp, source, tmp_type);
            printi("\n");
        } else {
            printi(format[_pushs], source);
            printi("%s", format[_createframe]);
        }
    }

    printi(format[_comment], "</call_params>");
}

/**
 * @brief Zpracování dalších parametrů volání funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void call_params_next(parser_tools_t* tools) {
    printi(format[_comment], "<call_params_next>");

    if (tools->current_token->type == tok_t_com) {
        next_token(tools);
        call_params(tools); OK;
    }
    printi(format[_comment], "</call_params_next>");
}

/**
 * @brief Zpracování hodnoty parametru
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void call_value(parser_tools_t* tools) {
    printi(format[_comment], "<call_value>");

    if (tools->current_token->type == tok_t_str || tools->current_token->type == tok_t_mstr) { // Hodnota je řetězec
        strcpy(tools->string_buffer_value, tools->current_token->attribute);
        stack_push(&tools->stack_codegen, tools->current_token);
        next_token(tools);
    } else {
        value(tools); OK;
    }

    printi(format[_comment], "</call_value>");
}

/**
 * @brief Zpracování návratového typu funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void return_type(parser_tools_t* tools) {
    printi(format[_comment], "<return_type>");

    if (tools->current_token->type != tok_t_void) {
        tools->varOrFunc = false;
        type(tools); OK;
        
        printi(format[_defvar], "LF@%retval");
    } else {
        expect_types(tools, 1, tok_t_void); OK;
    }

    printi(format[_comment], "</return_type>");
}

/**
 * @brief Zpracování typu symbolu
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void type(parser_tools_t* tools) {
    printi(format[_comment], "<type>");

    expect_types(tools, 7, tok_t_i32, tok_t_f64, tok_t_u8, tok_t_bool, tok_t_i32_opt, tok_t_f64_opt, tok_t_u8_opt); OK;
    if(tools->varOrFunc){
        switch (tools->current_token->type)    // přiřazení typu funkce
        {
            case tok_t_i32:
                tools->left_data->type = DATA_TYPE_INT;
                break;

            case tok_t_f64:
                tools->left_data->type = DATA_TYPE_DOUBLE;
                break;

            case tok_t_u8:
                tools->left_data->type = DATA_TYPE_U8;
                break;

            case tok_t_bool:
                tools->left_data->type = DATA_TYPE_BOOLEAN;
                break;
            
            case tok_t_i32_opt:
                tools->left_data->type = DATA_TYPE_INT;
                tools->left_data->canNull = true;
                break;

            case tok_t_f64_opt:
                tools->left_data->type = DATA_TYPE_DOUBLE;
                tools->left_data->canNull = true;
                break;

            case tok_t_u8_opt:
                tools->left_data->type = DATA_TYPE_U8;
                tools->left_data->canNull = true;
                break;
            
            default:    
                tools->error = err_internal;
                return;
                break;
        }
    }

    printi(format[_comment], "</type>");
}

/**
 * @brief Zpracování typu proměnné
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void definition(parser_tools_t* tools) {
    printi(format[_comment], "<definition>");

    if (tools->current_token->type == tok_t_colon) { // Příkaz obsahuje přímou definici proměnné
        next_token(tools);
        tools->varOrFunc = true;
        type(tools); OK;

        next_token(tools);
    }

    printi(format[_comment], "</definition>");
}

/**
 * @brief Zpracování hodnoty pro for cyklus
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void for_value(parser_tools_t* tools) {
    printi(format[_comment], "<for_value>");

    if (tools->current_token->type == tok_t_sym) { // Hodnota je symbol
        stack_push(&tools->stack_codegen, tools->current_token);
        
        next_token(tools);
        id_continue(tools); OK;
    } else { // Hodnota je zadána řetězcem
        // Generace proměnné pro uložení řetězce
        sprintf(tools->string_buffer_value, "LF@%%expression%i", tools->counter_global++);
        printi(format[_defvar], tools->string_buffer_value);
        printi("MOVE %s string@", tools->string_buffer_value);
        printi_string(&tools->string_tmp, tools->current_token->attribute, tools->current_token->type);
        printi("\n");

        next_token(tools);
    }

    printi(format[_comment], "</for_value>");
}

/**
 * @brief První průchod parseru zdrojovým kódem
 * 
 * Funkce prochází zdrojový kód a vytváří tabulku symbolů pro funkce.
 *  
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void parse_fn_first(parser_tools_t* tools) {
    // Get the first token.
    next_token_initial(tools); OK;
    // Iterate through program
    while (tools->current_token->type != tok_t_eof) {
        if (tools->current_token->type == tok_t_fn) {
            next_token_initial(tools); OK;
            expect_types(tools, 1, tok_t_sym); OK;

            check_redefinition(tools); OK;

            tools->left_data = symtable_insert(tools->current_symtable, tools->current_token->attribute, &tools->error); OK;
            tools->left_data->modified = true;
            
            next_token_initial(tools); OK;
            expect_types(tools, 1, tok_t_lpa); OK; // (

            next_token_initial(tools); OK;
            
            // Zpracování parametrů funkce
            bool not_first = false;
            while (tools->current_token->type != tok_t_rpa) { // )
                if (not_first) {
                    next_token_initial(tools); OK;
                } else {
                    not_first = true;
                }
                expect_types(tools, 1, tok_t_sym); OK;

                next_token_initial(tools); OK;
                expect_types(tools, 1, tok_t_colon); OK;

                next_token_initial(tools); OK;
                expect_types(tools, 7, tok_t_i32, tok_t_f64, tok_t_u8, tok_t_bool, tok_t_i32_opt, tok_t_f64_opt, tok_t_u8_opt); OK;

                symtable_insert_params(tools->left_data, tools->current_token->type, &tools->error); OK;

                next_token_initial(tools); OK;
                expect_types(tools, 2, tok_t_com, tok_t_rpa); OK;
            }
            
            // Zpracování návratového typu funkce
            next_token_initial(tools); OK;
            expect_types(tools, 8, tok_t_i32, tok_t_f64, tok_t_u8, tok_t_void, tok_t_bool, tok_t_i32_opt, tok_t_f64_opt, tok_t_u8_opt); OK;
            switch (tools->current_token->type)   // přiřazení typu funkce
                {
                case tok_t_i32:
                    tools->left_data->type = DATA_TYPE_INT;
                    break;

                case tok_t_f64:
                    tools->left_data->type = DATA_TYPE_DOUBLE;
                    break;

                case tok_t_u8:
                    tools->left_data->type = DATA_TYPE_U8;
                    break;

                case tok_t_bool:
                    tools->left_data->type = DATA_TYPE_BOOLEAN;
                    break;
                
                case tok_t_i32_opt:
                    tools->left_data->type = DATA_TYPE_INT;
                    tools->left_data->canNull = true;
                    break;

                case tok_t_f64_opt:
                    tools->left_data->type = DATA_TYPE_DOUBLE;
                    tools->left_data->canNull = true;
                    break;

                case tok_t_u8_opt:
                    tools->left_data->type = DATA_TYPE_U8;
                    tools->left_data->canNull = true;
                    break;

                case tok_t_void:
                    tools->left_data->type = DATA_TYPE_VOID;
                    break;
                
                default:
                    tools->error = err_internal;
                    return;
                    break;
                }
        }
        next_token_initial(tools); OK;
    }

    tok_free(tools->current_token);
}

/**
 * @brief Inicializace struktury parser_tools_t
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void parser_tools_init(parser_tools_t* tools) {
    tools->error = err_none;
    tools->current_token = NULL;

    tools->current_symtable = NULL;
    tools->current_context = CONTEXT_NONE;
    DLL_Init(&tools->sym_list);
    tools->left_data = NULL;
    tools->right_data = NULL;
    tools->result_data = NULL;
    tools->function_data = NULL;
    tools->depth = 0;
    dynamic_array_init(&tools->depth_sequence);
    tools->string_buffer_semantic[0] = '\0';
    tools->param_count = -1;
    tools->params = false;
    tools->is_left = false;
    tools->varOrFunc = false;

    tools->counter_codegen_if = 0;
    tools->counter_codegen_while = 0;
    tools->counter_codegen_for = 0;
    tools->counter_global = 0;
    tools->cycle = -1;
    tools->string_buffer[0] = '\0';
    tools->string_buffer_value[0] = '\0';
    stack_init(&tools->stack_codegen);
    str_init(&tools->string);
    str_init(&tools->string_tmp);
    str_init(&tools->string_defvar);
}

/**
 * @brief Uvolnění struktury parser_tools_t
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void parser_tools_destroy(parser_tools_t* tools) {
    tok_free(tools->current_token);
    DLL_Destroy(&tools->sym_list, &tools->error);
    dynamic_array_destroy(&tools->depth_sequence);
    str_destroy(&tools->string);
    str_destroy(&tools->string_tmp);
    str_destroy(&tools->string_defvar);
}

/**
 * @brief Hlavní funkce parseru
 * 
 * Parser má celkově dva režimy.
 * Při prvním průchodu přidá do tabulky symbolů všechny funkce a proměnné.
 * Při druhém průchodu se provádí samotná syntaktická analýza.
 * 
 * @return Návratový kód chyby během parsování
 */
err_codes parse() {
    parser_tools_t tools;
    parser_tools_init(&tools);

    tools.current_symtable = DLL_Insert_last(&tools.sym_list, &tools.error);
    if (tools.error != err_none) {
        return tools.error;
    }

    // První průchod parseru zdrovým kódem a naplnění tabulky symbolů funkcemi
    parse_fn_first(&tools);

    tools.left_data = symtable_get_item(tools.current_symtable, "main", &tools.error);
    if(tools.left_data == NULL) {
        fprintf(stderr, "Semantic error: main is undefined function\n");
        tools.error = err_undef;
        return tools.error; 
    }
    if(tools.left_data->type != DATA_TYPE_VOID || tools.left_data->parameters->size != 0) {
        fprintf(stderr, "Semantic error: main must be void and have no parameters\n");
        tools.error = err_param;
    }
    tools.left_data->used = true;
    
    // Druhý průchod parseru zdrovým kódem s úplnou syntaktická analýza
    if (tools.error == err_none) {
        program(&tools);

        // Výpis výsledného kódu při úspěšném průchodu
        if (tools.error == err_none) {
            str_printout(&tools.string);
        }
    }
    
    parser_tools_destroy(&tools);

    return tools.error;
}

/*** Konec souboru parser.c ***/