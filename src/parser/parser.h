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

// Function declarations
void print_token();
void next_token();
void expect_type(token_type type);
void expect_attribute(const char* attr);

// Grammar functions
int program();
int prolog();
int function_declaration();
int function_declaration_next();
int parameter();
int parameter_next();
int statement();
int statement_next();
int assignment();
int declaration();
int fn_call_statement(bool ifj_flag);
int if_statement();
int while_statement();
int for_statement();
int return_statement();
int break_statement();
int continue_statement();
void parse();

#endif // PARSER_H