/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

#ifndef SCANNER_H
#define SCANNER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "../utils/token.h"
#include "../utils/token_dll.h"

typedef struct _Scanner {
	FILE *file;
	tok_dllist *list;
} Scanner;

Scanner *scanner_init(FILE *file);
void scanner_free(Scanner *scanner);
bool scanner_scan(Scanner *scanner);

#endif