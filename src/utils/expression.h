/** 
 * @addtogroup IFJ2024
 * @file expression.h
 * @brief Rozhraní sématické kontroly matematických výrazů
 * @author xsidlil00; Lukáš Šidlík
 * 
 * Rozhraní sématické kontroly matematických výrazů.
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
 * @brief Hledá položku v symbolické tabulce s daným jménem.
 * 
 * @param result_data nalezená položka symtable
 * @param sym_list dvousmerný seznam symtable
 * @param popToken token s jménem proměnné
 * @param symtable aktuální symtable
 * @param error místo, kam se uloží chybový kód v případě chyby
 * @return nalezená položka symtable, NULL pokud nebylo nalezeno
 */
data_t *find(data_t *result_data, DLList sym_list, Token_ptr popToken, symtable_t *symtable, err_codes *error);

/**
 * @brief Vrací chybový kód pro neslučitelné datové typy.
 *
 * @return chybový kód err_dt_invalid
 */
err_codes error_found();

/**
 * @brief Kontroluje, zda datový typ v result_data odpovídá očekávanému typu.
 *
 * @param result_data ukazatel na strukturu obsahující datový typ, který má být zkontrolován
 * @param type očekávaný datový typ
 * @param error místo, kam se uloží chybový kód v případě chyby
 */
void check_type(data_t *result_data, data_type_t type, err_codes *error);

/**
 * @brief Kontroluje, zda datový typ v result_data odpovídá jednomu z očekávaných typů.
 *
 * @param result_data ukazatel na strukturu obsahující datový typ, který má být zkontrolován
 * @param type první očekávaný datový typ
 * @param type2 druhý očekávaný datový typ
 * @param error místo, kam se uloží chybový kód v případě chyby
 */
void check_two_types(data_t *result_data, data_type_t type, data_type_t type2, err_codes *error);

/**
 * @brief Zkontroluje, zda datový typ v result_data nemůže být NULL.
 *
 * @param result_data ukazatel na strukturu obsahující datový typ, který má být zkontrolován
 * @param error místo, kam se uloží chybový kód v případě chyby
 */
void check_not_canNull(data_t *result_data, err_codes *error);

/**
 * @brief Kontroluje, zda datový typ v result_data může být NULL.
 * 
 * @param result_data ukazatel na strukturu obsahující datový typ, který má být zkontrolován
 * @param error místo, kam se uloží chybový kód v případě chyby
 */
void check_canNull(data_t *result_data, err_codes *error);

/**
 * @brief Zkontroluje, zda datový typ v result_data je vhodný pro aritmetické operace.
 *
 * @param result_data ukazatel na strukturu obsahující datový typ, který má být zkontrolován
 * @param error místo, kam se uloží chybový kód v případě chyby
 */
void check_operator_calc(data_t *result_data, err_codes *error);

/**
 * @brief Kontroluje, zda datový typ v result_data a result_data2 je shodný.
 *
 * @param result_data ukazatel na strukturu obsahující datový typ, který má být zkontrolován
 * @param result_data2 ukazatel na strukturu obsahující datový typ, který má být zkontrolován
 * @param error místo, kam se uloží chybový kód v případě chyby
 */
void check_second_symbol(data_t *result_data, data_t *result_data2, err_codes *error);

/**
 * @brief Kontroluje, zda datový typ v result_data je vhodný pro porovnávání.
 *
 * @param result_data ukazatel na strukturu obsahující datový typ, který má být zkontrolován
 * @param result ukazatel na strukturu, kam se má uložit výsledný datový typ
 * @param error místo, kam se uloží chybový kód v případě chyby
 */
void check_operator_bool(data_t *result_data, data_t *result, err_codes *error);

/**
 * @brief Najde prvni symbol v DLList sym_list, kontroluje, zda je typu boolean.
 *
 * @param result_data ukazatel na strukturu obsahující datový typ, který má být zkontrolován
 * @param popToken token, který má být zkontrolován
 * @param sym_list dvousmerný seznam tabulek symbolů
 * @param symtable aktuální symbolická tabulka
 * @param error místo, kam se uloží chybový kód v případě chyby
 * @return ukazatel na strukturu obsahující datový typ, byl zkontrolován nebo NULL v případě chyby
 */
data_t *check_bool_first(data_t *result_data, Token_ptr popToken, DLList sym_list, symtable_t *symtable, err_codes *error);

/**
 * @brief Kontroluje, zda datový typ v result_data je vhodný pro bool operace.
 *
 * @param result_data ukazatel na strukturu obsahující datový typ, který má být zkontrolován
 * @param popToken token, který má být zkontrolován
 * @param sym_list dvousmerný seznam tabulek symbolů
 * @param symtable aktuální symbolická tabulka
 * @param error místo, kam se uloží chybový kód v případě chyby
 */
