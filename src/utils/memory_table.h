/**
 * @addtogroup IFJ2024
 * @file memory_table.h
 * @brief Knihovna pro dynamickou alokaci a ukládání paměti a její bezpečné uvolnění
 * @author xramas01; Jakub Ramaseuski
 * 
 * Modul pro dynamickou alokaci a ukladani pameti a jeji bezpecne uvolneni
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "hashtable.h"

#ifndef MEMORY_TABLE_H
#define MEMORY_TABLE_H

#define MEMORY_TABLE_SIZE 9973

/**
 * @brief Získání ukazatele na prvek v seznamu na daném indexu.
 * 
 * @param src Ukazatel na prvek.
 * @return Index v tabulce.
 */
size_t memt_hash(void *src);

/**
 * @brief Získání klíče pro porovnání.
 * 
 * @param src Ukazatel na prvek.
 * @return Klíč pro porovnání.
 */
void *memt_key(void *src);

/**
 * @brief Kopírování prvku.
 * 
 * @param src Ukazatel na prvek.
 * @return Ukazatel na kopii prvku.
 */
void *memt_copy(void *src);

/**
 * @brief Uvolnění paměti.
 * 
 * @param src Ukazatel na prvek.
 */
void memt_deinit(void **src);

/**
 * @brief Porovnání dvou prvků.
 * 
 * @param src Ukazatel na prvek.
 * @param other Ukazatel na druhý prvek.
 * @return 1 pokud jsou prvky stejné, jinak 0.
 */
size_t memt_comp(void *src, void *other);

/**
 * @brief Uvolni pamet alokovanou pro hasovaci tabulku a vsechny jeji prvky.
 * 
 * @param src Ukazatel na prvek.
 */
void memt_deinit(void **src);

/**
 * @brief Bezpečné uvolnění paměti jednosměrně vázaného seznamu.
 * 
 * @param src Ukazatel na prvek.
 */
void memt_safe_deinit(void **src);

/**
 * @brief Porovnání dvou prvků.
 * 
 * @param src Ukazatel na prvek.
 * @param other Ukazatel na druhý prvek.
 */
size_t memt_comp(void *src, void *other);

/**
 * @brief Bezpečné uvolnění paměti hasovací tabulky.
 * 
 * @param sll Ukazatel na seznam, ve kterém se má prvek smazat.
 * @param index Index prvku, který se má smazat.
 */
//void memory_sll_safe_delete(memory_sllist *sll, size_t index);

/**
 * @brief Uvolni pamet alokovanou pro seznam.
 * 
 * @param table Ukazatel na tabulku, ve které se má prvek smazat.
 * @param key Klíč, podle kterého se má prvek smazat.
 */
//void memory_ht_safe_delete(memory_ht *table, void *key);

/**
 * @brief Alokace pameti s moznosti sledovani.
 *
 * Funkce alokuje pamet o velikosti size a vraci ukazatel na ni. Pokud je
 * zapnuta kontrola pameti (safe_memory), tak se pamet take ulozi do tabulky
 * pameti, cimz ji lze pozdeji sledovat a uvolnit.
 *
 * @param size Velikost pameti, ktera se ma alokovat.
 * @return Ukazatel na alokovanou pamet.
 */
void *imalloc(size_t size);

/**
 * @brief Realokace pameti s moznosti sledovani.
 * 
 * Funkce realokuje pamet, ktera byla alokovana pomoci imalloc.
 * Pokud je zapnuta kontrola pameti, tak se pokusime najit puvodni pamet
 * v tabulce pameti a nahradit ji novou pameti.
 * 
 * @param ptr Ukazatel na puvodni pamet.
 * @param size Nova velikost pameti.
 * @return Ukazatel na nove alokovanou pamet.
 */
void *irealloc(void *ptr, size_t size);

/**
 * @brief Uvolni pamet alokovanou pomoci imalloc.
 *
 * Funkce uvolni pamet, ktera byla alokovana pomoci imalloc.
 * Pokud je zapnuta kontrola pameti, tak se prvne pokusime vymazat
 * prvky z tabulky pameti, cimz ji uvolnime, jinak uvolnime pamet s pomoci free.
 *
 * @param ptr Ukazatel na pamet, ktera se ma uvolnit.
 */
void ifree(void *ptr);
_HASHTABLE(MEMORY_TABLE_SIZE, memory, void *, void *)

memory_ht _memory_table;
bool safe_memory = false; 
#endif /* MEMORY_TABLE_H */

/*** Konec souboru memory_table.h ***/