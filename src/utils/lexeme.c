#include "lexeme.h"
#include <stdio.h>

Lexeme_ptr lex_init(token_type type) {
    Lexeme_ptr new_lex = (Lexeme_ptr) malloc(sizeof(struct _Lexeme));
    new_lex->type = type;
    new_lex->attribute = NULL;
    return new_lex;
}

void lex_free(Lexeme_ptr lex) {
    if (lex->attribute != NULL) {
        free(lex->attribute);
    }
    free(lex);
}

void lex_print(Lexeme_ptr lex) {
    
}

token_type lex_get_type(Lexeme_ptr lex) {
    return lex->type;
}

char *lex_get_attribute(Lexeme_ptr lex) {
    return lex->attribute;
}

void lex_set_attribute(Lexeme_ptr lex, char *attribute) {
    lex->attribute = attribute;
}

void lex_set_type(Lexeme_ptr lex, token_type type) {
    lex->type = type;
}

void lex_set_type_attribute(Lexeme_ptr lex, token_type type, char *attribute) {
    lex->type = type;
    lex->attribute = attribute;
}
