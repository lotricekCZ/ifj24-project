#include "memory_table.h"
#include "hashtable.c"
#include "sll.c"

SLL(memory, memory, void *, nothing, memt_deinit, false, false)
HASHTABLE(MEMORY_TABLE_SIZE, memory, void *, void *, memt_hash, memt_key, memt_copy, memt_comp, memt_deinit)

size_t memt_hash(void *src)
{
	return (int)src % MEMORY_TABLE_SIZE;
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

bool memt_comp(void *src, void *other)
{
	return src == other;
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