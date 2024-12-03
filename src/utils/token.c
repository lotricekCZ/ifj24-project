/**
 * @addtogroup IFJ2024
 * @file token.c
 * @brief Implementace tokenu
 * @author xramas01; Jakub Ramaseuski
 * 
 * Implementace funkcí pro práci s tokenem.
 */

#include "token.h"
#include "memory_table.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Funkce inicializuje strukturu reprezentujici lexem a navraci ukazatel na inicializovanou strukturu.
Token_ptr tok_init(token_type type)
{
    Token_ptr new_lex = (Token_ptr)imalloc(sizeof(struct _Token));
    new_lex->type = type;
    new_lex->attribute = NULL;
    return new_lex;
}

// Funkce inicializuje strukturu reprezentujici lexem.
void tok_init_no_ret(Token_ptr* tok)
{
    *tok = tok_init(tok_t_init);
}

// Funkce inicializuje strukturu reprezentujici lexem.
Token_ptr tok_init_no_type()
{
    return tok_init(tok_t_init);
}

// Funkce zkopiruje dany token a vrati ukazatel na novy token.
void tok_copy(Token_ptr dst, Token tok)
{
    tok_set_type(dst, tok.type);
    if (tok.attribute != NULL)
    {
        tok_set_attribute(dst, tok.attribute);
    }
}

// Funkce uvolni pamet, kterou zabira a jeho atribut.
void tok_free(Token_ptr tok)
{
    if (tok->attribute != NULL)
    {
        ifree(tok->attribute);
    }
    ifree(tok);
}

// Funkce vraci typ tokenu.
token_type tok_get_type(Token_ptr tok)
{
    return tok->type;
}

// Funkce navraci hodnotu atributu tokenu.
char *tok_get_attribute(Token_ptr tok)
{
    return tok->attribute;
}

// Funkce nastavi hodnotu atributu tokenu na novou hodnotu.
void tok_set_attribute(Token_ptr tok, char *attribute)
{
    if (tok->attribute != NULL)
    {
        ifree(tok->attribute);
        tok->attribute = NULL;
    }
    tok->attribute = (char *)imalloc(sizeof(char) * (strlen(attribute) + 1));
    strcpy(tok->attribute, attribute);
}

// Funkce nastavi typ tokenu na novy typ.
void tok_set_type(Token_ptr tok, token_type type)
{
    tok->type = type;
}

// Funkce nastavuje typ tokenu na novy typ a nastavi hodnotu atributu na novou hodnotu.
void tok_set_type_attribute(Token_ptr tok, token_type type, char *attribute)
{
    tok_set_type(tok, type);
    tok_set_attribute(tok, attribute);
}

/*** Konec souboru token.c ***/