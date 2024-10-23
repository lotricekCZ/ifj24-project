#include <stdio.h>
#include "parser.h"
#include "../scanner/scanner.h"
#include "../utils/token.h"
#include "../utils/token_types.h"

void parse() {
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
	} while (token->type != tok_t_eof && token->type != tok_t_error);

    // Free the scanner
    scn_free(scanner);
}