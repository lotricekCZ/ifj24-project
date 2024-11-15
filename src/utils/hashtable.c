#include "hashtable.h"
#include "sll.h"
#include <stdlib.h>
#include <string.h>

#define HT_INIT(entries, prefix, element, key_type)      \
	void prefix##_ht_init(prefix##_ht *table)            \
	{                                                    \
		prefix##_sllist *template = prefix##_sll_init(); \
		for (int i = 0; i < entries; i++)                \
		{                                                \
			((table->table)[i]) = *template;             \
		}                                                \
		free(template);                                  \
	}

/**
 * @brief Search for an element in the hash table.
 *
 * @param table The hash table.
 * @param key The key of the element to search for.
 * @return A pointer to the element if found, NULL otherwise.
 */
#define HT_SEARCH(entries, prefix, element, key_type, hash_fn, comp_fn)            \
	prefix##_sll_element_ptr prefix##_ht_search(prefix##_ht *table, key_type key) \
	{                                                                              \
		size_t hash = hash_fn(key);                                                \
		prefix##_sllist *row = &(table->table)[hash];                              \
		prefix##_sll_first(row);                                                   \
		prefix##_sll_element_ptr item = prefix##_sll_front(row);                   \
		while (prefix##_sll_is_active(row))                                        \
		{                                                                          \
			if (comp_fn == nothing_size)                                           \
			{                                                                      \
				if (*item->ptr == key)                                             \
					return item;                                                   \
			}                                                                      \
			else                                                                   \
			{                                                                      \
				if (comp_fn(*item->ptr, key))                                      \
				{                                                                  \
					return item;                                                   \
				}                                                                  \
			}                                                                      \
			item = prefix##_sll_next(row);                                         \
		}                                                                          \
		return NULL;                                                               \
	}

/**
 * @brief Insert an element into the hashtable.
 *
 * @param table The hashtable to insert into.
 * @param data The element to insert.
 */
#define HT_INSERT(entries, prefix, element, key_type, hash_fn, key_fn, copy_fn, deinit_fn) \
	void prefix##_ht_insert(prefix##_ht *table, element data)                              \
	{                                                                                      \
		prefix##_sll_element_ptr item = prefix##_ht_search(table, key_fn(data));           \
		if (item == NULL)                                                                  \
		{                                                                                  \
			size_t hash = hash_fn(key_fn(data));                                           \
			prefix##_sll_push_front(&(table->table)[hash], data);                          \
			table->size++;                                                                 \
		}                                                                                  \
		else                                                                               \
		{                                                                                  \
			deinit_fn(item->ptr);                                                          \
			item->ptr = copy_fn(data);                                                     \
		}                                                                                  \
	}

#define HT_GET(entries, prefix, element, key_type)                      \
	element *prefix##_ht_get(prefix##_ht *table, key_type key)          \
	{                                                                   \
		prefix##_sll_element_ptr item = prefix##_ht_search(table, key); \
		if (item != NULL)                                               \
		{                                                               \
			return item->ptr;                                           \
		}                                                               \
		return NULL;                                                    \
	}

#define HT_DELETE(entries, prefix, element, key_type, comp_fn, hash_fn, deinit_fn) \
	void prefix##_ht_delete(prefix##_ht *table, key_type key)                      \
	{                                                                              \
		size_t hash = hash_fn(key);                                                \
		prefix##_sllist *row = &((table->table)[hash]);                            \
		prefix##_sll_first(row);                                                   \
		size_t index = 0;                                                          \
		prefix##_sll_element_ptr item = prefix##_sll_front(row);                   \
		while (prefix##_sll_is_active(row))                                        \
		{                                                                          \
			if (comp_fn == nothing_size)                                           \
			{                                                                      \
				if (*item->ptr == key)                                             \
				{                                                                  \
					prefix##_sll_delete(row, index);                               \
					table->size--;                                                 \
					return;                                                        \
				}                                                                  \
			}                                                                      \
			else                                                                   \
			{                                                                      \
				if (comp_fn(item->ptr, key))                                       \
				{                                                                  \
					prefix##_sll_delete(row, index);                               \
					table->size--;                                                 \
					return;                                                        \
				}                                                                  \
			}                                                                      \
			item = prefix##_sll_next(row);                                         \
			index++;                                                               \
		}                                                                          \
	}
#define HT_DISPOSE(entries, prefix, element, key_type, deinit_fn) \
	void prefix##_ht_dispose(prefix##_ht *table)                  \
	{                                                             \
		for (size_t i = 0; i < entries; i++)                      \
		{                                                         \
			prefix##_sll_dispose(&(table->table)[i]);             \
		}                                                         \
	}

#define HASHTABLE(entries, prefix, element, key_type, hash_fn, key_fn, copy_fn, comp_fn, deinit_fn) \
	HT_INIT(entries, prefix, element, key_type)                                                     \
	HT_SEARCH(entries, prefix, element, key_type, hash_fn, comp_fn)                                 \
	HT_INSERT(entries, prefix, element, key_type, hash_fn, key_fn, copy_fn, deinit_fn)              \
	HT_GET(entries, prefix, element, key_type)                                                      \
	HT_DELETE(entries, prefix, element, key_type, comp_fn, hash_fn, deinit_fn)                      \
	HT_DISPOSE(entries, prefix, element, key_type, deinit_fn)
