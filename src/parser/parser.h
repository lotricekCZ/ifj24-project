/** 
 * @addtogroup IFJ2024
 * @file parser.c
 * @brief Knihovna parseru
 * @author xondre16; Jozef Ondrejička
 * @author xsidlil00; Lukáš Šidlík
 * @author xhubacv00; Vojtech Hubacek
 * 
 * Knihovna obsahuje funkce implementované v parseru,
 * který se skládá ze syntaktické analýzy,
 * sémantických kontrol a generování kódu.
 */

#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "../utils/token_types.h"
#include "../scanner/scanner.h"
#include "../utils/token.h"
#include "../utils/symtable.h"
#include "../utils/errors.h"
#include "../scanner/scanner.h"
#include "../utils/token_types.h"
#include "../utils/codegen.h"
#include "../utils/stack.h"
#include "../utils/symtable.h"
#include "../utils/symDLList.h"
#include "../utils/str.h"
#include "../utils/dynamic_array.h"

/**
 * @enum context_t
 * @brief Výčet kontextů
 */
typedef enum {
    CONTEXT_CONDITION,
    CONTEXT_NONE,
    CONTEXT_SYMBOL,
    CONTEXT_RETURN,
    CONTEXT_CONDITION_FOR
}context_t;

/**
 * @struct parser_tools_t
 * @brief Struktura pro uchování nástrojů pro parser
 * 
 * Struktura obsahuje informace potřebné pro správný běh parseru, sémantické kontroly
 * a generování kódu napříč volání jednotlivých funkcí parseru.
 */
typedef struct {
    //Parser
    Scanner_ptr scanner;                            /**< Ukazatel na reprezentaci scanneru */
    Token_ptr current_token;                        /**< Aktuální token parsru */
    err_codes error;                                /**< Chybový kód */

    //Sémantika
    symtable_t* current_symtable;                   /**< Aktuální tabulka symbolů */
    int current_context;                            /**< Aktuální kontext */
    DLList sym_list;                                /**< Seznam tabulek symbolů */
    data_t *left_data;                              /**< Data levé strana příkazu */
    data_t *right_data;                             /**< Data pravé strany příkazu */
    data_t *result_data;                            /**< Výsledná data výrazu */
    data_t *function_data;                          /**< Návratová data z funkce */
    int depth;                                      /**< Hloubka vnoření */
    int param_count;                                /**< Počet parametrů */
    dynamic_array_t depth_sequence;                 /**< Záznam hloubek vnoření */
    char string_buffer_semantic[MAX_STRING_LEN];    /**< Pomocný buffer pro sémantické kontroly */
    bool params;                                    /**< Příznak, zda se jedná o parametry */
    bool is_left;                                   /**< Příznak, zda se jedná o levou stranu přiřazení */
    bool varOrFunc;                                 /**< Příznak, zda se jedná o proměnnou nebo funkci */

    //Generace kódu
    int counter_codegen_if;                         /**< Počítadlo pro generování kódu if */
    int counter_codegen_while;                      /**< Počítadlo pro generování kódu while */
    int counter_codegen_for;                        /**< Počítadlo pro generování kódu for */
    int counter_global;                             /**< Počítadlo pro generování jednoznačných identifikátorů */
    int cycle;                                      /**< Aktuální cyklus */            
    char string_buffer[MAX_STRING_LEN];             /**< Pomocný buffer identifikátorů */
    char string_buffer_value[MAX_STRING_LEN];       /**< Pomocný buffer identifikátorů hodnot pro generování kódu */
    Stack stack_codegen;                            /**< Zásobník tokenů pro generování kódu */
    str_t string;                                   /**< Řetězec s cílovým kódem */
    str_t string_tmp;                               /**< Pomocný řetězec cílového kódu */
    str_t string_defvar;                            /**< Řetězec s již vygenerovanými deklaracemi proměnných */
} parser_tools_t;

/**
 * @brief Funkce pro získání indexu tokenu v precedenční tabulce
 * 
 * @param type Typ tokenu
 * @return Index elementu v precedenční tabulce
 */
int get_precedence_index(token_type type);

/**
 * @brief Zpracování precedenční analýzy
 * 
 * Funkce zpracovává precedenční analýzu výrazu za použití precedenční tabulky.
 * 
 * @param tools Ukazatel na strukturu parser_tools_t
 * @param precedence Ukazatel na dynamické pole
 * @param input Typ tokenu
 */
void precedence_analysis(parser_tools_t* tools, dynamic_array_t* precedence, token_type input);

/**
 * @brief Syntaktické zpracování výrazu a vytváření postfixové notace
 * 
 * Funkce zpracovává jednotlivé tokeny výrazu za použití precedenční analýzy
 * a zároveň vytváří postfixovou notaci výrazu pro náslendé zpracování
 * sémantickou analýzou a generátorem cílového kódu.
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 * @param postfix Ukazatel na pole tokenů
 * @param postfix_index Aktuální index pole tokenů
 * @param stack_functions Ukazatel na zásobník funkcí
 */
void expression_processing(parser_tools_t* tools, Token_ptr (*postfix)[MAX], int* postfix_index, dynamic_array_t* functions_retval);

