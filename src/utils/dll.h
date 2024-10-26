#ifndef DLL_H
#define DLL_H
#define _DLL(name, prefix, element)                                   \
	typedef struct _##prefix##_dll_element                                        \
	{                                                                             \
		element *ptr;                                                             \
		struct _##prefix##_dll_element *next;                                     \
		struct _##prefix##_dll_element *previous;                                 \
	} prefix##_dll_element;                                                       \
                                                                                  \
	typedef struct _##prefix##_dll_element *prefix##_dll_element_ptr;             \
	typedef struct                                                                \
	{                                                                             \
		prefix##_dll_element_ptr firstElement;                                    \
		prefix##_dll_element_ptr activeElement;                                   \
		prefix##_dll_element_ptr lastElement;                                     \
		size_t currentLength;                                                     \
	} prefix##_dllist;                                                            \
                                                                                  \
	prefix##_dllist *prefix##_dll_init();                                         \
	void prefix##_dll_dispose(prefix##_dllist *dll);                              \
	void prefix##_dll_insert(prefix##_dllist *dll, size_t index, element data);  \
	void prefix##_dll_delete(prefix##_dllist *dll, size_t index);                 \
	void prefix##_dll_push_back(prefix##_dllist *dll, element data);             \
	void prefix##_dll_push_front(prefix##_dllist *dll, element data);            \
	void prefix##_dll_pop_back(prefix##_dllist *dll);                             \
	void prefix##_dll_pop_front(prefix##_dllist *dll);                            \
	void prefix##_dll_first(prefix##_dllist *dll);                            \
	prefix##_dll_element_ptr prefix##_dll_at(prefix##_dllist *dll, size_t index); \
	prefix##_dll_element_ptr prefix##_dll_next(prefix##_dllist *dll);

/**
 * @brief Nic nedela
 *
 * Funkce nic nedela, je zde z duvodu, aby v pripade, kdy
 * element nema zadnou specialni deiniciacni funkci mohl byt vlozen do makra.
 */
static void nothing() { // Lotricek netusim co si myslel touto funkciou, ale robi to bordel pri preklade
	
}
#endif

// void DLL_First(DLList *);

// void DLL_Last(DLList *);

// void DLL_GetFirst(DLList *, long *);

// void DLL_GetLast(DLList *, long *);

// void DLL_DeleteFirst(DLList *);

// void DLL_DeleteLast(DLList *);

// void DLL_DeleteAfter(DLList *);

// void DLL_DeleteBefore(DLList *);

// void DLL_InsertAfter(DLList *, long);

// void DLL_InsertBefore(DLList *, long);

// void DLL_GetValue(DLList *, long *);

// void DLL_SetValue(DLList *, long);

// void DLL_Next(DLList *);

// void DLL_Previous(DLList *);

// bool DLL_IsActive(DLList *);