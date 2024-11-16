#include <stdbool.h>
#include "../../src/utils/memory_table.h"
#include "../../src/utils/token.h"
int main()
{
	save_memory = true;
	memory_ht_init(&_memory_table);
	for (int i = 1; i <= 6000; i++)
	{
		void *ptr = imalloc(sizeof(int));
		*(int *)(ptr) = i;
		continue;
		memory_ht_insert(&_memory_table, ptr);
	}
	Token_ptr token = tok_init(tok_t_sym);
	tok_set_attribute(token, "test");
	// tok_print(token);
	int *ptr = imalloc(sizeof(double));
	*ptr = 42;
	ifree(ptr);
	memory_ht_dispose(&_memory_table);
	return 0;
}