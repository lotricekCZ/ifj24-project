/**
 * @addtogroup IFJ2024
 * @file dynamic_array.h
 * @brief Knihovna pro práci s dynamickým polem
 * @author xhubacv00; Vojtěch Hubáček
 * 
 * Knihovna obsahuje strukturu pro dynamické pole a funkce pro práci s dynamickým polem.
 */

#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include "errors.h"

/**
 * @struct dynamic_array_t
 * @brief Struktura reprezentující dynamické pole
 * 
 * Obsahuje ukazatel na dynamicky alokovaná data, aktuální počet prvků v poli a celkovou kapacitu alokované paměti.
 */
typedef struct {
    int* data;      /**< Ukazatel na data */
    int size;       /**< Aktuální počet prvků v poli */
    int capacity;   /**< Celková alokovaná kapacita v bytech */
} dynamic_array_t;

/**
 * @brief Inicializace dynamického pole
 * 
 * Funkce inicializuje strukturu dynamického pole a alokuje paměť pro data.
 * 
 * @param array Ukazatel na strukturu dynamického pole
 * @return Návratový kód funkce
 */
err_codes dynamic_array_init(dynamic_array_t* array);

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
err_codes dynamic_array_insert(dynamic_array_t* array, int value);

/**
 * @brief Vyčištění dynamického pole
 * 
 * Funkce vyčistí dynamické pole, tzn. nastaví jeho velikost na 0. 
 *
 * @param array Ukazatel na strukturu dynamického pole
 */
void dynamic_array_clear(dynamic_array_t* array);

/**
 * @brief Uvolnění paměti alokované pro dynamické pole
 * 
 * Funkce uvolní paměť alokovanou pro dynamické pole.
 * 
 * @param array Ukazatel na strukturu dynamického pole
 */
void dynamic_array_destroy(dynamic_array_t* array);

#endif /* DYNAMIC_ARRAY_H */

/*** Konec souboru dynamic_array.h ***/