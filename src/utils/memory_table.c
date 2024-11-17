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

void *imalloc(size_t size)
{
	void *ptr = malloc(size);
	if (safe_memory)
		memory_ht_insert(&_memory_table, ptr);
	return ptr;
}
void ifree(void *ptr)
{
	if (safe_memory)
		memory_ht_delete(&_memory_table, ptr);
	else
		free(ptr);
}