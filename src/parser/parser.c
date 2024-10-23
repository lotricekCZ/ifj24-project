#include <stdio.h>
#include "parser.h"
#include "scanner/scanner.h"
#include "utils/token.h"
#include "utils/token_types.h"

void parse() {
    printf("No nazdar!\n");

    // Initialize the scanner with the source code file
    Scanner_ptr scanner = scn_init("/homes/eva/xo/xondre16/IFJ_NEW/ifj24-project/src/parser/input.txt");
    if (scanner == NULL) {
        fprintf(stderr, "Failed to initialize scanner.\n");
        return;
    }

    // Scan tokens
    Token_ptr token;
    while ((token = scn_scan(scanner)) != NULL) {
        // Process the token
        printf("Token: %s\n", token->attribute); // Use the correct member
    }

    // Free the scanner
    scn_free(scanner);
}