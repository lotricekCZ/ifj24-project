/**
 * @addtogroup IFJ2024
 * @file stack.c
 * @brief Implementace funkcí pro práci se zásobníkem
 * @author xondre16; Jozef Ondrejička
 * 
 * Implementace funkcí pro práci se zásobníkem.
 */

#include <stdio.h>
#include "stack.h"

/**
 * @brief Inicializace zásobníku
 * 
 * Funkce inicializuje zásobník a nastaví vrchol zásobníku na -1.
 * 
 * @param stack Ukazatel na strukturu zásobníku
 */
void stack_init(Stack *stack) {
    stack->top = -1;
}

/**
 * @brief Funkce pro prázdný zásobník
 * 
 * Funkce zkontroluje, zda je zásobník prázdný.
 * 
 * @param stack Ukazatel na strukturu zásobníku
 * @return Pravdivostní hodnota, zda je zásobník prázdný
 */
bool stack_isEmpty(Stack *stack) {
    return stack->top == -1;
}

/**
 * @brief Funkce pro přidání prvku na zásobník
 * 
 * Funkce přidá prvek na zásobník. Pokud je zásobník plný, vypíše chybovou hlášku.
 * 
 * @param stack Ukazatel na strukturu zásobníku
 * @param value Ukazatel na token, který se má přidat na zásobník
 */
void stack_push(Stack *stack, Token_ptr value) {
    if (stack->top == MAX - 1) {
        fprintf(stderr, "Stack overflow\n");
        return;
    }
    stack->items[++stack->top] = value;
}

/**
 * @brief Funkce pro odebrání prvku ze zásobníku
 * 
 * Funkce odebrá prvek ze zásobníku. Pokud je zásobník prázdný, vypíše chybovou hlášku.
 * 
 * @param stack Ukazatel na strukturu zásobníku
 * @return Ukazatel na odebraný prvek
 */
Token_ptr stack_pop(Stack *stack) {
    if (stack_isEmpty(stack)) {
        fprintf(stderr, "Stack underflow\n");
        return NULL;
    }
    return stack->items[stack->top--];
}

/**
 * @brief Funkce pro náhled na vrchol zásobníku
 * 
 * Funkce vrací ukazatel na prvek na vrcholu zásobníku. Pokud je zásobník prázdný, vrací NULL.
 * 
 * @param stack Ukazatel na strukturu zásobníku
 * @return Ukazatel na prvek na vrcholu zásobníku
 */
Token_ptr stack_peek(Stack *stack) {
    if (stack_isEmpty(stack)) {
        return NULL;
    }
    return stack->items[stack->top];
}

/*** Konec souboru stack.c ***/