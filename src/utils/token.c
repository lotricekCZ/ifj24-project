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
Token_ptr tok_init(token_type type)
{
    Token_ptr new_lex = (Token_ptr)malloc(sizeof(struct _Token));
    new_lex->type = type;
    new_lex->attribute = NULL;
    return new_lex;
}
/**
 * @brief Inicializace lexemu
 *
 * Funkce inicializuje strukturu reprezentujici lexem.
 * @return ukazatel na inicializovanou strukturu
 */
Token_ptr tok_init_no_type()
{
    Token_ptr new_lex = (Token_ptr)malloc(sizeof(struct _Token));
    new_lex->type = tok_t_init;
    new_lex->attribute = NULL;
    return new_lex;
}

/**
 * @brief Uvolni pamet, kterou zabira token
 *
 * Funkce uvolni pamet, kterou zabira a jeho atribut.
 * @param tok ukazatel na lexem, ktery  chceme uvolnit
 */
void tok_free(Token_ptr tok)
{
    if (tok->attribute != NULL)
    {
        free(tok->attribute);
    }
    free(tok);
}

/**
 * @brief Zkopiruje token
 *
 * Funkce zkopiruje dany token a vrati ukazatel na novy token.
 * @param tok token, ktery chceme zkopirovat
 * @return ukazatel na novy token
 */
void tok_copy(Token_ptr dst, Token tok)
{
    tok_set_type(dst, tok.type);
    if (tok.attribute != NULL)
    {
        tok_set_attribute(dst, tok.attribute);
    }
}

void tok_print(Token_ptr tok)
{
}

/**
 * @brief Ziska typ tokenu
 *
 * Funkce vraci typ tokenu.
 * @param tok token, jehoz typ chceme ziskat
 * @return typ tokenu
 */
token_type tok_get_type(Token_ptr tok)
{
    return tok->type;
}

/**
 * @brief Ziska hodnotu atributu tokenu
 *
 * Funkce vraci hodnotu atributu tokenu.
 * @param tok token, jehoz atribut chceme ziskat
 * @return hodnota atributu
 */
char *tok_get_attribute(Token_ptr tok)
{
    return tok->attribute;
}

/**
 * @brief Nastavi hodnotu atributu tokenu
 *
 * Funkce nastavi hodnotu atributu tokenu na novou hodnotu.
 * Pokud je atribut jiz nastaven, je stara hodnota uvolnena.
 * @param tok token, kteremu chceme nastavit atribut
 * @param attribute nova hodnota atributu
 */
void tok_set_attribute(Token_ptr tok, char *attribute)
{
    if (tok->attribute != NULL)
    {
        free(tok->attribute);
        tok->attribute = NULL;
    }
    tok->attribute = (char *)malloc(sizeof(char) * (strlen(attribute) + 1));
    strcpy(tok->attribute, attribute);
}

/**
 * @brief Nastavi typ tokenu
 *
 * Funkce nastavi typ tokenu na novy typ.
 * @param tok token, jehoz typ chceme nastavit
 * @param type novy typ tokenu
 */
void tok_set_type(Token_ptr tok, token_type type)
{
    tok->type = type;
}

/**
 * @brief Nastavi typ i hodnotu atributu tokenu
 *
 * Funkce nastavuje typ tokenu na novy typ a nastavi hodnotu atributu
 * na novou hodnotu.
 * @param tok token, jehoz typ a hodnota atributu chceme nastavit
 * @param type novy typ tokenu
 * @param attribute nova hodnota atributu
 */
void tok_set_type_attribute(Token_ptr tok, token_type type, char *attribute)
{
    tok_set_type(tok, type);
    tok_set_attribute(tok, attribute);
}
