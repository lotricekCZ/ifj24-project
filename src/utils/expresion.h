/** 
 * Projekt IFJ2024
 * 
 * @brief Rozhraní sématické kontroly matematických výrazů
 * 
 * @author xsidlil00; Lukáš Šidlík
 */

#ifndef EXPRESION_H
#define EXPRESION_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "token_types.h"
#include "token.h"
#include "stack.h"
#include "symtable.h"
#include "symDLList.h"
#include "errors.h"

/**
 * @brief Funkce pro výpočet postfixového výrazu s kontrolou datových typů.
 * @param postfix pole s postfixovým vyjádřením výrazu
 * @param postfix_index počet prvků v poli postfix
 * @param sym_list dvousmerný seznam tabulek symbolů
 * @param symtable aktuální symbolická tabulka
 * @param error místo, kam se uloží chybový kód v případě chyby
 * @return struktura s datovým typem výsledku
 */
data_t * postfix_semantic(Token_ptr *postfix, int postfix_index, DLList sym_list, symtable_t *symtable, err_codes *error);

#endif // EXPRESION_H