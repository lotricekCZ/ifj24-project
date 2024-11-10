/**
 * DLList pro symtable
 * Vytvořil: Lukáš Šidlík (xsidlil00)
 */

#include "symtable.h"
#include "symDLList.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void DLL_Init(DLList *list){
    list->first = NULL;
    list->current = NULL;
    list->last = NULL;
    list->length = 0;
}

void DLL_First(DLList *list){
    list->current = list->first;
}

void DLL_Last(DLList *list){
    list->current = list->last;
}

void DLL_Next(DLList *list){
    if(list->current != NULL)
        list->current = list->current->next;
}

void DLL_Prev(DLList *list){
    if(list->current != NULL)
        list->current = list->current->prev;
}


bool DLL_isActive(DLList *list){
    return list->current != NULL;
}

bool DLL_Destroy(DLList *list, int *err){
    list->current = NULL;
    while (list->first != NULL)
    {
        DLLElementPtr deleteElement = list->first;
        list->first = deleteElement->next;
        if(!symtable_destroy((symtable_t *)deleteElement->symtable, err)){
            //chyba
            return false;
        }
        free(deleteElement);
        list->length--;
    }
    return true;
}

symtable_t * DLL_GetFirst(DLList *list){
    if(list->first != NULL){
        return (symtable_t *)list->first->symtable;   
    }
    return NULL;
}

symtable_t * DLL_GetCurrent(DLList *list){
    if(list->current != NULL){
        return (symtable_t *)list->current->symtable;   
    }
    return NULL;
}

symtable_t * DLL_GetLast(DLList *list){
    if(list->last != NULL){
        return (symtable_t *)list->last->symtable; 
    }
    return NULL;
}

bool DLL_Delete_last(DLList *list, int *err){
    DLLElementPtr delete;
    if(list->last != NULL){
        delete = list->last;
        if(list->current == list->last)
            list->current = NULL;
        if(list->first == list->last){
            list->first = NULL;
            list->last = NULL;
        }
        else{
            list->last = list->last->prev;
            list->last->next = NULL;
        }
        if(!symtable_destroy((symtable_t *)delete->symtable, err)){
            //chyba
            return false;  
        }
        free(delete);
        list->length--;
         
    }
    return true; 
}

bool DLL_Insert_last(DLList *list, symtable_t *symtable, int *err){
    DLLElementPtr new = (DLLElementPtr)malloc(sizeof(struct DLLElement));
    if(new == NULL){
        //chyba
        return false;
    }
    new->symtable = (struct symtable_t *)symtable;
    new->next = NULL;
    new->prev = list->last;
    if(list->last != NULL)
        list->last->next = new;
    else
        list->first = new;

    list->last = new;
    list->length++;

    return true;
}

void DLL_Set(DLList *list, symtable_t *symtable){
    if(list->current != NULL)
        list->current->symtable = (struct symtable_t *)symtable;
}

void DLL_Set_First(DLList *list, symtable_t *symtable){
    if(list->first != NULL)
        list->first->symtable = (struct symtable_t *)symtable;
}

void DLL_Set_Last(DLList *list, symtable_t *symtable){
    if(list->last != NULL)
        list->last->symtable = (struct symtable_t *)symtable;
}
