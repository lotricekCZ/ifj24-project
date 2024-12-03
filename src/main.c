/**
 * @defgroup IFJ2024
 * @file main.c
 * @brief Hlavní soubor programu
 * @author xramas01; Jakub Ramaseuski
 */

#include <stdio.h>
#include "utils/token_types.h"
#include "parser/parser.h"
#include "utils/errors.h"
#include "utils/memory_table.h"
#include "scanner/scanner.h"

/**
 * @brief Hlavní funkce programu
 * 
 * Hlavní funkce programu, která inicializuje scanner a spouští parsování zdrojového kódu.
 * Inicializace memory_ht pro hlídání alokované paměti a bezpečné uvolnění při chybě.
 * 
 * @return Návratový kód programu
 */
int main()
{
    // Aktivace bezpečného uvolňování paměti
    safe_memory = true;
    memory_ht_init(&_memory_table);

    // Inicializace scanneru
    Scanner_ptr scanner = scn_init(NULL);
    if (scanner == NULL) {
        fprintf(stderr, "Failed to initialize scanner.\n");
        return err_internal;
    }

    // Spuštění parsování
    err_codes error = parse(scanner);

    // Uvolnění paměti
    scn_free(scanner);
    memory_ht_dispose(&_memory_table);
    return error;
}

/*** Konec souboru main.c ***/