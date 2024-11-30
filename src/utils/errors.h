/**
 * @addtogroup IFJ2024
 * @file errors.h
 * @brief Knihovny výpisů chybových hlašení
 * @author xramas01; Jakub Ramaseuski
 * 
 * Knihovna obsahující chybová hlášení a funkce pro výpis chybových hlášení.
 */

#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <stdbool.h>

/**
 * @brief Vypíše informaci o souboru a řádku, kde byla nalezena chyba a ukončí program.
 *
 * Toto makro se používá pro ukončení programu v případě, že dojde k vnitřní chybě
 * (tj. chybě, která nebyla způsobena špatným vstupem, ale vnitřní chybou programu).
 */
#define exit_internal() {fprintf(stderr, "%s: %d\n", __FILE__, __LINE__); _exit_internal();}

/**
 * @brief Počet chybových kódů
 */
#define ERR_COUNT 11

/**
 * @enum err_codes
 * @brief Výčet chybových kódů
 */
typedef enum _err_codes{
	err_none,			// 0 - normální stav
	err_lexic,			// 1 - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému). 
	err_syntax,	   		// 2 - chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu, chybějící hlavička, atp.). 
	err_undef,			// 3 - sémantická chyba v programu - nedefinovaná funkce či proměnná. 
	err_param,			// 4 - sémantická chyba v programu - špatný počet/typ parametrů u volání funkce; špatný typ či nepovolené zahození návratové hodnoty z funkce. 
	err_redef,			// 5 - sémantická chyba v programu - redefinice proměnné nebo funkce; přiřazení do nemodifikovatelné proměnné. 
	err_ret_val,		// 6 - sémantická chyba v programu - chybějící/přebývající výraz v příkazu návratu z funkce. 
	err_dt_invalid,		// 7 - sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech; nekompatibilní typ výrazu (např. při přiřazení). 
	err_dt_unknown,		// 8 - sémantická chyba odvození typu - typ proměnné není uveden a nelze odvodit od použitého výrazu. 
	err_dt_unused,		// 9 - sémantická chyba nevyužité proměnné v jejím rozsahu platnosti; modifikovatelná proměnná bez možnosti změny po její inicializaci. 
	err_semantic,		// 10 - ostatní sémantické chyby. 
	err_internal=99 	// 99 - interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alokace paměti atd.). 
} err_codes;

/**
 * @brief Struktura pro chybová hlášení
 */
typedef struct Err_message {
	err_codes code; /**< Chybový kód */
	char *format;   /**< Formát chybového hlášení */
} Err_message;

/**
 * @brief Pole chybových hlášení
 */
extern const Err_message err_msg[];

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
size_t err_fetch(err_codes err);

/**
 * @brief Vypíše chybovou zprávu dle kódu chyby
 *
 * Funkce vypise na standardni vystup chybovou zpravy dle kody chyby.
 * Kdyz je kod chyby neplatny, vypise se chybova zprava pro kod err_internal.
 *
 * @param code Kód chyby
 */
void err_print(err_codes err, bool message_needed);

/**
 * @brief Ukončí překlad s chybovým hlášením lexical error
 *
 * Funkce vypise na standardni vystup chybovou zpravy v podobe
 * "lexical error: <what>" a potom ukonci preklad s kodem chyby
 * err_lexic.
 *
 * @param what Textovy retezec, ktery bude vypsany jako cast chybove zpravy.
 */
void exit_lexic(char * what);

/**
 * @brief Ukončí překlad s chybovým hlášením syntax error
 *
 * Funkce vypise na standardni vystup chybovou zpravy v podobe
 * "syntax error: syntax error or missing header within program file\n"
 * a potom ukonci preklad s kodem chyby err_syntax.
 */
void exit_syntax();

/**
 * @brief Ukončí překlad s chybovým hlášením undefined function or variable
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: undefined function or variable\n"
 * a potom ukonci preklad s kodem chyby err_undef.
 */
void exit_undef();

/**
 * @brief Ukončí překlad s chybovým hlášením o chybném počtu nebo typu parametrů
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: wrong number/type of parameters in function call; wrong type or non-permitted disposal of function return value.\n"
 * a potom ukonci preklad s kodem chyby err_param.
 */
void exit_param();

/**
 * @brief Ukončí překlad s chybovým hlášením o chybné redefinici proměnné nebo funkce
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: redefinition of variable/function; assigment to non-modifiable variable.\n"
 * a potom ukonci preklad s kodem chyby err_redef.
 */
void exit_redef();

/**
 * @brief Ukončí překlad s chybovým hlášením o chybejícím nebo přebývajícím výrazu v příkazu návratu z funkce
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: missing/extra expression in function return statement.\n"
 * a potom ukonci preklad s kodem chyby err_ret_val.
 */
void exit_ret_val();

/**
 * @brief Ukončí překlad s chybovým hlášením o chybějící nebo nekompatibilní type v aritmetických, řetězcových a relačních výrazech,
 * nebo o chybějícím nebo přebývajícím výrazu v příkazu návratu z funkce.
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: incompatible types in arithmetic, comparison or relational expressions; wrong type or non-permitted disposal of function return value.\n"
 * a potom ukonci preklad s kodem chyby err_dt_invalid.
 */
void exit_dt_invalid();

/**
 * @brief Ukončí překlad s chybovým hlášením o neznámém typu výrazu
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: could not deduce type from expression\n"
 * a potom ukonci preklad s kodem chyby err_dt_unknown.
 */
void exit_dt_unknown();

/**
 * @brief Ukončí překlad s chybovým hlášením o nevyužité proměnné v jejím rozsahu platnosti
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: unused variable in its scope.\n"
 * a potom ukonci preklad s kodem chyby err_dt_unused.
 */
void exit_dt_unused();

/**
 * @brief Ukončí překlad s chybovým hlášením jine sémantické chyby
 *
 * Funkce vypise na standardni vystup chybovou zpravu v podobe
 * "semantic error: other semantic errors\n"
 * a potom ukonci preklad s kodem chyby err_semantic.
 */
void exit_semantic();

/**
 * @brief Interní chyba překladače
 *
 * Funkce vypíše na standardní výstup chybové hlášení v podobě
 * "internal error: error on the level of compiler (program may be ok, but the compiler is not)\n"
 * a potom ukončí překlad s kódem chyby err_internal.
 */
void _exit_internal();

#endif /* ERROR_H */

/*** Konec souboru errors.h ***/