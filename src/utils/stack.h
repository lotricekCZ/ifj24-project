/**
 * @addtogroup IFJ2024
 * @file stack.h
 * @brief Knihovna pro práci se zásobníkem
 * @author xondre16; Jozef Ondrejička
 * 
 * Knihovna obsahující definice struktury zásobníku a funkcí pro práci se strukturou zásobník.
 */

#include <stdbool.h>
#include "token.h"

#ifndef STACK_H
#define STACK_H

/**
 * @brief Maximální velikost zásobníku
 */
#define MAX 1000

/**
 * @struct Stack
 * @brief Struktura zásobníku
 * 
 * Struktura obsahuje pole prvků zásobníku a ukazatel na vrchol zásobníku.
 */
typedef struct {
    Token_ptr items[MAX];   /**< Pole prvků zásobníku */
    int top;                /**< Ukazatel na vrchol zásobníku */
} Stack;

/**
 * @brief Inicializace zásobníku
 * 
 * Funkce inicializuje zásobník a nastaví vrchol zásobníku na -1.
 * 
 * @param stack Ukazatel na strukturu zásobníku
 */
void stack_init(Stack *stack);

/**
 * @brief Funkce pro prázdný zásobník
 * 
 * Funkce zkontroluje, zda je zásobník prázdný.
 * 
 * @param stack Ukazatel na strukturu zásobníku
 * @return Pravdivostní hodnota, zda je zásobník prázdný
 */
bool stack_isEmpty(Stack *stack);

/**
 * @brief Funkce pro přidání prvku na zásobník
 * 
 * Funkce přidá prvek na zásobník. Pokud je zásobník plný, vypíše chybovou hlášku.
 * 
 * @param stack Ukazatel na strukturu zásobníku
 * @param value Ukazatel na token, který se má přidat na zásobník
 */
void stack_push(Stack *stack, Token_ptr value);

/**
 * @brief Funkce pro odebrání prvku ze zásobníku
 * 
 * Funkce odebrání prvek ze zásobníku. Pokud je zásobník prázdný, vypíše chybovou hlášku.
 * 
 * @param stack Ukazatel na strukturu zásobníku
 * @return Ukazatel na odebraný prvek
 */
Token_ptr stack_pop(Stack *stack);

/**
 * @brief Funkce pro náhled na vrchol zásobníku
 * 
 * Funkce vrací ukazatel na prvek na vrcholu zásobníku. Pokud je zásobník prázdný, vrací NULL.
 * 
 * @param stack Ukazatel na strukturu zásobníku
 * @return Ukazatel na prvek na vrcholu zásobníku
 */
Token_ptr stack_peek(Stack *stack);

#endif /* STACK_H */

/*** Konec souboru stack.c ***/