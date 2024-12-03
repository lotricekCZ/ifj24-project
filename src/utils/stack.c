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

// Funkce inicializuje zásobník a nastaví vrchol zásobníku na -1.
void stack_init(Stack *stack) {
    stack->top = -1;
}

// Funkce zkontroluje, zda je zásobník prázdný.
bool stack_isEmpty(Stack *stack) {
    return stack->top == -1;
}

// Funkce přidá prvek na zásobník. Pokud je zásobník plný, vypíše chybovou hlášku.
void stack_push(Stack *stack, Token_ptr value) {
    if (stack->top == MAX - 1) {
        fprintf(stderr, "Stack overflow\n");
        return;
    }
    stack->items[++stack->top] = value;
}

// Funkce odebrá prvek ze zásobníku. Pokud je zásobník prázdný, vypíše chybovou hlášku.
Token_ptr stack_pop(Stack *stack) {
    if (stack_isEmpty(stack)) {
        fprintf(stderr, "Stack underflow\n");
        return NULL;
    }
    return stack->items[stack->top--];
}

// Funkce vrací ukazatel na prvek na vrcholu zásobníku. Pokud je zásobník prázdný, vrací NULL.
Token_ptr stack_peek(Stack *stack) {
    if (stack_isEmpty(stack)) {
        return NULL;
    }
    return stack->items[stack->top];
}

/*** Konec souboru stack.c ***/