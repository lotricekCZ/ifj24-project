/** 
 * @addtogroup IFJ2024
 * @file symDLList.c
 * @brief Implementace dvousměrně vázaného seznamu
 * @author xsidlil00; Lukáš Šidlík
 * 
 * Implementace funkcí pro práci s dvousměrně vázaným seznamem.
 */

#include "symDLList.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "errors.h"
#include "memory_table.h"

/**
 * @brief Inicializuje seznam, tzn. nastaví všechny jeho ukazatele na NULL.
 *
 * @param list Ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Init(DLList *list) {
    list->first = NULL;
    list->current = NULL;
    list->last = NULL;
}

/**
 * @brief Nastaví aktuální prvek na začátek seznamu.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_First(DLList *list) {
    list->current = list->first;
}

/**
 * @brief Nastaví poslední prvek seznamu list jako aktivní.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Last(DLList *list) {
    list->current = list->last;
}

/**
 * @brief Posune aktivitu na následující prvek seznamu list.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Next(DLList *list) {
    if (list->current != NULL) {
        list->current = list->current->next;
    }
}

/**
 * @brief Posune aktivitu na předchozí prvek seznamu list.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Prev(DLList *list) {
    if (list->current != NULL) {
        list->current = list->current->prev;
    }
}

/**
 * @brief Funkce kontroluje, zda je seznam aktivní.
 * 
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 * @returns vrací true, pokud je aktuální prvek seznamu nenulový, jinak vrací false
 */
bool DLL_isActive(DLList *list) {
    return list->current != NULL;
}

/**
 * @brief Vloží nový prvek na konec seznamu.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 * @param error místo, kam se uloží chybový kód v případě chyby
 * @return v případě úspěchu ukazatel na nově vloženou symbolickou tabulku, jinak NULL
 */
symtable_t * DLL_Insert_last(DLList *list, err_codes *error) {
    DLLElementPtr new = (DLLElementPtr)imalloc(sizeof(struct DLLElement));
    if (new == NULL) {
        *error = err_internal;
        return NULL;
    }

    new->symtable  = (symtable_t *)imalloc(sizeof(symtable_t));
    if (new->symtable == NULL) {
        *error = err_internal;
        ifree(new);
        return NULL;
    }

    symtable_init(new->symtable, error);
    if (*error != err_none) 
        return NULL;
    
    new->next = NULL;
    new->prev = list->last;
    if (list->last != NULL) {
        list->last->next = new;
    } 
    else {
        list->first = new;
    }
    list->last = new;
    
    return (symtable_t *)new->symtable;
}

/**
 * @brief Vrátí první prvek seznamu, nebo NULL, pokud je seznam prázdný.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 * @return prvek ze seznamu, nebo NULL
 */
symtable_t *DLL_GetFirst(DLList *list) {
    if (list->first != NULL) {
        return (symtable_t *)list->first->symtable;
    }
    return NULL;
}

/**
 * @brief Vrátí poslední prvek seznamu, nebo NULL, pokud je seznam prázdný.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 * @return poslední prvek ze seznamu, nebo NULL
 */
symtable_t *DLL_GetLast(DLList *list) {
    if (list->last != NULL) {
        return (symtable_t *)list->last->symtable;
    }
    return NULL;
}

/**
 * @brief Vrátí aktuální prvek ze seznamu, nebo NULL, pokud seznam nemá žádný aktivní prvek.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 * @return aktuální prvek ze seznamu, nebo NULL
 */
symtable_t *DLL_GetCurrent(DLList *list) {
    if (list->current != NULL) {
        return (symtable_t *)list->current->symtable;
    }
    return NULL;
}

/**
 * @brief Zruší poslední prvek seznamu list.
 * Pokud byl poslední prvek aktivní, aktivita seznamu se ztrácí.
 * Pokud byl seznam list prázdný, nic se neděje.
 *
 * @param list ukazatel na strukturu dvousměrně vázaného seznamu
 */
void DLL_Delete_last(DLList *list, err_codes *error) {
    bool isFirst = list->first == list->last;
    DLLElementPtr delete;
    if (list->last != NULL) {
        delete = list->last;
        if (list->current == list->last) {
            list->current = NULL;
        }

        if (list->first == list->last) {
            list->first = NULL;
            list->last = NULL;
        } 
        else {
            list->last = list->last->prev;
            list->last->next = NULL;
        }

        symtable_destroy((symtable_t *)delete->symtable, error, isFirst);

        if (*error != err_none) 
            return;

        ifree(delete);
    }
}

/**
 * @brief Zruší všechny prvky seznamu list a uvede seznam do stavu, v jakém se nacházel po inicializaci.
 * 
 * Rušené prvky seznamu budou korektně uvolněny voláním operace free a symtable_destroy.
 *
 * @param list ukazatel na inicializovanou strukturu dvousměrně vázaného seznamu
 * @param error místo, kam se uloží chybový kód v případě chyby
 */
void DLL_Destroy(DLList *list, err_codes *error) {
    while (list->first != NULL) {
        DLLElementPtr deleteElement = list->first;
        bool isFirst = list->first == list->last;
        list->first = deleteElement->next;
        symtable_destroy((symtable_t *)deleteElement->symtable, error, isFirst);
        if (*error != err_none) 
            return;
        
        ifree(deleteElement);
    }
    return;
}
