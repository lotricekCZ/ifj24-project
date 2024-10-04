#ifndef LEXEME_H
#define LEXEME_H
#include "keywords.h"

/**
 * @struct _Lexeme
 * @brief Struktura reprezentující lexém
 *
 * Struktura _Lexeme reprezentuje jeden lexém, tzn. jeden token, který  je
 * výstupem lexikální analýzy. Lexém má  atribut type, který  udává typ
 * tokenu, a atribut attribute, který udává hodnotu tokenu.
 */
typedef struct _Lexeme
{
    token_type type; /** < Typ tokenu */
    char *attribute; /** < Hodnota tokenu */
} Lexeme;

typedef struct _Lexeme *Lexeme_ptr;

Lexeme_ptr lex_init(token_type type);

void lex_free(Lexeme_ptr lex);

void lex_print(Lexeme_ptr lex);

token_type lex_get_type(Lexeme_ptr lex);

char *lex_get_attribute(Lexeme_ptr lex);

void lex_set_attribute(Lexeme_ptr lex, char *attribute);

void lex_set_type(Lexeme_ptr lex, token_type type);

void lex_set_type_attribute(Lexeme_ptr lex, token_type type, char *attribute);
#endif // LEXEME_H