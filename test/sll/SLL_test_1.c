#include <stdio.h>

#include "../../src/utils/sll.c"
#include "../../src/utils/sll.h"

/**
 * @file DLL_test_1.c
 * @brief Testovaci soubor pro dynamicky alokovany spojovy seznam
 *
 * Modul obsahuje test inicializace pro dynamicky alokovany spojovy seznam.
 * seznam je iniciovan, je do nej vlozeno, a pote je z nej odstraneno.
 * testovane funkce
 * - inicializace
 * - vlozeni dopredu
 * - vlozeni na index
 * - vymazani prvku
 * - deinicializace
 *
 * @author xramas01 Jakub Ramaseuski
 */

_SLL(test, t, int)
t_sllist *t_sll_init()
{
	t_sllist *sll = malloc(sizeof(*sll));
	if (sll == ((void *)0))
	{
		return ((void *)0);
	}
	sll->firstElement = ((void *)0);
	sll->currentLength = 0;
	return sll;
}
void t_sll_insert(t_sllist *sll, size_t index, int data)
{
	t_sll_element_ptr newElement = malloc(sizeof(struct _t_sll_element));
	if (newElement == ((void *)0))
	{
		return;
	}
	newElement->ptr = malloc(sizeof(int));
	if (newElement->ptr == ((void *)0))
	{
		return;
	}
	if (nothing != nothing)
		nothing(newElement->ptr, data);
	else
		*newElement->ptr = data;
	if (index == 0)
	{
		newElement->next = sll->firstElement;
		sll->firstElement = newElement;
	}
	else
	{
		t_sll_element_ptr temp = t_sll_at(sll, index - 1);
		if (temp->next != ((void *)0))
		{
			newElement->next = temp->next;
		}
		else
		{
			newElement->next = ((void *)0);
		}
		temp->next = newElement;
	}
	sll->currentLength++;
}
void t_sll_delete(t_sllist *sll, size_t index)
{
	if (index == 0)
	{
		t_sll_element_ptr temp = sll->firstElement;
		sll->firstElement = temp->next;
		if (temp->next != ((void *)0))
		{
			nothing(temp->ptr);
			if (nothing == nothing)
				free(temp->ptr);
			free(temp);
			sll->currentLength--;
		}
	}
	t_sll_element_ptr temp = t_sll_at(sll, index - 1);
	t_sll_element_ptr target = NULL;
	if (temp != ((void *)0) && temp->next != ((void *)0)){
		target = temp->next;
		temp->next = temp->next->next;
		}
	if(target != NULL)
		nothing(target->ptr);
	if (nothing == nothing)
		free(target->ptr);
	free(target);
	sll->currentLength--;
}
void t_sll_push_front(t_sllist *sll, int data) { t_sll_insert(sll, 0, data); }
void t_sll_pop_front(t_sllist *sll) { t_sll_delete(sll, 0); }
t_sll_element_ptr t_sll_at(t_sllist *sll, size_t index)
{
	if (index >= sll->currentLength)
	{
		return ((void *)0);
	}
	t_sll_element_ptr temp = sll->firstElement;
	for (size_t i = 0; i < index; i++)
	{
		temp = temp->next;
	}
	return temp;
}
t_sll_element_ptr t_sll_next(t_sllist *sll)
{
	if (sll->firstElement == ((void *)0) || sll->activeElement == ((void *)0))
	{
		return ((void *)0);
	}
	sll->activeElement = sll->activeElement->next;
	return sll->activeElement;
}
void t_sll_dispose(t_sllist *sll)
{
	t_sll_clear(sll);
	free(sll);
}
void t_sll_clear(t_sllist *sll)
{
	while (sll->firstElement != ((void *)0))
	{
		t_sll_element_ptr temp = sll->firstElement;
		sll->firstElement = sll->firstElement->next;
		nothing(temp->ptr);
		if (nothing == nothing)
			free(temp->ptr);
		free(temp);
	}
}
void t_sll_first(t_sllist *sll) { sll->activeElement = sll->firstElement; }
_Bool t_sll_is_active(t_sllist *sll) { return sll->activeElement != ((void *)0); }
t_sll_element_ptr t_sll_front(t_sllist *sll) { return sll->firstElement; }
void sll_print(t_sllist *sll)
{
	t_sll_element_ptr temp = sll->firstElement;
	while (temp != NULL)
	{
		printf("%d ", *temp->ptr);
		temp = temp->next;
	}
	printf("\n");
}

int main()
{
	t_sllist *sll = t_sll_init();

	t_sll_push_front(sll, 1);
	t_sll_push_front(sll, 2);
	t_sll_push_front(sll, 3);
	t_sll_push_front(sll, 4);
	t_sll_push_front(sll, 5);
	t_sll_push_front(sll, 6);
	t_sll_push_front(sll, 7);
	t_sll_push_front(sll, 8);
	t_sll_push_front(sll, 9);
	t_sll_push_front(sll, 10);
	t_sll_push_front(sll, 11);
	t_sll_insert(sll, 5, 42);
	t_sll_delete(sll, 5);
	t_sll_insert(sll, 5, 42);
	sll_print(sll);
	t_sll_clear(sll);
	sll_print(sll);
	t_sll_dispose(sll);
	return 0;
}