/**
 * @brief Zpraocvání výrazu
 * 
 * Hlavní funkce řídící zpracování výrazu, sémantickou kontrolu
 * a jeho následné vygenerování do cílového kódu.
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void expression(parser_tools_t* tools);

/**
 * @brief Kontrola redefinece proměnné
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void check_redefinition(parser_tools_t* tools);

/**
 * @brief Načítání tokenů prvního průchodu parseru
 * 
 * Funkce načítá tokeny ze scanneru.
 * Uvolňuje paměť po předchozím tokenu při počáteční načtení.
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void next_token_initial(parser_tools_t* tools);

/**
 * @brief Načítání tokenů parseru při druhém průchodu
 * 
 * Funkce pouze načítá tokeny ze scanneru.
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void next_token(parser_tools_t* tools);

/**
 * @brief Kontrolu typu tokenu
 * 
 * Funkce kontroluje, zda je aktuální token jednoho z očekávaných typů.
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 * @param count Počet očekávaných typů
 * @param ... Očekávané typy
 */
void expect_types(parser_tools_t* tools, int count, ...);

/**
 * @brief Počáteční funkce rekurzivního sestupu
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void program(parser_tools_t* tools);

/**
 * @brief Zpracování prologu programu
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void prolog(parser_tools_t* tools);

/**
 * @brief Zpracování funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void function(parser_tools_t* tools);

/**
 * @brief Zpracování dalších případných funkcí
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void function_next(parser_tools_t* tools);

/**
 * @brief Zpracování parametrů funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void parameter(parser_tools_t* tools);

/**
 * @brief Zpracování dalších případných parametrů funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void parameter_next(parser_tools_t* tools);

/**
 * @brief Zpracování těla funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru 
 */
void body(parser_tools_t* tools);

/**
 * @brief Zpracování jednotlivých příkazů
 * 
 * Zpracovávají se jednotlivé příkazy těla funkce na základě aktuálního tokenu.
 * Příkazy funkce, které se vyskytují v cyklech mají další stupeň ověření.
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void statement(parser_tools_t* tools);

/**
 * @brief Pomocná funkce zpracování příkazu if
 * 
 * @param tools Ukazatel na strukturu parser_tools_t
 */
void statement_if(parser_tools_t* tools);

/**
 * @brief Pomocná funkce zpracování příkazu while
 * 
 * @param tools Ukazatel na strukturu parser_tools_t
 */
void statement_while(parser_tools_t* tools);

/**
 * @brief Pomocná funkce zpracování příkazu for
 * 
 * @param tools Ukazatel na strukturu parser_tools_t
 */
void statement_for(parser_tools_t* tools);

/**
 * @brief Zpracování příkazu počínající symbolem
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void id_statement(parser_tools_t* tools);

/**
 * @brief Zpracování hodnoty
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void value(parser_tools_t* tools);

/**
 * @brief Přiřazení hodnoty výrazu podmíněného neprázdnou hodnotou
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void not_null_value(parser_tools_t* tools);

/**
 * @brief Zpracování těla cyklů a podmínek
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void then(parser_tools_t* tools);

/**
 * @brief Zpracování těla else
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void else_then(parser_tools_t* tools);

/**
 * @brief Zpracování symbolu
 * 
 * Rozhoduje, zda se jedná o volání funkce nebo proměnnou.
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void id_continue(parser_tools_t* tools);

/**
 * @brief Zpracování návratové hodnoty funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void return_value(parser_tools_t* tools);

/**
 * @brief Zpracování volání funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void call(parser_tools_t* tools);

/**
 * @brief Zpracování parametrů volání funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void call_params(parser_tools_t* tools);

/**
 * @brief Zpracování dalších parametrů volání funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void call_params_next(parser_tools_t* tools);

/**
 * @brief Zpracování hodnoty parametru
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void call_value(parser_tools_t* tools);

/**
 * @brief Zpracování návratového typu funkce
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void return_type(parser_tools_t* tools);

/**
 * @brief Zpracování typu symbolu
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void type(parser_tools_t* tools);

/**
 * @brief Zpracování typu proměnné
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void definition(parser_tools_t* tools);

/**
 * @brief Zpracování hodnoty pro for cyklus
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void for_value(parser_tools_t* tools);

/**
 * @brief První průchod parseru zdrojovým kódem
 * 
 * Funkce prochází zdrojový kód a vytváří tabulku symbolů pro funkce.
 *  
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void parse_fn_first(parser_tools_t* tools);

/**
 * @brief Inicializace struktury parser_tools_t
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void parser_tools_init(parser_tools_t* tools);

/**
 * @brief Uvolnění struktury parser_tools_t
 * 
 * @param tools Ukazatel na strukturu s nástroji pro fungování parseru
 */
void parser_tools_destroy(parser_tools_t* tools);

/**
 * @brief Hlavní funkce parseru
 * 
 * Parser má celkově dva režimy.
 * Při prvním průchodu přidá do tabulky symbolů všechny funkce a proměnné.
 * Při druhém průchodu se provádí samotná syntaktická analýza.
 * 
 * @param scanner Ukazatel na reprezentaci scanner
 * @return Návratový kód chyby během parsování
 */
err_codes parse(Scanner_ptr scanner);

#endif // PARSER_H

/*** Konec souboru parser.h ***/