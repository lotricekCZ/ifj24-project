/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

#include <stdlib.h>
#include <stdio.h>
#include "errors.h"

/**
 * @file errors.c
 * @brief Modul pro vypisovani chybovych hlaseni
 *
 * Modul obsahuje funkci pro vypisovani chybovych hlaseni do standardniho
 * vystupu. Chybove hlaseni jsou definovany v poli @ref err_msg.
 *
 */
const Err_message err_msg[] = {
    {err_lexic, "lexical error: current lexeme %s is not valid in this context\n"},
    {err_syntax, "syntax error: syntax error or missing header within program file\n"},
    {err_undef, "semantic error: undefined function or variable\n"},
    {err_param, "semantic error: wrong number/type of parameters in function call; wrong type or non-permitted disposal of function return value.\n"},
    {err_redef, "semantic error: redefinition of variable/function\n"},
    {err_ret_val, "semantic error: wrong return value in function call\n"},
    {err_dt_invalid, "semantic error: incompatible types in arithmetic, comparison or relational expressions; wrong type or non-permitted disposal of function return value.\n"},
    {err_dt_unknown, "semantic error: could not deduce type from expression\n"},
    {err_dt_unused, "semantic error: unused variable/function\n"},
    {err_semantic, "semantic error: other semantic errors\n"},
    {err_internal, "internal error: error on the level of compiler (program may be ok, but the compiler is not)\n"}
    };

/**
 * @brief Najde index zpravy v poli err_msg podle kody chyby
 *
 * Funkce prochazi pole err_msg a najde index zpravy, ktera ma kod chyby
 * shodny s parametrem code. Pokud takovy kod chyby neexistuje, vrati
 * index posledniho prvku v poli (ktery ma kod chyby err_internal).
 *
 * @param code kod chyby, ktery chceme najit
 * @return index zpravy v poli err_msg
 */
size_t err_fetch(err_codes code)
{
    for (int i = 0; i < ERR_COUNT; i++)
    {
        if (err_msg[i].code == code)
        {
            return i;
        }
    }
    return ERR_COUNT - 1;
}

/**
 * @brief Vypise chybovou zpravy dle kody chyby
 *
 * Funkce vypise na standardni vystup chybovou zpravy dle kody chyby.
 * Kdyz je kod chyby neplatny, vypise se chybova zprava pro kod err_internal.
 *
 * @param code Kod chyby
 */
void err_print(err_codes code)
{
    printf("%s", err_msg[err_fetch(code)].format);
    if(code != err_none){
        // ukoncit preklad, uvolnit vsechny zdroje
        exit(code);
    }
}
