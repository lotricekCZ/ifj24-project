#include "token_dll.h"

tok_dllist *tok_dll_init()
{
	tok_dllist *dll = malloc(sizeof(*dll));
	if (dll == ((void *)0))
	{
		return ((void *)0);
	}
	dll->firstElement = ((void *)0);
	dll->lastElement = ((void *)0);
	dll->currentLength = 0;
	return dll;
}
void tok_dll_insert(tok_dllist *dll, size_t index, Token data)
{
	tok_dll_element_ptr newElement = malloc(sizeof(struct _tok_dll_element));
	if (newElement == ((void *)0))
	{
		return;
	}
	newElement->ptr = malloc(sizeof(Token));
	if (newElement->ptr == ((void *)0))
	{
		return;
	}
	if (tok_copy != nothing)
		tok_copy(newElement->ptr, data);
	else
		*newElement->ptr = data;
	if (index == 0)
	{
		newElement->previous = ((void *)0);
		newElement->next = dll->firstElement;
		if (dll->firstElement != ((void *)0))
		{
			dll->firstElement->previous = newElement;
		}
		else
		{
			dll->lastElement = newElement;
		}
		dll->firstElement = newElement;
	}
	else
	{
		tok_dll_element_ptr temp = tok_dll_at(dll, index - 1);
		newElement->previous = temp;
		newElement->next = temp->next;
		if (temp->next != ((void *)0))
		{
			temp->next->previous = newElement;
		}
		else
		{
			dll->lastElement = newElement;
		}
		temp->next = newElement;
	}
	dll->currentLength++;
}
void tok_dll_delete(tok_dllist *dll, size_t index)
{
	tok_dll_element_ptr temp = tok_dll_at(dll, index);
	if (temp->previous != ((void *)0))
	{
		temp->previous->next = temp->next;
	}
	else
	{
		dll->firstElement = temp->next;
	}
	if (temp->next != ((void *)0))
	{
		temp->next->previous = temp->previous;
	}
	else
	{
		dll->lastElement = temp->previous;
	}
	tok_free(temp->ptr);
	if (tok_free == nothing)
		free(temp->ptr);
	free(temp);
	dll->currentLength--;
}
void tok_dll_push_back(tok_dllist *dll, Token data) { tok_dll_insert(dll, dll->currentLength, data); }
void tok_dll_push_front(tok_dllist *dll, Token data) { tok_dll_insert(dll, 0, data); }
void tok_dll_pop_back(tok_dllist *dll) { tok_dll_delete(dll, dll->currentLength - 1); }
void tok_dll_pop_front(tok_dllist *dll) { tok_dll_delete(dll, 0); }
tok_dll_element_ptr tok_dll_at(tok_dllist *dll, size_t index)
{
	tok_dll_element_ptr temp = dll->firstElement;
	for (size_t i = 0; i < index; i++)
	{
		temp = temp->next;
	}
	return temp;
}
tok_dll_element_ptr tok_dll_next(tok_dllist *dll)
{
	if (dll->lastElement == ((void *)0))
	{
		return ((void *)0);
	}
	dll->activeElement = dll->activeElement->next;
	return dll->activeElement;
}
void tok_dll_dispose(tok_dllist *dll)
{
	tok_dll_clear(dll);
	free(dll);
}
void tok_dll_clear(tok_dllist *dll)
{
	while (dll->firstElement != ((void *)0))
	{
		tok_dll_element_ptr temp = dll->firstElement;
		dll->firstElement = dll->firstElement->next;
		tok_free(temp->ptr);
		if (tok_free == nothing)
			free(temp->ptr);
		free(temp);
	}
}
void tok_dll_first(tok_dllist *dll) { dll->activeElement = dll->firstElement; }