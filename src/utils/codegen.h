/**
 * @addtogroup IFJ2024
 * @file codegen.h
 * @brief Kihuvna pro generování instrukcí strojového kódu
 * @author xhubacv00; Vojtěch Hubáček
 * 
 * Knihovna obsahuje výčet instrukcí strojového kódu a pomocné funkce pro generování instrukcí
 */

#ifndef CODEGEN_H
#define CODEGEN_H

#include "token.h"
#include "str.h"
#include "stack.h"
#include "symtable.h"
#include "symDLList.h"
#include "errors.h"

/**
 * @brief Maximální délka pomocný řetězců potřebných pro generování instrukcí
 */
#define MAX_STRING_LEN 1000

/**
 * @brief Počet instrukcí strojového kódu
 */
#define INSTRUCTION_COUNT 57

/**
 * @enum instruction
 * @brief Výčet instrukcí strojového kódu
 */
typedef enum {
    _move,
    _createframe,
    _pushframe,
    _popframe,
    _defvar,
    _call,
    _return,
    _pushs,
    _pops,
    _clears,
    _add,
    _sub,
    _mul,
    _div,
    _idiv,
    _adds,
    _subs,
    _muls,
    _divs,
    _idivs,
    _lt,
    _gt,
    _eq,
    _lts,
    _gts,
    _eqs,
    _and,
    _or,
    _not,
    _ands,
    _ors,
    _nots,
    _int2float,
    _float2int,
    _int2char,
    _stri2int,
    _int2floats,
    _float2ints,
    _int2chars,
    _stri2ints,
    _read,
    _write,
    _concat,
    _strlen,
    _getchar,
    _setchar,
    _type,
    _label,
    _jump,
    _jumpifeq,
    _jumpifneq,
    _jumpifeqs,
    _jumpifneqs,
    _exit,
    _break,
    _dprint,
    _comment
} instruction;

/**
 * @brief Pole formátů instrukcí strojového kódu
 */
extern const char* format[INSTRUCTION_COUNT];

/**
 * @brief Funkce pro výpis instrukce strojového kódu
 * 
 * Funkce pro výpis řetězce s ohledem na speciální znaky a escape sekvence v závislosti na typu tokenu.
 * 
 * @param string Ukazatel na strukturu řetězce s generovaným kódem
 * @param source Vypisovaný řetězec
 * @param type Typ tokenu řetězce
 */
void printi_string(str_t* string, char* input_string, token_type type);

/**
 * @brief Výpis instrukcí strojového kódu vyhodnocení výrazu
 * 
 * Funkce pro výpis posloupnosti instrukcí vyhodnocení výrazu na zásobníku v postfixové notaci.
 * 
 * @param string Ukazatel na strukturu řetězce s generovaným kódem
 * @param postfix Pole tokenů v postfixové notaci
 * @param postfix_index Index posledního prvku v poli
 * @param stack Ukazatel na zásobník s názvy generovaných proměnných s hodnotymi návratů funkcí
 * @param sym_list Ukazatel na seznam tabulek symbolů
 * @param symtable Ukazatel na tabulku symbolů
 * @param error Ukazatel na chybový kód
 */
void printi_postfix(str_t* string, Token_ptr *postfix, int postfix_index, dynamic_array_t* functions_retval, DLList* sym_list, symtable_t* symtable, err_codes *error);

/**
 * @brief Výpis podmíněného skoku
 * 
 * Funkce pro přednostní výpis instrukcí definicí proměnných, pokud ještě nebyly definovány.
 * 
 * @param string Ukazatel na strukturu řetězce s generovaným kódem
 * @param instruction Typ instrukce
 * @param ... Argumenty pro formátovaný řetězec
 */
void printi_defvar(str_t* string, str_t* defvar, const char* source, ...);

/**
 * @brief Výpis posloupnosti instrukcí vestavěných funkcí a pomocných funkcí vyhodnocení výrazu.
 * 
 * Funkce pro výpis posloupnosti instrukcí pro vyhodnocení podmínky skoku v programu.
 * 
 * @param string Ukazatel na strukturu řetězce s generovaným kódem
 * @param name Název návěští
 * @param number Číslo návěští
 */
void printi_condition_jump(str_t* string, const char *name, int number);

/**
 * @brief Výpis posloupnosti instrukcí vestavěných funkcí a pomocných funkcí vyhodnocení výrazu.
 * 
 * Funkce pro výpis posloupnosti instrukcí vestavěných funkcí a pomocných funkcí vyhodnocení výrazu.
 * 
 * @param string Ukazatel na strukturu řetězce s generovaným kódem
 */
void printi_builtin(str_t* string);

#endif /* CODEGEN_H */

/*** Konec souboru codegen.h ***/