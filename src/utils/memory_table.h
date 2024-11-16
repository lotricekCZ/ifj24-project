#include "hashtable.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef MEMORY_TABLE_H
#define MEMORY_TABLE_H

#define MEMORY_TABLE_SIZE 9973
size_t memt_hash(void *src);
void *memt_key(void *src);
void *memt_copy(void *src);
void memt_deinit(void **src);
bool memt_comp(void *src, void *other);
void memt_deinit(void **src);
void *imalloc(size_t size);
void ifree(void *ptr);
_HASHTABLE(MEMORY_TABLE_SIZE, memory, void *, void *)

memory_ht _memory_table;
bool save_memory = false; 
#endif