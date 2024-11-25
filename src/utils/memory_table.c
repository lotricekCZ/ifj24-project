/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

#include <stdint.h>
#include "memory_table.h"
#include "hashtable.c"
#include "sll.c"

SLL(memory, memory, void *, nothing, memt_deinit, false, false)

HASHTABLE(MEMORY_TABLE_SIZE, memory, void *, void *, memt_hash, memt_key, memt_copy, memt_comp, memt_deinit)

size_t memt_hash(void *src)
{
	return (uintptr_t)src % MEMORY_TABLE_SIZE;
}

void *memt_key(void *src)
{
	return src;
}

void *memt_copy(void *src)
{
	// void **copy = malloc(sizeof(void *));
	// *copy = src;
	return src;
}
void memt_deinit(void **src)
{
	free(*src);
	free(src);
}

void memt_safe_deinit(void **src)
{
	// free(*src);
	free(src);
}

bool memt_comp(void *src, void *other)
{
	return src == other;
}

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

/**
 * @brief Alokace pameti s moznosti sledovani.
 *
 * Funkce alokuje pamet o velikosti size a vraci ukazatel na ni. Pokud je
 * zapnuta kontrola pameti (safe_memory), tak se pamet take ulozi do tabulky
 * pameti, cimz ji lze pozdeji sledovat a uvolnit.
 *
 * @param size Velikost pameti, ktera se ma alokovat.
 * @return Ukazatel na alokovanou pamet.
 */
void *imalloc(size_t size)
{
	void *ptr = malloc(size);
	if (safe_memory)
		memory_ht_insert(&_memory_table, ptr);
	return ptr;
}
/**
 * @brief Uvolni pamet alokovanou pomoci imalloc.
 *
 * Funkce uvolni pamet, ktera byla alokovana pomoci imalloc.
 * Pokud je zapnuta kontrola pameti, tak se prvne pokusime vymazat
 * prvky z tabulky pameti, cimz ji uvolnime, jinak uvolnime pamet s pomoci free.
 *
 * @param ptr Ukazatel na pamet, ktera se ma uvolnit.
 */
void ifree(void *ptr)
{
	if (safe_memory)
		memory_ht_delete(&_memory_table, ptr);
	else
		free(ptr);
}
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