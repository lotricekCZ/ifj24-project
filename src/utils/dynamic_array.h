#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include "errors.h"

// Struktura pro dynamické pole
typedef struct {
    int* data;          // Ukazatel na dynamicky alokovaná data
    int size;        // Aktuální počet prvků v poli
    int capacity;    // Celková kapacita alokované paměti
} dynamic_array_t;

// Inicializace dynamického pole
err_codes dynamic_array_init(dynamic_array_t* array);
err_codes dynamic_array_insert(dynamic_array_t* array, int value);
void dynamic_array_clear(dynamic_array_t* array);
void dynamic_array_destroy(dynamic_array_t* array);

#endif // DYNAMIC_ARRAY_H