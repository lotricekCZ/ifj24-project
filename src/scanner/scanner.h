/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "../utils/token.h"
#include "../utils/token_dll.h"
#include "../utils/keywords.h"
#include "scan_state.h"

typedef struct _Scanner {
	char *file_name;
	tok_dllist *list;
} Scanner;

typedef struct _Scanner *Scanner_ptr;

Scanner_ptr scn_init(char *filename);
void scn_free(Scanner_ptr scanner);
bool scn_scan(Scanner_ptr scanner);

Token_ptr scn_get_token(Scanner_ptr scanner);
Token_ptr scn_previous(Scanner_ptr scanner);
Token_ptr scn_next(Scanner_ptr scanner);
char *scn_open_file(Scanner_ptr scanner);

#endif