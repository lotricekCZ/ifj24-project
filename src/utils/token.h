/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

#ifndef TOKEN_H
#define TOKEN_H
#include "token_types.h"

/**
 * @struct _Token
 * @brief Struktura reprezentující token
 *
 * Struktura _Token reprezentuje jeden token, který  je
 * výstupem lexikální analýzy. Token má atribut type, který udává typ
 * tokenu, a atribut attribute, který udává hodnotu tokenu.
 */
typedef struct _Token
{
    token_type type; /** < Typ tokenu */
    char *attribute; /** < Hodnota tokenu */
} Token;

typedef struct _Token *Token_ptr;

/**
 * @brief Inicializace lexemu
 *
 * Funkce inicializuje strukturu reprezentujici lexem.
 * Navraci ukazatel na inicializovanou strukturu.
 * 
 * @param type typ lexemu
 * @return ukazatel na inicializovanou strukturu
 */
Token_ptr tok_init(token_type type);

/**
 * @brief Inicializace lexemu
 *
 * Funkce inicializuje strukturu reprezentujici lexem.
 * 
 * @param tok Ukazatel na ukazaatel na lexem pro inicializaci
 * @return ukazatel na inicializovanou strukturu
 */
void tok_init_no_ret(Token_ptr* tok);

/**
 * @brief Inicializace lexemu
 *
 * Funkce inicializuje strukturu reprezentujici lexem.
 * 
 * @return ukazatel na inicializovanou strukturu
 */
Token_ptr tok_init_no_type();

/**
 * @brief Zkopiruje token
 *
 * Funkce zkopiruje dany token a vrati ukazatel na novy token.
 * 
 * @param dst ukazatel na novy token
 * @param tok token, ktery chceme zkopirovat
 * @return ukazatel na novy token
 */
void tok_copy(Token_ptr dst, Token tok);

/**
 * @brief Uvolni pamet, kterou zabira token
 *
 * Funkce uvolni pamet, kterou zabira a jeho atribut.
 * @param tok ukazatel na lexem, ktery  chceme uvolnit
 */
void tok_free(Token_ptr tok);

/**
 * @brief Ziska typ tokenu
 *
 * Funkce vraci typ tokenu.
 * @param tok token, jehoz typ chceme ziskat
 * @return typ tokenu
 */
token_type tok_get_type(Token_ptr tok);

/**
 * @brief Ziska hodnotu atributu tokenu
 *
 * Funkce vraci hodnotu atributu tokenu.
 * @param tok token, jehoz atribut chceme ziskat
 * @return hodnota atributu
 */
char *tok_get_attribute(Token_ptr tok);

/**
 * @brief Nastavi hodnotu atributu tokenu
 *
 * Funkce nastavi hodnotu atributu tokenu na novou hodnotu.
 * Pokud je atribut jiz nastaven, je stara hodnota uvolnena.
 * @param tok token, kteremu chceme nastavit atribut
 * @param attribute nova hodnota atributu
 */
void tok_set_attribute(Token_ptr tok, char *attribute);

/**
 * @brief Ziska typ tokenu
 *
 * Funkce vraci typ tokenu.
 * @param tok token, jehoz typ chceme ziskat
 * @param type novy typ tokenu
 * @return typ tokenu
 */
void tok_set_type(Token_ptr tok, token_type type);

/**
 * @brief Nastavi typ i hodnotu atributu tokenu
 *
 * Funkce nastavuje typ tokenu na novy typ a nastavi hodnotu atributu
 * na novou hodnotu.
 * @param tok token, jehoz typ a hodnota atributu chceme nastavit
 * @param type novy typ tokenu
 * @param attribute nova hodnota atributu
 */
void tok_set_type_attribute(Token_ptr tok, token_type type, char *attribute);
#endif /* TOKEN_H */

/*** Konec souboru token.h ***/