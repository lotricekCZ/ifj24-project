/**
 * @addtogroup IFJ2024
 * @file token_dll.c
 * @brief Implementace dvousměrně vázaného seznamu tokenů
 * @author xramas01; Jakub Ramaseuski
 * 
 * Implementace funkcí pro práci s dvousměrně vázaným seznamem tokenů.
 */

#include "token_dll.h"

/**
 * @brief Vytvoření dvojsměrně vázaného seznamu
 * 
 * Vytvoření funkcí dvojsměrně vázaného seznamu tokenů makrem. 
 */
DLL(token, tok, Token, tok_copy, tok_init_no_ret, tok_free)

/*** Konec souboru token_dll.c ***/