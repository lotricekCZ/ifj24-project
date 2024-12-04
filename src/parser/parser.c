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
#include <float.h>
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

/**
 * @brief Precedenční tabulka
*/ 
char precedence_table[12][12] = {
//     .?   !   * / orlse + -   r   and   or   (    )    ID   $
    { '=', ' ', '>', '>', '>', '>', '>', '>', ' ', '>', ' ', '>', }, // .?
    { ' ', '<', '>', '>', '>', '>', '>', '>', '<', '>', '<', '>', }, // !
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

/*

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

 */

// Funkce pro získání indexu tokenu v precedenční tabulce
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

void precedence_analysis_print (dynamic_array_t *precedence, token_type input, int precedence_top) {
    for (int i = 0; i < precedence->size; i++) {
        if (precedence->data[i] == (int)'>' || precedence->data[i] == (int)'<' || precedence->data[i] == (int)'E') {
            fprintf(stderr, "%c ", precedence->data[i]);
        } else
        fprintf(stderr, "%s ", tok_type_to_str(precedence->data[i]));
    }
    fprintf(stderr, "| %c | ", precedence_table[get_precedence_index(precedence_top)][get_precedence_index(input)]);
    fprintf(stderr, "%s\n", tok_type_to_str(input));
}

// Funkce pro zpracování precedenční analýzy
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
        precedence_analysis_print(precedence, input, precedence_top);
        // Získání akce z precedenční tabulky pro daný vstup a vrchol zásobníku
        action = precedence_table[get_precedence_index(precedence_top)][get_precedence_index(input)];
        switch (action) {
            case '<': // Shift
                dynamic_array_insert(precedence, input);
                for (int index = precedence->size - 2; index >= 0; index--) { // Pro prvky výrazu počínaje od konce
                    if (precedence->data[index] == precedence_top) { // Vložení shift na místo za vrcholem zásobníku
                        precedence->data[index + 1] = (int)'<';
                        break;
                    } else {
                        precedence->data[index + 1] = precedence->data[index];
                    }
                }
                break;

            case '>': // Redukce
                if (precedence->data[precedence->size - 1] == (int)'<') { // Kontrola validity výrazu
                    fprintf(stderr, "Syntax error: Wrong expression.\n");
                    tools->error = err_syntax;
                    return;
                }
                bool without_value = true; // Příznak pro kontrolu existence hodnoty mezi operátory
                for (int index = precedence->size - 1; index >= 0; index--) { // Pro prvky výrazu počínaje od konce
                    // Redukce výrazu
                    if (precedence->data[index] == (int)'<') {
                        precedence->data[index] = (int)'E';
                        break;
                    }
                    // Existence hodnoty mezi operátory
                    if (precedence->data[index] == tok_t_sym || precedence->data[index] == (int)'E') {
                        without_value = false;
                    } else if (precedence->data[index] == precedence_top) { // Redukce výrazu bez hodnoty
                        if (without_value && precedence_top != tok_t_rpa && precedence_top != tok_t_orelse_un) {
                            fprintf(stderr, "Syntax error: Error in expression.\n");
                            tools->error = err_syntax;
                            return;
                        }
                        if (precedence_top != tok_t_not) {
                            without_value = true;
                        }
                    }
                    precedence->size--; // Odstranění prvku z výrazu
                    // Neplatný výraz
                    if (index == 0) {
                        fprintf(stderr, "Syntax error: Error in expression.\n");
                        tools->error = err_syntax;
                        return;
                    }
                }
                if (without_value) {
                    fprintf(stderr, "Syntax error: Error in expression.\n");
                    tools->error = err_syntax;
                    return;
                }
                break;

            case ' ': //Neexistující pravidlo
                if (action == ' ' && (input != tok_t_eof || precedence_top != tok_t_eof)) {
                    fprintf(stderr, "Syntax error: Error in expression.\n");
                    tools->error = err_syntax;
                    return;
                } else if (precedence->size != 2) { // Zakončení výrazu musí být absolutně zredukováno
                    fprintf(stderr, "Syntax error: Error in expression.\n");
                    tools->error = err_syntax;
                    return;
                }
                break;
        }
    } while (action == (int)'>');

    // Vložení tokenu do výrazu
    dynamic_array_insert(precedence, input);
}