void check_bool_second(data_t *result_data, Token_ptr popToken, DLList sym_list, symtable_t *symtable, err_codes *error);

/**
 * @brief Kontroluje, zda datový typ v result_data a result_data2 je vhodný pro bool operace.
 *
 * @param result_data ukazatel na strukturu obsahující datový typ, který má být zkontrolován
 * @param result_data2 druhý ukazatel na strukturu obsahující datový typ, který má být zkontrolován
 * @param popToken token, který má být zkontrolován
 * @param popToken2 token, který má být zkontrolován
 * @param sym_list dvousmerný seznam tabulek symbolů
 * @param symtable aktuální symbolická tabulka
 * @param result ukazatel na strukturu obsahující datový typ výsledku
 * @param error místo, kam se uloží chybový kód v případě chyby
 */
void check_bool(data_t *result_data, data_t *result_data2, Token_ptr popToken, Token_ptr popToken2, DLList sym_list, symtable_t *symtable, data_t *result, err_codes *error);

/**
 * @brief Kontroluje, zda datový typ v result_data je vhodný pro AND a OR operace.
 *
 * @param popToken token, který má být zkontrolován
 * @param popToken2 druhý token, který má být zkontrolován
 * @param result_data ukazatel na strukturu obsahující datový typ, který má být zkontrolován
 * @param sym_list dvousmerný seznam tabulek symbolů
 * @param symtable aktuální symbolická tabulka
 * @param error místo, kam se uloží chybový kód v případě chyby
 * @return popToken, pokud je vše v pořádku, jinak NULL
 */
Token_ptr check_and_or(Token_ptr popToken, Token_ptr popToken2, data_t *result_data, DLList sym_list, symtable_t *symtable, err_codes *error);

/**
 * @brief Najde prvni symbol v DLList sym_list, kontroluje, jestli může nabývat hodnoty NULL.
 *
 * @param popToken token, který má být zkontrolován
 * @param result_data ukazatel na strukturu obsahující datový typ, který má být zkontrolován
 * @param sym_list dvousmerný seznam tabulek symbolů
 * @param symtable aktuální symbolická tabulka
 * @param error místo, kam se uloží chybový kód v případě chyby
 * @return ukazatel na strukturu obsahující datový typ, byl zkontrolován nebo NULL v případě chyby
 */
data_t *check_first_orelse(Token_ptr popToken, data_t *result_data, DLList sym_list, symtable_t *symtable, err_codes *error);

/**
 * @brief Kontroluje druhý symbol v DLList sym_list a ověřuje datové typy.
 *
 * @param result_data ukazatel na strukturu obsahující datový typ prvního symbolu
 * @param result_data2 ukazatel na strukturu obsahující datový typ druhého symbolu
 * @param popToken2 token pro druhý symbol
 * @param sym_list dvousměrný seznam tabulek symbolů
 * @param symtable aktuální symbolická tabulka
 * @param error místo, kam se uloží chybový kód v případě chyby
 * @return ukazatel na strukturu obsahující datový typ druhého symbolu, nebo NULL v případě chyby
 */
data_t *check_second_orelse(data_t *result_data, data_t *result_data2, Token_ptr popToken2, DLList sym_list, symtable_t *symtable, err_codes *error);

/**
 * @brief Funkce, která nahradí typ tokenu.
 *
 * @param popToken token, který má být nahrazen
 * @param result datový typ, podle kterého má být nahrazen
 * @param error místo, kam se uloží chybový kód v případě chyby
 * @return nahrazený token, nebo NULL v případě chyby
 */
Token_ptr unreachble_type(Token_ptr popToken, data_t *result, err_codes *error);

/**
 * @brief Nastaví typ dat v result podle tokenu popToken.
 *
 * @param result struktura, kam má být typ dat uložen
 * @param popToken token, podle kterého má být nastaven typ dat
 * @param symtable aktuální symbolická tabulka
 * @param sym_list dvousmerný seznam tabulek symbolů
 * @param error místo, kam se uloží chybový kód v případě chyby
 * @return ukazatel na strukturu s nastaveným typem dat, nebo NULL v případě chyby
 */
data_t* resultType(data_t *result, Token_ptr popToken, symtable_t *symtable, DLList sym_list, err_codes *error);

/**
 * @brief Funkce pro výpočet postfixového výrazu s kontrolou datových typů.
 * 
 * @param postfix pole s postfixovým vyjádřením výrazu
 * @param postfix_index počet prvků v poli postfix
 * @param sym_list dvousmerný seznam tabulek symbolů
 * @param symtable aktuální symbolická tabulka
 * @param error místo, kam se uloží chybový kód v případě chyby
 * @return struktura s datovým typem výsledku
 */
data_t * postfix_semantic(Token_ptr *postfix, int postfix_index, DLList sym_list, symtable_t *symtable, err_codes *error);

#endif /* EXPRESION_H */

/*** Konec souboru expression.h ***/