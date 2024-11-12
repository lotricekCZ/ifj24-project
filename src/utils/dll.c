#include <stdlib.h>

/**
 * @file dll.c
 * @brief Dynamicky alokovany spojovy seznam
 *
 * Modul obsahuje funkci pro inicializaci seznamu, vkladani a odebrani prvku
 * a funkcionality pro praci s aktivnim prvkem.
 *
 * @author xramas01 Jakub Ramaseuski
 */
#include "dll.h"
#ifndef DLL_C
#define DLL_C
/**
 * @brief Inicializace dynamicky alokovanho seznamu
 *
 * Funkce inicializuje dynamicky alokovany seznam. Vraci ukazatel na
 * strukturu seznamu, nebo NULL, pokud selhalo alokovani pameti.
 *
 * @return Ukazatel na inicializovanou strukturu seznamu, nebo NULL.
 */
#define DLL_INIT(name, prefix, element, deallocate)  \
	prefix##_dllist *prefix##_dll_init()             \
	{                                                \
		prefix##_dllist *dll = malloc(sizeof(*dll)); \
		if (dll == NULL)                             \
		{                                            \
			return NULL;                             \
		}                                            \
		dll->firstElement = NULL;                    \
		dll->lastElement = NULL;                     \
		dll->currentLength = 0;                      \
		return dll;                                  \
	}
/**
 * @brief Vymaze vsechny prvky seznamu
 *
 * Funkce vymaze vsechny prvky seznamu a uvolni pamet.
 */
#define DLL_CLEAR(name, prefix, element, deallocate)           \
	void prefix##_dll_clear(prefix##_dllist *dll)              \
	{                                                          \
		while (dll->firstElement != NULL)                      \
		{                                                      \
			prefix##_dll_element_ptr temp = dll->firstElement; \
			dll->firstElement = dll->firstElement->next;       \
			deallocate(temp->ptr);                             \
			if (deallocate == nothing)                         \
				free(temp->ptr);                               \
			free(temp);                                        \
		}                                                      \
	}

/**
 * @brief Uvolni pamet alokovanou pro seznam
 *
 * Funkce uvolni pamet alokovanou pro seznam a vsechny jeho prvky.
 */
#define DLL_DISPOSE(name, prefix, element, deallocate) \
	void prefix##_dll_dispose(prefix##_dllist *dll)    \
	{                                                  \
		prefix##_dll_clear(dll);                       \
		free(dll);                                     \
	}
/**
 * @brief Vlozeni prvku do seznamu na pozici index
 *
 * Funkce vlozi prvek s hodnotou data do seznamu na pozici index.
 * Alokujem pamet pro novy prvek, protoze funkci je dano, ze data jsou validni.
 * V pripade, ze selhalo alokovani pameti, funkce nic nedela.
 *
 * @param dll Ukazatel na seznam, do ktereho se ma prvek vlozit.
 * @param index Pozice, na kterou se ma prvek vlozit.
 * @param data Hodnota prvku, ktery se ma vlozit.
 */
#define DLL_INSERT(name, prefix, element, copy, init, deallocate)                                   \
	void prefix##_dll_insert(prefix##_dllist *dll, size_t index, element data)                \
	{                                                                                         \
		prefix##_dll_element_ptr newElement = malloc(sizeof(struct _##prefix##_dll_element)); \
		if (newElement == NULL)                                                               \
		{                                                                                     \
			return;                                                                           \
		}                                                                                     \
		newElement->ptr = (init == nothing)? malloc(sizeof(element)): init();                                            \
		if (newElement->ptr == NULL)                                                          \
		{                                                                                     \
			return;                                                                           \
		}                                                                                     \
		if (copy != nothing)                                                                  \
			copy(newElement->ptr, data);                                                     \
		else                                                                                  \
			*newElement->ptr = data;                                                          \
		if (index == 0)                                                                       \
		{                                                                                     \
			newElement->previous = NULL;                                                      \
			newElement->next = dll->firstElement;                                             \
			if (dll->firstElement != NULL)                                                    \
			{                                                                                 \
				dll->firstElement->previous = newElement;                                     \
			}                                                                                 \
			else                                                                              \
			{                                                                                 \
				dll->lastElement = newElement;                                                \
			}                                                                                 \
			dll->firstElement = newElement;                                                   \
		}                                                                                     \
		else                                                                                  \
		{                                                                                     \
			prefix##_dll_element_ptr temp = prefix##_dll_at(dll, index - 1);                  \
			newElement->previous = temp;                                                      \
			newElement->next = temp->next;                                                    \
			if (temp->next != NULL)                                                           \
			{                                                                                 \
				temp->next->previous = newElement;                                            \
			}                                                                                 \
			else                                                                              \
			{                                                                                 \
				dll->lastElement = newElement;                                                \
			}                                                                                 \
			temp->next = newElement;                                                          \
		}                                                                                     \
		dll->currentLength++;                                                                 \
	}
/**
 * @brief Odstrani prvek na pozici index
 *
 * Funkce odstrani prvek na pozici index v seznamu. Pokud je seznam
 * prazdny, nebo je pozice mimo rozsah, funkce nic nedela.
 *
 * @param dll Ukazatel na seznam, ze ktereho se ma prvek odstranit.
 * @param index Pozice, na ktere se ma prvek odstranit.
 */
#define DLL_DELETE(name, prefix, element, deallocate)                \
	void prefix##_dll_delete(prefix##_dllist *dll, size_t index)     \
	{                                                                \
		prefix##_dll_element_ptr temp = prefix##_dll_at(dll, index); \
		if (temp->previous != NULL)                                  \
		{                                                            \
			temp->previous->next = temp->next;                       \
		}                                                            \
		else                                                         \
		{                                                            \
			dll->firstElement = temp->next;                          \
		}                                                            \
		if (temp->next != NULL)                                      \
		{                                                            \
			temp->next->previous = temp->previous;                   \
		}                                                            \
		else                                                         \
		{                                                            \
			dll->lastElement = temp->previous;                       \
		}                                                            \
		deallocate(temp->ptr);                                       \
		if (deallocate == nothing)                                   \
			free(temp->ptr);                                         \
		free(temp);                                                  \
		dll->currentLength--;                                        \
	}
