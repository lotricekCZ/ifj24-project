/**
 * @addtogroup IFJ2024
 * @file memory_table.c
 * @brief Implementace paměťové tabulky
 * @author xramas01; Jakub Ramaseuski
 * 
 * Implementace funkcí pro práci s paměťovou tabulkou.
 */

#include <stdint.h>
#include "memory_table.h"
#include "hashtable.c"
#include "sll.c"

SLL(memory, memory, void *, nothing, memt_deinit, false, false)

HASHTABLE(MEMORY_TABLE_SIZE, memory, void *, void *, memt_hash, memt_key, memt_copy, memt_comp, memt_deinit)

// Získání ukazatele na prvek v seznamu na daném indexu.
size_t memt_hash(void *src)
{
	return (uintptr_t)src % MEMORY_TABLE_SIZE;
}

// Získání klíče pro porovnání.
void *memt_key(void *src)
{
	return src;
}

// Kopírování prvku.
void *memt_copy(void *src)
{
	// void **copy = malloc(sizeof(void *));
	// *copy = src;
	return src;
}

// Uvolnění paměti.
void memt_deinit(void **src)
{
	free(*src);
	free(src);
}

// Bezpečné uvolnění paměti.
void memt_safe_deinit(void **src)
{
	// free(*src);
	free(src);
}

// Porovnání dvou prvků.
size_t memt_comp(void *src, void *other)
{
	return src == other;
}

// Bezpečné uvolnění paměti jednosměrně vázaného seznamu.
void memory_sll_safe_delete(memory_sllist *sll, size_t index)
{
	if (index == 0)
	{
		memory_sll_element_ptr temp = sll->firstElement;
		sll->firstElement = temp->next;
        if(temp == sll->activeElement) sll->activeElement = NULL;
		if (temp != NULL && temp->ptr != NULL)
		{
			memt_safe_deinit(temp->ptr);
			free(temp);

			sll->currentLength--;
		}
		return;
	}
	memory_sll_element_ptr temp = memory_sll_at(sll, index - 1);
	memory_sll_element_ptr target = NULL;
	if (temp != NULL && temp->next != NULL)
	{
		target = temp->next;
		temp->next = temp->next->next;
	}
	if (target != NULL)
	{
		memt_safe_deinit(target->ptr);
		free(target);
		sll->currentLength--;
	}
}

// Bezpečné uvolnění paměti hasovací tabulky.
void memory_ht_safe_delete(memory_ht *table, void *key)
{
	size_t hash = memt_hash(key);
	memory_sllist *row = &((table->table)[hash]);
	memory_sll_first(row);
	size_t index = 0;
	memory_sll_element_ptr item = memory_sll_front(row);
	while (memory_sll_is_active(row))
	{
		if (memt_comp(*item->ptr, key))
		{
			memory_sll_safe_delete(row, index);
			table->size--;
			return;
		}
		item = memory_sll_next(row);
		index++;
	}
}

// Alokace pameti s moznosti sledovani.
void *imalloc(size_t size)
{
	void *ptr = malloc(size);
	if (safe_memory)
		memory_ht_insert(&_memory_table, ptr);
	return ptr;
}

// Uvolni pamet alokovanou pomoci imalloc.
void ifree(void *ptr)
{
	if (safe_memory)
		memory_ht_delete(&_memory_table, ptr);
	else
		free(ptr);
}

// Realokace pameti s moznosti sledovani.
void *irealloc(void *ptr, size_t size)
{
	void *newptr = realloc(ptr, size);
	if (newptr == NULL && size != 0)
	{
		memory_ht_dispose(&_memory_table);
		exit(99);
	}
	if (safe_memory && newptr != ptr)
	{
		memory_ht_insert(&_memory_table, newptr);
		memory_ht_safe_delete(&_memory_table, ptr);
	}
	return newptr;
}

/*** Konec souboru memory_table.c ***/