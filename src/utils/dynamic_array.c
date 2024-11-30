/**
 * @addtogroup IFJ2024
 * @file dynamic_array.c
 * @brief Implementace funkcí pro práci s dynamickým polem
 * @author xhubacv00; Vojtěch Hubáček
 * 
 * Implementace funkcí pro práci s dynamickým polem.
 */

#include "dynamic_array.h"
#include "memory_table.h"

/**
 * @brief Inicializace dynamického pole
 * 
 * Funkce inicializuje strukturu dynamického pole a alokuje paměť pro data.
 * 
 * @param array Ukazatel na strukturu dynamického pole
 * @return Návratový kód funkce
 */
err_codes dynamic_array_init(dynamic_array_t* array) {
    array->data = (int*)imalloc(sizeof(int) * 16);
    if (array->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return err_internal;
    }
    array->size = 0;
    array->capacity = 16;
    return err_none;
}

/**
 * @brief Přidání prvku do dynamického pole
 * 
 * Funkce přidává prvek do dynamického pole.
 * Pokud je pole plné, tak se zdvojnásobí jeho kapacita.
 * 
 * @param array Ukazatel na strukturu dynamického pole
 * @param value Hodnota, která se má přidat do pole
 * @return Návratový kód funkce
 */
err_codes dynamic_array_insert(dynamic_array_t* array, int value) {
    if (array->size == array->capacity) {
        array->capacity *= 2;
        array->data = (int*)irealloc(array->data, sizeof(int) * array->capacity);
        if (array->data == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            return err_internal;
        }
    }
    array->data[array->size++] = value;
    return err_none;
}

/**
 * @brief Vyčištění dynamického pole
 * 
 * Funkce vyčistí dynamické pole, tzn. nastaví jeho velikost na 0.
 * 
 * @param array Ukazatel na strukturu dynamického pole
 * @return Návratový kód funkce
 */
void dynamic_array_clear(dynamic_array_t* array) {
    array->size = 0;
}

/**
 * @brief Uvolnění paměti alokované pro dynamické pole
 * 
 * Funkce uvolní paměť alokovanou pro dynamické pole.
 * 
 * @param array Ukazatel na strukturu dynamického pole
 * @return Návratový kód funkce
 */
void dynamic_array_destroy(dynamic_array_t* array) {
    if (array->data != NULL) {
        ifree(array->data);
    }
    array->data = NULL;
    array->size = 0;
    array->capacity = 0;
}