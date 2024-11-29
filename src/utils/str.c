/**
 * @addtogroup IFJ2024
 * @file str.c
 * @author xhubacv00; Vojtěch Hubáček
 * @brief Implementace funkcí pro práci s dynamickými řetězci
 * 
 * Implementace funkcí pro práci s dynamickými řetězci.
 */

#include <stdio.h>
#include "str.h"
#include "memory_table.h"
#include "errors.h"


/**
 * @brief Inicializace řetězce
 * 
 * Funkce inicializuje strukturu řetězce a alokuje paměť pro data.
 * 
 * @param string Ukazatel na strukturu řetězce
 */
void str_init(str_t* string) {
    string->length = 0;
    string->capacity = 16;
    string->data = (char*)imalloc(string->capacity);
    if (string->data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit_internal();
    }
    string->data[0] = '\0';
}


/**
 * @brief Přidání formátovaného řetězce
 * 
 * Funkce přidává formátovaný řetězec do struktury řetězce.
 * 
 * @param string Ukazatel na strukturu řetězce
 * @param source Formátovaný řetězec
 * @param ... Argumenty pro formátovaný řetězec
 */
void str_append(str_t* string, const char* source, ...) {
    va_list args;
    va_start(args, source);
    
    //Zjištění potřebné délky bufferu
    va_list args_copy;
    va_copy(args_copy, args);
    int needed_length = vsnprintf(NULL, 0, source, args_copy);
    va_end(args_copy);
    
    //Kontrola, zda je potřeba zvětšit buffer
    size_t new_length = string->length + needed_length;
    if (new_length >= string->capacity) {
        size_t new_capacity = string->capacity;
        while (new_capacity <= new_length) {
            new_capacity *= 2;
        }
        char* new_data = (char*)irealloc(string->data, new_capacity);
        if (new_data == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            va_end(args);
            return;
        }
        string->data = new_data;
        string->capacity = new_capacity;
    }
    
    //Formátování do bufferu
    vsnprintf(string->data + string->length, string->capacity - string->length, source, args);
    string->length = new_length;
    
    va_end(args);
}

/**
 * @brief Spojení dvou řetězců
 * 
 * Funkce spojuje dva řetězce do jednoho.
 * 
 * @param string Ukazatel na strukturu řetězce, do kterého se přidává
 * @param tmp Ukazatel na strukturu řetězce, který se přidává na konec prvního
 */
void str_unify(str_t* string, str_t* tmp) {
    str_append(string, "%s", tmp->data);
}

/**
 * @brief Vyhledání podřetězce v řetězci
 * 
 * Funkce vyhledává podřetězec v řetězci.
 * 
 * @param string Ukazatel na strukturu řetězce
 * @param substring Podřetězec, který se hledá
 * @return Pravdivostní hodnota výsledeku vyhledávání
 */
bool str_search(str_t* string, const char* substring) {
    if (string == NULL || string->data == NULL || substring == NULL) {
        return false;
    }
    return (strstr(string->data, substring) != NULL);
}

/**
 * @brief Výpis řetězce na standardní výstup
 * 
 * Funkce vypisuje řetězec na standardní výstup.
 * 
 * @param string Ukazatel na strukturu řetězce
 */
void str_printout(str_t* string) {
    printf("%s", string->data);
}

/**
 * @brief Výpis řetězce na standardní výstup
 * 
 * Funkce aktuální vyčistí obsah řetězce.
 * 
 * @param string Ukazatel na strukturu řetězce
 */
void str_clear(str_t* string) {
    string->data[0] = '\0';
    string->length = 0;
}

/**
 * @brief Uvolnění paměti alokované pro řetězec
 * 
 * Funkce uvolní paměť alokovanou pro řetězec.
 * 
 * @param string Ukazatel na strukturu řetězce
 */
void str_destroy(str_t* string) {
    ifree(string->data);
    string->data = NULL;
    string->length = 0;
    string->capacity = 0;
}

/*** Konec souboru str.c ***/