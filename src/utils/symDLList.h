/**
 * @addtogroup IFJ2024
 * @file symDLList.h
 * @brief Rozhraní dvousměrně vázaného seznamu
 * @author xsidlil00; Lukáš Šidlík
 * 
 * Rozhraní funkcí pro práci s dvousměrně vázaným seznamem.
 */

#ifndef SYMDLLIST_H
#define SYMDLLIST_H

#include "symtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "errors.h"

/**
 * @struct prvek dll seznamu
 * @brief Struktura prvků dvousměrně vázaného seznamu
 */
typedef struct DLLElement{
    symtable_t *symtable;               /**< symbolická tabulka */
    struct DLLElement *next;            /**< ukazatel na následujíci prvek */
    struct DLLElement *prev;            /**< ukazatel na predchozí prvek */
}*DLLElementPtr;

/**
 * @struct dll seznam
 * @brief Struktura dvousměrně vázaného seznamu
 */
typedef struct DLList{
    DLLElementPtr first;                /**< ukazatel na prvni prvek */
    DLLElementPtr current;              /**< ukazatel na aktivní prvek */
    DLLElementPtr last;                 /**< ukazatel na poslední prvek */
}DLList;

/**
 * @brief Inicializuje seznam, tzn. nastaví všechny jeho ukazatele na NULL.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init(DLList *list);

/**
 * @brief Nastaví aktuální prvek na začátek seznamu.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_First(DLList *list);

/**
 * @brief Nastaví poslední prvek seznamu list jako aktivní.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Last(DLList *list);

/**
 * @brief Posune aktivitu na následující prvek seznamu list.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Next(DLList *list);

/**
 * @brief Posune aktivitu na předchozí prvek seznamu list.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Prev(DLList *list);

/**
 * @brief Funkce kontroluje, zda je seznam aktivní.
 * 
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 * @returns vrací true, pokud je aktuální prvek seznamu nenulový, jinak vrací false
 */
bool DLL_isActive(DLList *list);

/**
 * @brief Vloží nový prvek na konec seznamu.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 * @param error místo, kam se uloží chybový kód v případě chyby
 * @return v případě úspěchu ukazatel na nově vloženou symbolickou tabulku, jinak NULL
 */
symtable_t * DLL_Insert_last(DLList *list, err_codes *error);

/**
 * @brief Vrátí první prvek seznamu, nebo NULL, pokud je seznam prázdný.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 * @return prvek ze seznamu, nebo NULL
 */
symtable_t * DLL_GetFirst(DLList *list);

/**
 * @brief Vrátí poslední prvek seznamu, nebo NULL, pokud je seznam prázdný.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 * @return poslední prvek ze seznamu, nebo NULL
 */
symtable_t * DLL_GetLast(DLList *list);

/**
 * @brief Vrátí aktuální prvek ze seznamu, nebo NULL, pokud seznam nemá žádný aktivní prvek.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 * @return aktuální prvek ze seznamu, nebo NULL
 */
symtable_t * DLL_GetCurrent(DLList *list);

/**
 * @brief Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Delete_last(DLList *list, err_codes *error);

/**
 * @brief Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel po inicializaci.
 * 
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free a symtable_destroy.
 *
 * @param list ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param error místo, kam se uloží chybový kód v případě chyby
 */
void DLL_Destroy(DLList *list, err_codes *error);

#endif //SYMDLLIST_H

/*** Konec souboru symDLList.h ***/