// Funkce syntaktického zpracování výrazu a vytváření postfixové notace
void expression_processing(parser_tools_t* tools, Token_ptr (*postfix)[MAX], int* postfix_index, dynamic_array_t* functions_retval) {
    Stack stack_postfix; // Pomocný zásobník pro postfixovou notaci
    stack_init(&stack_postfix);

    dynamic_array_t precedence; // Dynamické pole pro precedenční analýzu
    dynamic_array_init(&precedence);
    dynamic_array_insert(&precedence, tok_t_eof); // Inicializace precedenční analýzy

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
                data_t* data = symtable_get_item(tools->current_symtable, tools->string_buffer_semantic, &tools->error); OK; // Vyhledání aktuálního symbolu v symtable
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
    precedence_analysis(tools, &precedence, tok_t_eof); OK; // Ukončení precedenční analýzy

    // Vložení zbylých operátorů do postfixové notace
    while (!stack_isEmpty(&stack_postfix)) {
        (*postfix)[(*postfix_index)++] = stack_pop(&stack_postfix);
    }
}

// Funkce zpraocvání výrazu
void expression(parser_tools_t* tools) {
    // Uložení kontextu
    context_t save_context = tools->current_context;

    // Syntaktické zpracování výrazu
    Token_ptr postfix[MAX];
    int postfix_index = 0;
    dynamic_array_t functions_retval;
    dynamic_array_init(&functions_retval);
    expression_processing(tools, &postfix, &postfix_index, &functions_retval); OK;

    sprintf(tools->string_buffer_value, "LF@%%expression%i", tools->counter_global++);
    printi(format[_defvar], tools->string_buffer_value);

    bool single_flat_float = false; // Příznak pro jednoduché floatové číslo
    if (postfix_index == 1 && postfix[0]->type == tok_t_flt &&
        (tools->current_context == CONTEXT_SYMBOL || tools->current_context == CONTEXT_NONE || tools->current_context == CONTEXT_RETURN)) { // Pokud lze float hodnotu převést implicitně na int
        char *endptr;
        double cislo = strtod(postfix[0]->attribute, &endptr);
        single_flat_float = endptr != postfix[0]->attribute && *endptr == '\0' && cislo == (int)cislo; // Zjištění desetinné části float čísla
    }
    if (single_flat_float) { //Pokud je výraz jednoduché floatové číslo
        tools->result_data = imalloc(sizeof(data_t));
        if (tools->result_data == NULL) { 
            tools->error = err_internal;
            return;
        }
        tools->result_data->type = DATA_TYPE_RETYPABLE_DOUBLE; // Vytvoření výsledných dat jakožto přetypovatelný float
        tools->result_data->id = imalloc(strlen(postfix[0]->attribute) + 1);
        strcpy(tools->result_data->id, postfix[0]->attribute);
    } else {
        // Generování výrazu v cílovém kódu
        printi_postfix(&tools->string_tmp, postfix, postfix_index, &functions_retval, &tools->sym_list, tools->current_symtable, &tools->error); OK;
        printi(format[_pops], tools->string_buffer_value); // Přesun výsledku do unikátní proměnné

        // Sémantická kontrola výrazu
        tools->result_data = postfix_semantic(postfix, postfix_index, tools->sym_list, tools->current_symtable, &tools->error); OK;
        tools->current_context = save_context;
    }

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
        if (tools->result_data->type == DATA_TYPE_RETYPABLE_DOUBLE) { // Pokud je výsledek přetypovatelný float
            char source[MAX_STRING_LEN]; // Pomocný buffer pro výpis hodnoty
            if (tools->function_data->type == DATA_TYPE_DOUBLE || tools->function_data->type == DATA_TYPE_UND) { // Ponechání float hodnoty
                sprintf(source, "float@%a", atof(tools->result_data->id));
                printi(format[_move], tools->string_buffer_value, source);
            } else if (tools->left_data->type == DATA_TYPE_INT) { // Přetypování float na int
                sprintf(source, "int@%i", atoi(tools->result_data->id));
                printi(format[_move], tools->string_buffer_value, source);
            } else {
                fprintf(stderr, "Semantic error: missmatch data type\n");
            }
        } else if((tools->function_data->type != tools->result_data->type || tools->function_data->canNull != tools->result_data->canNull)){
            fprintf(stderr, "Semantic error: Invalid return type\n");
            tools->error = err_param;
            return;
        }
    break;

    case CONTEXT_SYMBOL:
        if(tools->result_data->type == DATA_TYPE_VOID){
            fprintf(stderr, "Semantic error: Cant assign void value\n");
            tools->error = err_dt_invalid;
            return;
        }

        if(tools->left_data->type == DATA_TYPE_UND){
            if(tools->result_data->type == DATA_TYPE_UND){
                tools->error = err_dt_unknown;
                fprintf(stderr, "Semantic error: Cant assign NULL value\n");
                return;
            }
            tools->left_data->type = tools->result_data->type;
            tools->left_data->canNull = tools->result_data->canNull;
        }
        else if (tools->result_data->type == DATA_TYPE_RETYPABLE_DOUBLE) { // Pokud je výsledek přetypovatelný float
            char source[MAX_STRING_LEN]; // Pomocný buffer pro výpis hodnoty
            if (tools->left_data->type == DATA_TYPE_DOUBLE || tools->left_data->type == DATA_TYPE_UND) { // Ponechání float hodnoty
                sprintf(source, "float@%a", atof(tools->result_data->id));
                printi(format[_move], tools->string_buffer_value, source);
            } else if (tools->left_data->type == DATA_TYPE_INT) { // Přetypování float na int
                sprintf(source, "int@%i", atoi(tools->result_data->id));
                printi(format[_move], tools->string_buffer_value, source);
            } else {
                fprintf(stderr, "Semantic error: missmatch data type\n");
            }
        }
        else{
            if(!tools->left_data->canNull){
                if(tools->left_data->type != tools->result_data->type){
                    tools->error = err_dt_invalid;
                    fprintf(stderr, "Semantic error: missmatch data type\n");
                    return;
                }
            }
            
            if(tools->left_data->type != tools->result_data->type && tools->result_data->type != DATA_TYPE_UND){
                tools->error = err_dt_invalid;
                fprintf(stderr, "Semantic error: missmatch data type\n");
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
}

// Funkce kontroly redefinece proměnné
void check_redefinition(parser_tools_t* tools) {
    // Vyhledání symbolu v symtable
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

    // Kontrola redefinice symbolu
    if(tools->right_data != NULL){
        fprintf(stderr, "Semantic error: Redefinition of variable\n");
        tools->error = err_redef;
        return;
    }
}

// Funkce načítání tokenů prvního průchodu parseru
void next_token_initial(parser_tools_t* tools) {
    do { //Dokud je tokenem komentář
        if (tools->current_token != NULL) {
            tok_free(tools->current_token);
        }   
        tools->current_token = scn_scan(tools->scanner);
        if (tools->current_token->type == tok_t_error) {
            tok_free(tools->current_token);
            tools->current_token = NULL;
            tools->error = err_lexic;
        }
    } while (tools->current_token->type == tok_t_doc);
}

// Funkce pro načítání tokenů parseru při druhém průchodu
void next_token(parser_tools_t* tools) {
    do { // Dokud je tokenem komentář
        tools->current_token = scn_scan(tools->scanner);
    } while (tools->current_token->type == tok_t_doc);
}

//Funkce pro kontrolu typu tokenu
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

// Počáteční funkce rekurzivního sestupu
void program(parser_tools_t* tools) {
    // Zpracování prologu programu
    next_token(tools);
    prolog(tools); OK;

    // Generace prvního rámce
    printi("%s", format[_createframe]);
    printi(format[_call], "$main");
    printi(format[_jump], "&$main");

    // Zpracování funkcí programu
    next_token(tools);
    function(tools); OK;

    expect_types(tools, 1, tok_t_eof); OK;

    // Vyčištění zásobníku
    stack_pop(&tools->stack_codegen);
    // Generace vestavěných funkcí
    printi_builtin(&tools->string);
    // Návěští konce programu
    printi(format[_label], "&$main");
    // Závěrečné sjednocení řetězců cílového kódu
    str_unify(&tools->string, &tools->string_tmp);
}

// Funkce zpracování prologu programu
void prolog(parser_tools_t* tools) {
    expect_types(tools, 1, tok_t_const); OK;  

    // Kontrola identifikátoru konstanty prologu
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

    // Kontrola zadaného souboru v řetězci prologu
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

    // Vytvoření hlavní symtable
    symtable_insert_builtin(tools->current_symtable, &tools->error); OK;

    // Generace hlavičky cílového kódu
    printi(".ifjcode24\n");
}

// Funkce pro zpracování funkce
void function(parser_tools_t* tools) {
    tools->current_symtable = DLL_Insert_last(&tools->sym_list, &tools->error); OK;

    expect_types(tools, 1, tok_t_pub); OK;

    next_token(tools);
    expect_types(tools, 1, tok_t_fn); OK;

    next_token(tools);
    expect_types(tools, 1, tok_t_sym); OK;
    stack_push(&tools->stack_codegen, tools->current_token);

    // Zaznamenání funkce do symtable
    tools->current_symtable = DLL_GetFirst(&tools->sym_list);
    tools->function_data = symtable_get_item(tools->current_symtable, tools->current_token->attribute, &tools->error); OK;
    tools->current_symtable = DLL_GetLast(&tools->sym_list);

    // Generace návěští funkce
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

    // Vyhledání funkce v symtable
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

    // Zpracování dalších funkcí
    next_token(tools);
    function_next(tools); OK;
}

// Funkce zpracování dalších případných funkcí
void function_next(parser_tools_t* tools) {
    if (tools->current_token->type != tok_t_eof) {
        function(tools); OK;
    } else {
        expect_types(tools, 1, tok_t_eof); OK;
    }
}

//Funkce pro zpracování parametrů funkce
void parameter(parser_tools_t* tools) {
    if (tools->current_token->type == tok_t_sym) {
        // Generace proměnné pro parametr
        sprintf(tools->string_buffer, "LF@%s", tools->current_token->attribute);
        printi(format[_defvar], tools->string_buffer);
        printi(format[_pops], tools->string_buffer);

        check_redefinition(tools); OK;

        // Uložení parametru do symtable
        tools->left_data = symtable_insert(tools->current_symtable, tools->current_token->attribute, &tools->error); OK;
        tools->left_data->isConst = true;
        tools->left_data->modified = true;

        next_token(tools);
        expect_types(tools, 1, tok_t_colon); OK;

        // Zpracování typu parametru
        next_token(tools);
        tools->varOrFunc = true;
        type(tools); OK;

        next_token(tools);
        parameter_next(tools); OK;
    }
}

// Funkce zpracování dalších případných parametrů funkce
void parameter_next(parser_tools_t* tools) {
    if (tools->current_token->type == tok_t_com) {
        next_token(tools);
        parameter(tools); OK;
    }
}

// Funkce zpracování těla funkce
void body(parser_tools_t* tools) {
    // Pokud je token začátkem příkazu, tak se zpracuje příkaz
    if (tools->current_token->type == tok_t_sym || tools->current_token->type == tok_t_unused || tools->current_token->type == tok_t_const || tools->current_token->type == tok_t_var || tools->current_token->type == tok_t_if ||
        tools->current_token->type == tok_t_while || tools->current_token->type == tok_t_for || tools->current_token->type == tok_t_return || tools->current_token->type == tok_t_break || tools->current_token->type == tok_t_continue) {
        statement(tools); OK;
        body(tools); OK;
    }
}

// Funkce zpracování jednotlivých příkazů
void statement(parser_tools_t* tools) {
    bool constFlag = false; // Příznak konstanty
    char destination[MAX_STRING_LEN]; // Dočasné uložení cílové proměnné
    tools->left_data = NULL;
    tools->params = false;
    // Získání aktuální symtable
    DLL_Last(&tools->sym_list);
    tools->current_symtable = DLL_GetCurrent(&tools->sym_list);

    switch (tools->current_token->type) {
    case tok_t_unused: //_
        tools->current_context = CONTEXT_NONE;

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

        next_token(tools);
        expect_types(tools, 1, tok_t_sym); OK;

        check_redefinition(tools); OK;

        // Uložení proměnné do symtable
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

        // Vytvoření nové symtable pro tělo if
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

        // Uvolnění symtable pro tělo if
        DLL_Delete_last(&tools->sym_list, &tools->error); OK;

        // Vytvoření nové symtable pro tělo else
        tools->current_symtable = DLL_Insert_last(&tools->sym_list, &tools->error); OK;
        
        // Tělo negativního výsledku podmínky
        else_then(tools); OK;

        //Uvolnění symtable pro tělo else
        DLL_Delete_last(&tools->sym_list, &tools->error); OK;

        // Generace návěští pro konec if
        sprintf(tools->string_buffer, "*$if%i", if_number);
        printi(format[_label], tools->string_buffer);
        break;

    case tok_t_while: // while
        tools->current_context = CONTEXT_CONDITION;
        int cycle_while = tools->cycle; // Uložení cyklu while
        tools->counter_codegen_while += 2;
        int while_number = tools->counter_codegen_while; // Uložení počitadla cyklů while
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

        // Vytvoření nové symtable pro tělo while
        tools->current_symtable = DLL_Insert_last(&tools->sym_list, &tools->error); OK;

        // Případné provedení výrazu podmíněného neprázdnou hodnotou
        next_token(tools);
        sprintf(tools->string_buffer, "LF@%%while%i", while_number);
        not_null_value(tools); OK;

        // Tělo pozitivního výsledku podmínky
        then(tools); OK;
        // Uvolnění symtable pro tělo while
        DLL_Delete_last(&tools->sym_list, &tools->error); OK;

        // Generace skoku na začátek while
        sprintf(tools->string_buffer, "*$while%i", while_number);
        printi(format[_jump], tools->string_buffer);

        // Generace návěští pro konec while
        sprintf(tools->string_buffer, "!$while%i", while_number);
        printi(format[_label], tools->string_buffer);
        tools->cycle = cycle_while;

        // Vytvoření nové symtable pro tělo else
        tools->current_symtable = DLL_Insert_last(&tools->sym_list, &tools->error); OK;

        // Tělo negativního výsledku podmínky
        else_then(tools); OK;
        // Uvolnění symtable pro tělo else
        DLL_Delete_last(&tools->sym_list, &tools->error); OK;

        // Generace návěští pro konec while
        sprintf(tools->string_buffer, "&$while%i", while_number);
        printi(format[_label], tools->string_buffer);
        break;

    case tok_t_for: //for
        tools->current_context = CONTEXT_CONDITION_FOR;
        int cycle_for = tools->cycle; // Uložení cyklu for
        tools->counter_codegen_for += 2;
        int for_number = tools->counter_codegen_for; // Uložení počitadla cyklů for
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

        // Vytvoření nové symtable pro tělo for
        tools->current_symtable = DLL_Insert_last(&tools->sym_list, &tools->error); OK;

        next_token(tools);
        expect_types(tools, 1, tok_t_alias); OK;

        next_token(tools);
        expect_types(tools, 2, tok_t_sym, tok_t_unused); OK;

        // Generace proměnné pro uložeí hodnoty výrazu ve for
        if (tools->current_token->type == tok_t_sym) { // ID
            // Generace proměnné
            sprintf(destination, "LF@%s", tools->current_token->attribute);
            printi(format[_defvar], destination);

            check_redefinition(tools); OK;
            // Uložení proměnné do symtable
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
        // Podmínka pro ukončení cyklu for
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

        // Uvolnění symtable pro tělo for
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
}

// Funkce zpracování příkazu počínající symbolem
void id_statement(parser_tools_t* tools) {
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
}

// Funkce zpracování hodnoty
void value(parser_tools_t* tools) {
    if(tools->current_token->type == tok_t_str || tools->current_token->type == tok_t_mstr){
        if(tools->left_data != NULL){
            if(tools->left_data->type != DATA_TYPE_UND){
                fprintf(stderr, "Semantic error: cannot cast string to ID.\n");
                tools->error = err_dt_invalid;
                return;
            }
            else{
                fprintf(stderr, "Semantic error: cannot deduce type from string.\n");
                tools->error = err_dt_unknown;
                return;
            }
        }
    }

    expect_types(tools, 9, tok_t_null, tok_t_int, tok_t_flt, tok_t_true, tok_t_false, tok_t_as, tok_t_sym, tok_t_lpa, tok_t_not); OK;

    // Zpracování výrazu
    expression(tools); OK;
}

// Funkce přiřazení hodnoty výrazu podmíněného neprázdnou hodnotou
void not_null_value(parser_tools_t* tools) {
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
            // Uložení proměnné do symtable
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
}

// Funkce zpracování těla cyklů a podmínek
void then(parser_tools_t* tools) {
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
    --tools->depth; // Snížení hloubky
}

// Funkce zpracování těla else
void else_then(parser_tools_t* tools) {
    if (tools->current_token->type == tok_t_else) { // Poku je else
        next_token(tools);
        then(tools); OK;

        bool else_returned = false; // Příznak, zdali else větev obsahuje return
        for (int index = tools->depth_sequence.size - 1; index >= 0; index--) { // Pro každou již procházenou hloubku
            if (tools->depth_sequence.data[index] <= tools->depth + 1 && tools->depth_sequence.data[index] > 0  ||
            tools->depth_sequence.data[index] > -tools->depth - 1 && tools->depth_sequence.data[index] < 0) { // Pokud jsme narazili na hloubku, kterou nalezený return již neovliňuje
                break;
            } else if (else_returned && tools->depth_sequence.data[index] == -1 - tools->depth) { // pokud else větev obsahuje return a kladná taktéž
                // Označení všech nižších či shodných hloubek do první vyšší, které byly již procházeny, za navrácené
                for (int index2 = tools->depth_sequence.size - 1; index2 >= 0; index2--) { 
                    if (tools->depth_sequence.data[index2] > 0) {
                        tools->depth_sequence.data[index2] *= -1; 
                    }
                    if (tools->depth_sequence.data[index2] <= tools->depth && tools->depth_sequence.data[index2] > 0 ||
                    tools->depth_sequence.data[index2] >= -tools->depth && tools->depth_sequence.data[index2] < 0) { // Pokud jsme narazili na hloubku, kterou nalezený return již neovliňuje
                        break;
                    }
                }
                break;
            } else if (tools->depth_sequence.data[index] == -1 - tools->depth) { // else větev obsahuje return
                else_returned = true;
            }
        }
    }
}

// Funkce zpracování symbolu
void id_continue(parser_tools_t* tools) {
    if (tools->current_token->type == tok_t_dot || tools->current_token->type == tok_t_lpa) { // Symbol je funkce
        char source[MAX_STRING_LEN]; // Dočasné uložení cílové proměnné
        strcpy(source, tools->string_buffer_value);

        tools->is_left = false;
        call(tools); OK;
        strcpy(tools->string_buffer_value, source);

        // Příprava názvu generované proměnné pro uchování návratové hodnoty
        sprintf(tools->string_buffer_value, "TF@%%retval");

        next_token(tools);
    } else { // Symbol je proměnná
        // Vyhledání symbolu mezi v symtable
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
        if (tools->params) { // Symbol je parametr
            tools->current_symtable = DLL_GetLast(&tools->sym_list);
        }
        if(tools->sym_list.current == tools->sym_list.first || data == NULL) { // Symbol nenalezen
            if (tools->params) { // Symbol je parametr
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
}

// Funkce zpracování návratové hodnoty funkce
void return_value(parser_tools_t* tools) {
    if (tools->current_token->type != tok_t_semicolon) {
        if(tools->function_data->type == DATA_TYPE_VOID){
            fprintf(stderr, "Semantic error: void function cannot return value\n");
            tools->error = err_ret_val;
            return;
        }
        // Zpracování návratové hodnoty
        value(tools); OK;

        // Generace proměnné přiřazení hodnoty k návratové proměnné
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
        if (tools->depth_sequence.data[index] <= tools->depth && tools->depth_sequence.data[index] > 0 ||
        tools->depth_sequence.data[index] >= -tools->depth && tools->depth_sequence.data[index] < 0) { // Pokud jsme narazili na hloubku, kterou nalezený return již neovliňuje
            break;
        }
    }
}

// Funkce zpracování volání funkce
void call(parser_tools_t* tools) {
    int param_count_save = tools->param_count; // Uložení počtu parametrů
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

        // Zpracování parametrů volané funkce
        next_token(tools);
        call_params(tools); OK;

        expect_types(tools, 1, tok_t_rpa); OK;

        // Získání názvu návěští funkce
        sprintf(tools->string_buffer, "$%s", stack_pop(&tools->stack_codegen)->attribute);
    }
    tools->param_count = param_count_save;

    printi(format[_call], tools->string_buffer);
}

// Funkce zpracování parametrů volání funkce
void call_params(parser_tools_t* tools) {
    tools->current_context = CONTEXT_NONE;
    char source[MAX_STRING_LEN]; // Dočasné uložení cílové proměnné
    if (tools->current_token->type != tok_t_rpa) { // Pokud jsou nějaké parametry
        // Zpracování hodnoty parametru
        tools->param_count++;
        data_t* data = tools->right_data; // Uložení informací o funkci
        int param_count_save = tools->param_count; // Uložení počtu parametrů
        // Uložení názvu pro sémantickou kontrolu
        char string_buffer_semantic_tmp[MAX_STRING_LEN];
        strcpy(string_buffer_semantic_tmp, tools->string_buffer_semantic);
        call_value(tools); OK;
        strcpy(tools->string_buffer_semantic, string_buffer_semantic_tmp);

        tools->right_data = data;

        // Kontrola parametru
        bool found = true; // Příznak, zdali byl nalezen parametr
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
                            found = false;
                        }
                    }
                    break;
                case DATA_TYPE_DOUBLE:
                    if (tools->result_data->canNull) {
                        if (data->parameters->data[param_count_save] != tok_t_f64_opt) {
                            found = false;
                        }
                    }
                    break;
                case DATA_TYPE_U8:
                    if (tools->result_data->canNull) {
                        if (data->parameters->data[param_count_save] != tok_t_u8_opt) {
                            found = false;
                        }
                    } else if (data->parameters->data[param_count_save] != tok_t_u8) {
                        found = false;
                    }
                    break;
                case DATA_TYPE_BOOLEAN:
                    if (data->parameters->data[param_count_save] != tok_t_bool) {
                        found = false;
                    }
                    break;
                case DATA_TYPE_RETYPABLE_DOUBLE:
                    if (data->parameters->data[param_count_save] == tok_t_flt || data->parameters->data[param_count_save] == tok_t_unused) { // Bez implicitního přetypování
                        sprintf(source, "float@%a", atof(tools->result_data->id)); // Přesunutí hodnoty do pomocné proměnné výrazu
                        printi(format[_move], tools->string_buffer_value, source);
                    } else if (data->parameters->data[param_count_save] == tok_t_i32) { // Implicitní přetypování
                        sprintf(source, "int@%i", atoi(tools->result_data->id)); // Přesunutí přetypované hodnoty do pomocné proměnné výrazu
                        printi(format[_move], tools->string_buffer_value, source);
                    } else {
                        found = false;
                    }
                    break;
                default:
                    break;
            }
        }
        if (!found && data->parameters->data[param_count_save] != tok_t_unused) { // Kontrola typu parametru
            fprintf(stderr, "Semantic error: Function parameter type mismatch\n");
            tools->error = err_param;
            return;
        }
        else{
            tools->error = err_none;
        }

        // Generace přiřazení hodnoty parametru
        token_type tmp_type = stack_peek(&tools->stack_codegen)->type; // Uložení typu parametru
        strcpy(source, tools->string_buffer_value);
        bool string_value = false; // Příznak, zdali je parametrem řetězec
        if (stack_peek(&tools->stack_codegen)->type == tok_t_str || stack_peek(&tools->stack_codegen)->type == tok_t_mstr) {
            stack_pop(&tools->stack_codegen);
            string_value = true;
        } else if (strstr(source, "TF@%retval") != NULL) {
            // Generace uležení navrácené hodnoty funkce mimo dočasný rámec
            sprintf(source, "LF@%%%%%%%i", tools->counter_global++);
            printi(format[_defvar], source);
            printi(format[_move], source, "TF@%retval");
            printi("%s" ,format[_createframe]); // Vytvoření nového rámce
        }

        // Zpracování dalších parametrů
        call_params_next(tools); OK;
        if (tools->param_count != data->parameters->size -1 && tools->param_count != -2) { // Kontrola počtu parametrů
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
}

// Funkce zpracování dalších parametrů volání funkce
void call_params_next(parser_tools_t* tools) {
    if (tools->current_token->type == tok_t_com) {
        next_token(tools);
        call_params(tools); OK;
    }
}

// Funkce zpracování hodnoty parametru
void call_value(parser_tools_t* tools) {
    if (tools->current_token->type == tok_t_str || tools->current_token->type == tok_t_mstr) { // Hodnota je řetězec
        strcpy(tools->string_buffer_value, tools->current_token->attribute);
        stack_push(&tools->stack_codegen, tools->current_token);
        next_token(tools);
    } else {
        // Zpracování hodnoty parametru volané funkce
        value(tools); OK;
    }
}

// Funkce zpracování návratového typu funkce
void return_type(parser_tools_t* tools) {
    if (tools->current_token->type != tok_t_void) {
        tools->varOrFunc = false;
        type(tools); OK;
        // Generace proměnné pro návratovou hodnotu
        printi(format[_defvar], "LF@%retval");
    } else {
        expect_types(tools, 1, tok_t_void); OK;
    }
}

// Funkce zpracování typu symbolu
void type(parser_tools_t* tools) {
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
}

// Funkce zpracování typu proměnné
void definition(parser_tools_t* tools) {
    if (tools->current_token->type == tok_t_colon) { // Příkaz obsahuje přímou definici proměnné
        next_token(tools);
        tools->varOrFunc = true;
        type(tools); OK;

        next_token(tools);
    }
}

// Funkce zpracování hodnoty pro for cyklus
void for_value(parser_tools_t* tools) {
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
}

// Funkce prvního průchodu parseru zdrojovým kódem
void parse_fn_first(parser_tools_t* tools) {
    // Získání prvního tokenu
    next_token_initial(tools); OK;
    // Zpracování zdrojového kódu
    while (tools->current_token->type != tok_t_eof) {
        if (tools->current_token->type == tok_t_fn) {
            next_token_initial(tools); OK;
            expect_types(tools, 1, tok_t_sym); OK;

            check_redefinition(tools); OK;

            // Vytvoření nového záznamu v symtable
            tools->left_data = symtable_insert(tools->current_symtable, tools->current_token->attribute, &tools->error); OK;
            tools->left_data->modified = true;
            
            next_token_initial(tools); OK;
            expect_types(tools, 1, tok_t_lpa); OK; // (

            next_token_initial(tools); OK;
            
            // Zpracování parametrů funkce
            bool not_first = false; // Příznak, zdali je zpracováván první parametr
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

                // Zaznamenání parametru funkce
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

// Funkce inicializace struktury parser_tools_t
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

// Funkce uvolnění struktury parser_tools_t
void parser_tools_destroy(parser_tools_t* tools) {
    tok_free(tools->current_token);
    DLL_Destroy(&tools->sym_list, &tools->error);
    dynamic_array_destroy(&tools->depth_sequence);
    str_destroy(&tools->string);
    str_destroy(&tools->string_tmp);
    str_destroy(&tools->string_defvar);
}

// Hlavní funkce parseru
err_codes parse(Scanner_ptr scanner) {
    // Inicializace struktury parser_tools_t
    parser_tools_t tools;
    tools.scanner = scanner;
    parser_tools_init(&tools);

    tools.current_symtable = DLL_Insert_last(&tools.sym_list, &tools.error);
    if (tools.error != err_none) {
        return tools.error;
    }

    // První průchod parseru zdrovým kódem a naplnění tabulky symbolů funkcemi
    parse_fn_first(&tools);
    
    // Druhý průchod parseru zdrovým kódem s úplnou syntaktická analýza
    if (tools.error == err_none) {
        program(&tools); 

        if(tools.error == err_none) {
            // Získání definoice funkce main
            tools.left_data = symtable_get_item(tools.current_symtable, "main", &tools.error);
            if(tools.left_data == NULL) { // Ověření existence funkce main
                fprintf(stderr, "Semantic error: main is undefined function\n");
                tools.error = err_undef;
                return tools.error; 
            }
            if(tools.left_data->type != DATA_TYPE_VOID || tools.left_data->parameters->size != 0) { // Ověření parametrů funkce main
                fprintf(stderr, "Semantic error: main must be void and have no parameters\n");
                tools.error = err_param;
            }
            tools.left_data->used = true;

            str_printout(&tools.string); // Výpis výsledného kódu při úspěšném průchodu
        }
    }
    
    // Uvolnění nástrojů parseru
    parser_tools_destroy(&tools);

    return tools.error;
}

/*** Konec souboru parser.c ***/