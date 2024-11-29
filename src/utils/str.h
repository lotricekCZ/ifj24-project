/**
 * @addtogroup IFJ2024
 * @file str.h
 * @author xhubacv00; Vojtěch Hubáček
 * @brief Knihovna dynamického řetězce a funkcí pro práci s dynamickými řetězci
 * 
 * Tato knihovna poskytuje strukturu dynamického řetězce a funkce pro práci s dynamickými řetězci.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

#ifndef STR_H
#define STR_H

/**
 * @struct str_t
 * @brief Struktura reprezentující dynamický řetězec
 * 
 * Obsahuje ukazatel na data, aktuální délku řetězce a alokovanou kapacitu.
 * Automaticky je místo pro data zvětšováno podle potřeby při přidávání nových dat.
 */
typedef struct {
    char* data;     /**< Ukazatel na data řetězce */
    int length;     /**< Aktuální délka řetězce bez ukončovací nuly */
    int capacity;   /**< Celková alokovaná kapacita v bytech */
} str_t;

/**
 * @brief Inicializace řetězce
 * 
 * Funkce inicializuje strukturu řetězce a alokuje paměť pro data.
 * 
 * @param string Ukazatel na strukturu řetězce
 */
void str_init(str_t* string);

/**
 * @brief Přidání formátovaného řetězce
 * 
 * Funkce přidává formátovaný řetězec do struktury řetězce.
 * 
 * @param string Ukazatel na strukturu řetězce
 * @param source Formátovaný řetězec
 * @param ... Argumenty pro formátovaný řetězec
 */
void str_append(str_t* string, const char* source, ...);

/**
 * @brief Spojení dvou řetězců
 * 
 * Funkce spojuje dva řetězce do jednoho.
 * 
 * @param string Ukazatel na strukturu řetězce, do kterého se přidává
 * @param tmp Ukazatel na strukturu řetězce, který se přidává na konec prvního
 */
void str_unify(str_t* string, str_t* tmp);

/**
 * @brief Vyhledání podřetězce v řetězci
 * 
 * Funkce vyhledává podřetězec v řetězci.
 * 
 * @param string Ukazatel na strukturu řetězce
 * @param substring Podřetězec, který se hledá
 * @return Pravdivostní hodnota výsledeku vyhledávání
 */
bool str_search(str_t* string, const char* substring);

/**
 * @brief Výpis řetězce na standardní výstup
 * 
 * @param string Ukazatel na strukturu řetězce
 */
void str_printout(str_t* string);

/**
 * @brief Výpis řetězce na standardní výstup
 * 
 * Funkce vypisuje řetězec na standardní výstup.
 * 
 * @param string Ukazatel na strukturu řetězce
 */
void str_clear(str_t* string);

/**
 * @brief Výpis řetězce na standardní výstup
 * 
 * Funkce aktuální vyčistí obsah řetězce.
 * 
 * @param string Ukazatel na strukturu řetězce
 */
void str_destroy(str_t* string);

#endif /* STR_H */

/*** Konec souboru str.h ***/