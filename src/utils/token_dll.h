/**
 * @addtogroup IFJ2024
 * @file token_dll.h
 * @brief Knihovna pro dvousměrně vázaný seznam tokenů
 * @author xramas01
 * 
 * Knihovna obsahuje definice struktur a funkcí pro dvousměrně vázaný seznam tokenů.
 */

#ifndef TOKEN_DLL_H
#define TOKEN_DLL_H

#include <stdlib.h>
#include <stdio.h>
#include "token.h"
#include "dll.c"

/**
 * @brief Definice makra pro dvousměrně vázaný seznam
 */
_DLL(token, tok, Token)

#endif /* TOKEN_DLL_H */

/*** Konec souboru token_dll.h ***/