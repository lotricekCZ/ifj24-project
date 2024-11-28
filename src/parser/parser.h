/** IFJ2024
 * xondre16; Jozef Ondrejička
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "../utils/token_types.h"
#include "../scanner/scanner.h"
#include "../utils/token.h"
#include "../utils/symtable.h"
#include "../utils/errors.h"

#include "../scanner/scanner.h"
#include "../utils/token_types.h"
#include "../utils/codegen.h"
#include "../utils/stack.h"
#include "../utils/symtable.h"
#include "../utils/symDLList.h"
#include "../utils/str.h"
#include "../utils/dynamic_array.h"

// Global variables
extern Token_ptr current_token;
extern Scanner_ptr scanner;
extern token_type fn_ret_type;
extern bool ifj_flag;
extern bool first_parse_done;
extern int statement_index;

// Contex enum
typedef enum {
    CONTEXT_CONDITION,
    CONTEXT_NONE,
    CONTEXT_SYMBOL,
    CONTEXT_RETURN,
    CONTEXT_CONDITION_FOR
}context_t;

// Contex enum
typedef struct {
    Token_ptr current_token;
    Scanner_ptr scanner;
    err_codes error;

    Stack stack_codegen;
    int counter_codegen_if;
    int counter_codegen_while;
    int counter_codegen_for;
    int counter_global;
    int cycle;
    char string_buffer[MAX_STRING_LEN];
    char string_buffer_value[MAX_STRING_LEN];
    str_t string;
    str_t string_tmp;
    str_t string_defvar;

    symtable_t* current_symtable;
    int current_context;
    DLList sym_list;
    data_t *left_data;
    data_t *right_data;
    data_t *result_data;
    data_t *function_data;
    int depth;
    dynamic_array_t depth_sequence;
    char stringBuffer[MAX_STRING_LEN];
    int param_count;
    bool params;
} parser_tools_t;

// Function declarations
void print_token();
void next_token();
void expect_type(token_type type);
void expect_types(int count, ...);
void expect_attribute(const char* attr);


// Grammar functions
void program();
void prolog();
void function();
void function_next();
void parameter();
void parameter_next();
void body();
void statement();
void id_statement();
void value();
void not_null_value();
void then();
void else_then();
void id_continue();
void return_value();
void call(bool is_left);
void call_params();
void call_value();
void call_params_next();
void return_type();
void type(bool varOrFunc);
void definition();
void for_value();

// Precedence parsing
extern char precedence_table[19][19];
int get_precedence_index(token_type type);
int getPrecedence(token_type stackTop, token_type input);
void push_operator(token_type op, token_type* operator_stack, int* operator_top);
token_type pop_operator(token_type* operator_stack, int* operator_top);
void push_operand(Token_ptr operand, Token_ptr* operand_stack, int* operand_top);
Token_ptr pop_operand(Token_ptr* operand_stack, int* operand_top);
char get_precedence_relation(token_type stack_top, token_type current);
void parse_expression();
//void expression_processing(Token_ptr (*postfix)[MAX], int* postfix_index, Stack* stack_functions);

err_codes parse();
void parse_fn_first();

/**
 * @brief Zkontroluje, id, které je právě vkládáno, není redefinováno.
 *
 * Projde se od současné symbolické tabulky až k té první.
 * Pokud najde stejné id, tak nastane chyba.
 */
void check_redefinition();

#endif // PARSER_H