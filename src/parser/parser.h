/** IFJ2024
 * xondre16; Jozef Ondrejička
 */

#ifndef PARSER_H
#define PARSER_H

#include "../scanner/scanner.h"
#include "../utils/token.h"
#include "../utils/token_types.h"

// Function declarations
int statement();
int declaration();
int assignment();
int statement_next();
int parameter_next();
int parameter();
int function_declaration();
int function_declaration_next();
int program();
int prolog();
void parse();

#endif // PARSER_H