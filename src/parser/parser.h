/** IFJ2024
 * xondre16; Jozef Ondrejička
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "../utils/token_types.h"
#include "../scanner/scanner.h"
#include "../utils/token.h"

// Global variables
extern Token_ptr current_token;
extern Scanner_ptr scanner;
extern token_type fn_ret_type;
extern bool ifj_flag;
extern int statement_index;

// Function declarations
void print_token();
void next_token();
void expect_type(token_type type);
void expect_attribute(const char* attr);


// Grammar functions
int program();
int prolog();
int function();
int function_next();
int parameter();
int parameter_next();
int body();
int statement();
int id_statement();
int value();
int not_null_value();
int then();
int else_then();
int id_continue();
int return_value();
int call();
int call_params();
int call_value();
int call_params_next();

// Precedence parsing
extern char precedence_table[17][17];
int get_precedence_index(token_type type);
int getPrecedence(token_type stackTop, token_type input);
void push_operator(token_type op, token_type* operator_stack, int* operator_top);
token_type pop_operator(token_type* operator_stack, int* operator_top);
void push_operand(Token_ptr operand, Token_ptr* operand_stack, int* operand_top);
Token_ptr pop_operand(Token_ptr* operand_stack, int* operand_top);
char get_precedence_relation(token_type stack_top, token_type current);
void parse_expression(Token_ptr stored_token);

void parse();

#endif // PARSER_H