
#include "../../src/utils/hashtable.c"
#include "../../src/utils/sll.c"

size_t getHash(int src)
{
    return src % 11;
}

int key(int src){
	return src;
}

int *ht_copy(int src){
	int *copy = malloc(sizeof(int));
	*copy = src;
	return copy;
}
void ht_deinit(int *src){
	free(src);
}

bool ht_comp(int src, int other){
	return src == other;
}


_HASHTABLE(11, ht, int, int)
SLL(test, ht, int, nothing, nothing, false)
HASHTABLE(11, ht, int, int, getHash, key, ht_copy, ht_comp, ht_deinit)

void ht_print_table(ht_ht *table) {
  int max_count = 0;
  int sum_count = 0;

  printf("------------HASH TABLE--------------\n");
  for (int i = 0; i < 11; i++) {
    printf("%i: ", i);
    int count = 0;
	ht_sll_element *item = ((table->table)[i]).firstElement;
    while (item != NULL) {
      printf("(%d)", *item->ptr);
      if (item->ptr != NULL) {
        count++;
      }
      item = item->next;
    }
    printf("\n");
    if (count > max_count) {
      max_count = count;
    }
    sum_count += count;
  }

  printf("------------------------------------\n");
  printf("Total items in hash table: %i\n", sum_count);
  printf("Maximum hash collisions: %i\n", max_count == 0 ? 0 : max_count - 1);
  printf("------------------------------------\n");
}
int main() {
	ht_ht table;
	ht_ht_init(&table);
	ht_ht_insert(&table, 1);
	ht_ht_insert(&table, 2);
	ht_ht_insert(&table, 3);
	ht_ht_insert(&table, 4);
	ht_ht_insert(&table, 5);
	ht_ht_insert(&table, 6);
	ht_ht_insert(&table, 7);
	ht_ht_insert(&table, 8);
	ht_ht_insert(&table, 9);
	ht_ht_insert(&table, 10);
	ht_ht_insert(&table, 11);
	ht_ht_insert(&table, 12);
	ht_ht_insert(&table, 41);
	ht_ht_insert(&table, 42);
	ht_ht_insert(&table, 43);
	ht_ht_insert(&table, 44);
	ht_ht_insert(&table, 45);
	ht_ht_insert(&table, 46);
	ht_ht_insert(&table, 47);
	ht_ht_insert(&table, 48);
	ht_ht_insert(&table, 49);
	ht_ht_insert(&table, 51);
	ht_ht_insert(&table, 61);
	ht_ht_insert(&table, 44);
	ht_print_table(&table);
	ht_ht_dispose(&table);
	return 0;
}