/**
 * @brief Prida prvek na konec seznamu
 *
 * Funkce prida prvek s hodnotou data na konec seznamu.
 *
 * @param dll Ukazatel na seznam, do ktereho se ma prvek vlozit.
 * @param data Hodnota prvku, ktery se ma vlozit.
 */
#define DLL_PUSH_BACK(name, prefix, element, deallocate)            \
	void prefix##_dll_push_back(prefix##_dllist *dll, element data) \
	{                                                               \
		prefix##_dll_insert(dll, dll->currentLength, data);         \
	}

/**
 * @brief Nastavi aktivni prvek na prvni prvek v seznamu
 *
 * Funkce nastavi aktivni prvek na prvni prvek v seznamu.
 *
 * @param dll Ukazatel na seznam, v nemz se nastavuje aktivni prvek.
 */
#define DLL_FIRST(name, prefix, element, deallocate) \
	void prefix##_dll_first(prefix##_dllist *dll)    \
	{                                                \
		dll->activeElement = dll->firstElement;      \
	}
/**
 * @brief Prida prvek na zacatek seznamu
 *
 * Funkce prida prvek s hodnotou data na zacatek seznamu.
 *
 * @param dll Ukazatel na seznam, do ktereho se ma prvek vlozit.
 * @param data Hodnota prvku, ktery se ma vlozit.
 */
#define DLL_PUSH_FRONT(name, prefix, element, deallocate)            \
	void prefix##_dll_push_front(prefix##_dllist *dll, element data) \
	{                                                                \
		prefix##_dll_insert(dll, 0, data);                           \
	}
/**
 * @brief odstran  posledn  prvek z  seznamu
 *
 * Funkce odstran  posledn  prvek z  seznamu.
 *
 * @param dll Ukazatel na seznam, z  kter ho se ma prvek odebrat.
 */
#define DLL_POP_BACK(name, prefix, element, deallocate)   \
	void prefix##_dll_pop_back(prefix##_dllist *dll)      \
	{                                                     \
		prefix##_dll_delete(dll, dll->currentLength - 1); \
	}
/**
 * @brief Odstrani prvni prvek z  seznamu
 *
 * Funkce odstrani prvni prvek z  seznamu.
 *
 * @param dll Ukazatel na seznam, z  kter ho se ma prvek odebrat.
 */
#define DLL_POP_FRONT(name, prefix, element, deallocate) \
	void prefix##_dll_pop_front(prefix##_dllist *dll)    \
	{                                                    \
		prefix##_dll_delete(dll, 0);                     \
	}
/**
 * @brief Vrati ukazatel na prvek seznamu na pozici index
 *
 * Funkce vrati ukazatel na prvek seznamu, ktery se nachazi na pozici index.
 *
 * @param dll Ukazatel na seznam, v nemz se hleda prvek.
 * @param index Pozice, na ktere se hleda prvek.
 * @return Ukazatel na prvek nebo NULL, pokud je seznam prazdny nebo je pozice mimo rozsah.
 */
#define DLL_AT(name, prefix, element, deallocate)                                \
	prefix##_dll_element_ptr prefix##_dll_at(prefix##_dllist *dll, size_t index) \
	{                                                                            \
		prefix##_dll_element_ptr temp = dll->firstElement;                       \
		for (size_t i = 0; i < index; i++)                                       \
		{                                                                        \
			temp = temp->next;                                                   \
		}                                                                        \
		return temp;                                                             \
	}
/**
 * @brief Vrati dalsi prvek v seznamu a posune aktivni prvek na dalsi pozici
 *
 * Funkce vrati dalsi prvek v seznamu, posune aktivni prvek na dalsi pozici a vrati jej.
 * Pokud je seznam prazdny nebo je aktivni prvek posledni v seznamu, vrati NULL.
 *
 * @param dll Ukazatel na seznam, v nemz se hleda dalsi prvek.
 * @return Ukazatel na dalsi prvek nebo NULL, pokud je seznam prazdny nebo je aktivni
 * prvek posledni v seznamu.
 */
#define DLL_NEXT(name, prefix, element, deallocate)                  \
	prefix##_dll_element_ptr prefix##_dll_next(prefix##_dllist *dll) \
	{                                                                \
		if (dll->lastElement == NULL)                                \
		{                                                            \
			return NULL;                                             \
		}                                                            \
		dll->activeElement = dll->activeElement->next;               \
		return dll->activeElement;                                   \
	}
#define DLL(name, prefix, element, copy, init, deallocate)    \
	DLL_INIT(name, prefix, element, deallocate)         \
	DLL_INSERT(name, prefix, element, copy, init, deallocate) \
	DLL_DELETE(name, prefix, element, deallocate)       \
	DLL_PUSH_BACK(name, prefix, element, deallocate)    \
	DLL_PUSH_FRONT(name, prefix, element, deallocate)   \
	DLL_POP_BACK(name, prefix, element, deallocate)     \
	DLL_POP_FRONT(name, prefix, element, deallocate)    \
	DLL_AT(name, prefix, element, deallocate)           \
	DLL_NEXT(name, prefix, element, deallocate)         \
	DLL_DISPOSE(name, prefix, element, deallocate)      \
	DLL_CLEAR(name, prefix, element, deallocate)        \
	DLL_FIRST(name, prefix, element, deallocate)

#endif