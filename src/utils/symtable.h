/** 
 * Projekt IFJ2024
 * 
 * @brief Rozhraní symbolické tabulky
 * 
 * @author xsidlil00; Lukáš Šidlík
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "dynamic_array.h"
#include "token_types.h"
#include "errors.h"

#define SYMTABLE_SIZE 4001  // velikost symbol table

/**
 * @enum Datové typy
 */
typedef enum
{
    DATA_TYPE_INT,     // integer
    DATA_TYPE_DOUBLE,  // double
    DATA_TYPE_U8,      // unsigned char
    DATA_TYPE_STRING,  // string
    DATA_TYPE_BOOLEAN, // boolean
    DATA_TYPE_VOID,    // void
    DATA_TYPE_UND      // undefined
} data_type_t;

/**
 * @struct Rozhraní dat v položce symbolické tabulky
 */
typedef struct data_t
{
    data_type_t type;               // datový typ dat
    dynamic_array_t *parameters;    // parametry funkce
    char *id;                       // identifikátor
    bool as_func;                   // funkce @as
    bool canNull;                   // nullability
    bool isConst;                   // konstanta
    bool used;                      // použití
    bool modified;                  // modifikace
} data_t;

/**
 * @struct Položka symbolické tabulky
 */
typedef struct symtable_item_t
{
    char *key;                    // klíč
    data_t data;                  // data
    struct symtable_item_t *next; // ukazatel na dalsí polozku
} symtable_item_t;

/**
 * @brief Symbolická tabulka
 */
typedef symtable_item_t *symtable_t[SYMTABLE_SIZE];

/**
 * @brief Inicializuje symbolické tabulky.
 *
 * Inicializuje symbol table tak, že nastaví všechny prvky pole na NULL.
 *
 * @param symtable Symbol table, které chceme inicializovat
 * @param error místo, kam se uloží chybový kód v případě chyby
 */
void symtable_init(symtable_t *symtable, err_codes *error);

/**
 * @brief Najde a vráti ukazatel na položku v symbolické tabulce s daným jménem.
 *
 * @param symtable tabulka symbolů, ve které chceme hledat
 * @param name jméno, které chceme najít
 * @param error místo, kam se uloží chybový kód v případě chyby
 * @return ukazatel na položku, nebo NULL, pokud nenalezen
 */
data_t *symtable_get_item(symtable_t *table, char *name, err_codes *error);

/**
 * @brief Vloží novou položku do tabulky symbolů.
 *
 * Pokud je položka s daným jménem již v symbolické tabulce, pak se vrací chyba err_redef.
 * Pokud dojde k vnitřní chybě, pak se vrací chyba err_internal.
 *
 * @param symtable tabulka symbolů do které se má vložit prvek
 * @param name jméno položky, která se má vložit
 * @param error místo, kam se uloží chybový kód v případě chyby
 * @return vložený item, nebo NULL v případě chyby
 */
data_t *symtable_insert(symtable_t *symtable, char *name, err_codes *error);

/**
 * @brief Přidá parametr do seznamu parametrů daného prvku v tabulce symbolů.
 *
 * @param data Ukazatel na prvky v tabulce symbolů, do něhož má být parametr přidán.
 * @param type Typ datového typu, který má být přidán.
 * @param error místo, kam se uloží chybový kód v případě chyby
 */
void symtable_insert_params(data_t *data, token_type type, err_codes *error);

/**
 * @brief Zruší tabulku symbolů a dealokuje všechna její položky.
 *
 * @param symtable tabulka symbolů, kterou chceme zničit
 * @param error místo, kam se uloží chybový kód v případě chyby
 * @param isFirst příznak, zda se jedná o tabulku funkcí (ovlivňuje kontrolu nepoužitých proměnných)
 */
void symtable_destroy(symtable_t *symtable, err_codes *error, bool isFirst);

/**
 * @brief Vloží vestavěné funkce do symbolické tabulky.
 *
 * Vloží vestavěné funkce do symtable. Pokud dojde k chybě, je
 * chybový kód uložen do error.
 *
 * @param symtable tabulka symbolů
 * @param error místo, kam se uloží chybový kód v případě chyby
 */
void symtable_insert_builtin(symtable_t *symtable, err_codes *error);

#endif // SYMTABLE_H