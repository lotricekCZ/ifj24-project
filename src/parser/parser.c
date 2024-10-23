#include <stdio.h>
#include "parser.h"
#include "../scanner/scanner.h"
#include "../utils/token.h"
#include "../utils/token_types.h"

int statement() 
{
    return 0;
};

int declaration() 
{
    return 0;
};

int assignment() 
{
    return 0;
};


int statement_next() 
{
    return 0;
};

int statement() 
{
    return 0;
};

int parameter_next() 
{
    return 0;
};

int parameter() 
{
    return 0;
};

int function_declaration() 
{
    return 0;
};

int function_declaration_next() 
{
    return 0;
};

int program() 
{
    return 0;
};

int function_declaration() 
{
    return 0;
};

int prolog() 
{
    return 0;
};

int program() 
{
    return 0;
};

void parse() 
{
    printf("No nazdar!\n");

    // Initialize the scanner with the source code file
    Scanner_ptr scanner = scn_init("test/scanner/test2.zig");
    if (scanner == NULL) {
        fprintf(stderr, "Failed to initialize scanner.\n");
        return;
    }

    // Scan tokens
    Token_ptr token = NULL;
    do {
		if(token != NULL)
			tok_free(token);
        // Process the token
		token = scn_scan(scanner);
		printf("Token: %s\n", token->attribute); // Use the correct member
        printf("Token type: %d\n", token->type);
        // printf("Bomba!\n");
	} while (token->type != tok_t_eof && token->type != tok_t_error);


    // Free the scanner
    scn_free(scanner);
}