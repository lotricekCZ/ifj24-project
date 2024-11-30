/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

#include <stdlib.h>
#include <stdio.h>
#include "errors.h"
#include "memory_table.h"

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
    {err_internal, "internal error: error on the level of compiler (program may be ok, but the compiler is not)\n"}};

/**
 * @brief Najde index zprávy v poli err_msg podle kódu chyby
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
 * @brief Vypíše chybovou zprávu dle kódu chyby
 *
 * Funkce vypise na standardni vystup chybovou zpravy dle kody chyby.
 * Kdyz je kod chyby neplatny, vypise se chybova zprava pro kod err_internal.
 *
 * @param code Kód chyby
 */
void err_print(err_codes code, bool message_needed)
{
    if (message_needed)
        fprintf(stderr, "%s", err_msg[err_fetch(code)].format);
    if (code != err_none)
    {
        // ukoncit preklad, uvolnit vsechny zdroje
        memory_ht_dispose(&_memory_table);
        exit(code);
    }
}

/**
 * @brief Ukončí překlad s chybovým hlášením lexical error
 *
 * Funkce vypise na standardni vystup chybovou zpravy v podobe
 * "lexical error: <what>" a potom ukonci preklad s kodem chyby
 * err_lexic.
 *
 * @param what Textový řetězec, který bude vypsaný jako část chybové zprávy.
 */
void exit_lexic(char *what)
{
    fprintf(stderr, "%s\n", what);
    err_print(err_lexic, false);
}

/**
 * @brief Ukončí překlad s chybovým hlášením syntax error
 *
 * Funkce vypise na standardni vystup chybovou zpravy v podobe
 * "syntax error: syntax error or missing header within program file\n"
 * a potom ukonci preklad s kodem chyby err_syntax.
 */
void exit_syntax()
{
    err_print(err_syntax, true);
}

/**
 * @brief Ukončí překlad s chybovým hlášením undefined function or variable
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: undefined function or variable\n"
 * a potom ukonci preklad s kodem chyby err_undef.
 */
void exit_undef()
{
    err_print(err_undef, true);
}

/**
 * @brief Ukončí překlad s chybovým hlášením o chybném počtu nebo typu parametrů
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: wrong number/type of parameters in function call; wrong type or non-permitted disposal of function return value.\n"
 * a potom ukonci preklad s kodem chyby err_param.
 */
void exit_param()
{
    err_print(err_param, true);
}

/**
 * @brief Ukončí překlad s chybovým hlášením o chybné redefinici proměnné nebo funkce
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: redefinition of variable/function; assigment to non-modifiable variable.\n"
 * a potom ukonci preklad s kodem chyby err_redef.
 */
void exit_redef()
{
    err_print(err_redef, true);
}

/**
 * @brief Ukončí překlad s chybovým hlášením o chybejícím nebo přebývajícím výrazu v příkazu návratu z funkce
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: missing/extra expression in function return statement.\n"
 * a potom ukonci preklad s kodem chyby err_ret_val.
 */
void exit_ret_val()
{
    err_print(err_ret_val, true);
}

/**
 * @brief Ukončí překlad s chybovým hlášením o chybějící nebo nekompatibilní type v aritmetických, řetězcových a relačních výrazech,
 * nebo o chybějícím nebo přebývajícím výrazu v příkazu návratu z funkce.
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: incompatible types in arithmetic, comparison or relational expressions; wrong type or non-permitted disposal of function return value.\n"
 * a potom ukonci preklad s kodem chyby err_dt_invalid.
 */
void exit_dt_invalid()
{
    err_print(err_dt_invalid, true);
}

/**
 * @brief Ukončí překlad s chybovým hlášením o neznámém typu výrazu
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: could not deduce type from expression\n"
 * a potom ukonci preklad s kodem chyby err_dt_unknown.
 */
void exit_dt_unknown()
{
    err_print(err_dt_unknown, true);
}

/**
 * @brief Ukončí překlad s chybovým hlášením o nevyužité proměnné v jejím rozsahu platnosti
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: unused variable in its scope.\n"
 * a potom ukonci preklad s kodem chyby err_dt_unused.
 */
void exit_dt_unused()
{
    err_print(err_dt_unused, true);
}


/**
 * @brief Ukončí překlad s chybovým hlášením jine sémantické chyby
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: other semantic errors\n"
 * a potom ukonci preklad s kodem chyby err_semantic.
 */
void exit_semantic()
{
    err_print(err_semantic, true);
}


/**
 * @brief Interní chyba překladače
 *
 * Funkce vypíše na standardní výstup chybové hlášení v podobě
 * "internal error: error on the level of compiler (program may be ok, but the compiler is not)\n"
 * a potom ukončí překlad s kódem chyby err_internal.
 */
void _exit_internal()
{
    err_print(err_internal, true);
}

/*** Konec souboru errors.c ***/