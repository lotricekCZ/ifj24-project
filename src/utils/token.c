#include "token.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @brief Inicializace lexemu
 *
 * Funkce inicializuje strukturu reprezentujici lexem.
 * @param type typ lexemu
 * @return ukazatel na inicializovanou strukturu
 */
Token_ptr tok_init(token_type type) {
    Token_ptr new_lex = (Token_ptr) malloc(sizeof(struct _Token));
    new_lex->type = type;
    new_lex->attribute = NULL;
    return new_lex;
}

void tok_free(Token_ptr tok) {
    if (tok->attribute != NULL) {
        free(tok->attribute);
    }
    free(tok);
}


void tok_print(Token_ptr tok) {
    
}

token_type tok_get_type(Token_ptr tok) {
    return tok->type;
}

char *tok_get_attribute(Token_ptr tok) {
    return tok->attribute;
}

void tok_set_attribute(Token_ptr tok, char *attribute) {
    if (tok->attribute != NULL) {
        free(tok->attribute);
        tok->attribute = NULL;
    }
    tok->attribute = (char *) malloc(sizeof(char) * (strlen(attribute) + 1));
    strcpy(tok->attribute, attribute);
}

void tok_set_type(Token_ptr tok, token_type type) {
    tok->type = type;
}

void tok_set_type_attribute(Token_ptr tok, token_type type, char *attribute) {
    tok_set_type(tok, type);
    tok_set_attribute(tok, attribute);
}
