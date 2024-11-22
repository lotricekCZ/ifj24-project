/**
 * DLList pro symtable
 * Vytvořil: Lukáš Šidlík (xsidlil00)
 */

#include "symDLList.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "errors.h"
#include "memory_table.h"

void DLL_Init(DLList *list) {
    list->first = NULL;
    list->current = NULL;
    list->last = NULL;
    list->length = 0;
}

void DLL_First(DLList *list) {
    list->current = list->first;
}

void DLL_Last(DLList *list) {
    list->current = list->last;
}

void DLL_Next(DLList *list) {
    if (list->current != NULL) {
        list->current = list->current->next;
    }
}

void DLL_Prev(DLList *list) {
    if (list->current != NULL) {
        list->current = list->current->prev;
    }
}

bool DLL_isActive(DLList *list) {
    return list->current != NULL;
}

void DLL_Destroy(DLList *list, err_codes *error) {
    while (list->first != NULL) {
        DLLElementPtr deleteElement = list->first;
        list->first = deleteElement->next;
        symtable_destroy((symtable_t *)deleteElement->symtable, error);
        if (*error != err_none) 
            return;
        
        free(deleteElement);
        list->length--;
    }
    return;
}

symtable_t * DLL_Insert_last(DLList *list, err_codes *error) {
    DLLElementPtr new = (DLLElementPtr)imalloc(sizeof(struct DLLElement));
    if (new == NULL) {
        *error = err_internal;
        return NULL;
    }

    new->symtable  = (symtable_t *)imalloc(sizeof(symtable_t));
    if (new->symtable == NULL) {
        *error = err_internal;
        free(new);
        return NULL;
    }

    symtable_init(new->symtable, error);
    if (*error != err_none) 
        return NULL;
    
    new->next = NULL;
    new->prev = list->last;
    if (list->last != NULL) {
        list->last->next = new;
    } else {
        list->first = new;
    }
    list->last = new;
    list->length++;
    
    return (symtable_t *)new->symtable;
}

symtable_t *DLL_GetFirst(DLList *list) {
    if (list->first != NULL) {
        return (symtable_t *)list->first->symtable;
    }
    return NULL;
}

symtable_t *DLL_GetLast(DLList *list) {
    if (list->last != NULL) {
        return (symtable_t *)list->last->symtable;
    }
    return NULL;
}

symtable_t *DLL_GetCurrent(DLList *list) {
    if (list->current != NULL) {
        return (symtable_t *)list->current->symtable;
    }
    return NULL;
}

void DLL_Delete_last(DLList *list, err_codes *error) {
    DLLElementPtr delete;
    if (list->last != NULL) {
        delete = list->last;
        if (list->current == list->last) {
            list->current = NULL;
        }
        if (list->first == list->last) {
            list->first = NULL;
            list->last = NULL;
        } else {
            list->last = list->last->prev;
            list->last->next = NULL;
        }
        symtable_destroy((symtable_t *)delete->symtable, error);
        if (*error != err_none) 
            return;

        free(delete);
        list->length--;
    }
}
