/**
 * @addtogroup IFJ2024
 * @file codegen.c
 * @brief Implementace zpracování instrukcí strojového kódu
 * @author xhubacv00; Vojtěch Hubáček
 * 
 * Implementace funkcí pro zpracování instrukcí strojového kódu.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "codegen.h"

// Pole formátů instrukcí strojového kódu.
const char *format[INSTRUCTION_COUNT] = {
    "MOVE %s %s\n",         // MOVE ⟨var⟩ ⟨symb⟩
    "CREATEFRAME\n",        // CREATEFRAME
    "PUSHFRAME\n",          // PUSHFRAME
    "POPFRAME\n",           // POPFRAME
    "DEFVAR %s\n",          // DEFVAR ⟨var⟩
    "CALL %s\n",            // CALL ⟨label⟩
    "RETURN\n",             // RETURN
    "PUSHS %s\n",           // PUSHS ⟨symb⟩
    "POPS %s\n",            // POPS ⟨var⟩
    "CLEARS\n",             // CLEARS
    "ADD %s %s %s\n",       // ADD ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "SUB %s %s %s\n",       // ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "MUL %s %s %s\n",       // ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "DIV %s %s %s\n",       // ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "IDIV %s %s %s\n",      // ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "ADDS\n",               // ADDS
    "SUBS\n",               // SUBS
    "MULS\n",               // MULS
    "DIVS\n",               // DIVS
    "IDIVS %s\n",           // IDIVS ⟨var⟩
    "LT %s %s %s\n",        // LT ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "GT %s %s %s\n",        // GT ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "EQ %s %s %s\n",        // EQ ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "LTS\n",                // LTS
    "GTS\n",                // GTS
    "EQS\n",                // EQS
    "AND %s %s %s\n",       // AND ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "OR %s %s %s\n",        // OR ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "NOT %s %s %s\n",       // NOT ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "ANDS\n",               // ANDS
    "ORS\n",                // ORS
    "NOTS\n",               // NOTS
    "INT2FLOAT %s %s\n",    // INT2FLOAT ⟨var⟩ ⟨symb⟩
    "FLOAT2INT %s %s\n",    // FLOAT2INT ⟨var⟩ ⟨symb⟩
    "INT2CHAR %s %s\n",     // INT2CHAR ⟨var⟩ ⟨symb⟩
    "STRI2INT %s %s %s\n",  // STRI2INT ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "INT2FLOATS %s\n",      // INT2FLOATS ⟨var⟩
    "FLOAT2INTS %s\n",      // FLOAT2INTS ⟨var⟩
    "INT2CHARS %s\n",       // INT2CHARS ⟨var⟩
    "STRI2INTS %s\n",       // STRI2INTS ⟨var⟩
    "READ %s %s\n",         // READ ⟨var⟩ ⟨type⟩
    "WRITE %s\n",           // WRITE ⟨symb⟩
    "CONCAT %s %s %s\n",    // CONCAT ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "STRLEN %s %s\n",       // STRLEN ⟨var⟩ ⟨symb⟩
    "GETCHAR %s %s %s\n",   // GETCHAR ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "SETCHAR %s %s %s\n",   // SETCHAR ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "TYPE %s %s\n",         // TYPE ⟨var⟩ ⟨symb⟩
    "LABEL %s\n",           // LABEL ⟨label⟩
    "JUMP %s\n",            // JUMP ⟨label⟩
    "JUMPIFEQ %s %s %s\n",  // JUMPIFEQ ⟨label⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "JUMPIFNEQ %s %s %s\n", // JUMPIFNEQ ⟨label⟩ ⟨symb 1⟩ ⟨symb 2⟩
    "JUMPIFEQS %s\n",       // JUMPIFEQS ⟨label⟩
    "JUMPIFNEQS %s\n",      // JUMPIFNEQS ⟨label⟩
    "EXIT %s\n",            // EXIT ⟨symb⟩
    "BREAK\n",              // BREAK
    "DPRINT %s\n",          // DPRINT ⟨symb⟩
    "# %s\n"                // # ⟨string⟩
};

// Funkce pro výpis řetězce s ohledem na speciální znaky a escape sekvence v závislosti na typu tokenu.
void printi_string(str_t* string, char* source, token_type type) {
    for (int index = 0; index < strlen(source); index++) {
        if (type == tok_t_str) {

            // Převod speciálních sekvencí na odpovídající znaky pro výpis
            if (source[index] == 92 && source[index + 1] == 'x' && index + 3 < strlen(source)) { // Detekce sekvence \x následované 2 hex číslicemi
                if (isxdigit(source[index + 2]) && isxdigit(source[index + 3])) {
                    char high = source[index + 2];
                    char low = source[index + 3];

                    int value;
                    if (isdigit(high))
                        value = (high - '0') * 16;
                    else
                        value = (toupper(high) - 'A' + 10) * 16;

                    if (isdigit(low))
                        value += low - '0';
                    else
                        value += toupper(low) - 'A' + 10;

                    source[index + 3] = (char)value; // Uložení výsledné hodnoty
                    index += 3; // Přeskočení zpracovaných znaků
                }
            } else if (source[index] == '\\' && source[index + 1] == 'n' && index + 1 < strlen(source)) { // Detekce sekvence \n
                source[index + 1] = (char)10;
                index++;
            } else if (source[index] == '\\' && source[index + 1] == 't' && index + 1 < strlen(source)) { // Detekce sekvence \t
                source[index + 1] = (char)9;
                index++;
            } else if (source[index] == '\\' && source[index + 1] == '\\' && index + 1 < strlen(source)) { // Detekce sekvence \\    
                source[index + 1] = (char)92;
                index++;
            } else if (source[index] == '\\' && source[index + 1] == '\"' && index + 1 < strlen(source)) {
                source[index + 1] = (char)34;
                index++;
            }
        }

        // Výpis znaku v závislosti na jeho hodnotě (vybrané znaky jsou vypisovány jako sekvence \xxx)
        if ((source[index] >= '\0' && source[index] <= ' ') || source[index] == '#' || source[index] == '\\') {
            str_append(string, "\\%03d", source[index]);
        } else {
            str_append(string, "%c", source[index]);
        }
    }
}

// Funkce pro výpis posloupnosti instrukcí vyhodnocení výrazu na zásobníku v postfixové notaci.
void printi_postfix(str_t* string, Token_ptr *postfix, int postfix_index, dynamic_array_t* functions_retval, DLList* sym_list, symtable_t* symtable, err_codes *error) {
    int functions_retval_index = 0;
    for (size_t index = 0; index < postfix_index; index++) {
        char buffer[MAX_STRING_LEN]; // Pomocný buffer pro výpis generovaných řetězců
        switch (postfix[index]->type) {
            case tok_t_null: // null
                str_append(string, format[_pushs], "nil@nil");
                break;
            case tok_t_int: // int
                sprintf(buffer, "int@%i", atoi(postfix[index]->attribute));
                str_append(string, format[_pushs], buffer);
                break;
            case tok_t_flt: // float
                sprintf(buffer, "float@%a", atof(postfix[index]->attribute));
                str_append(string, format[_pushs], buffer);
                break;
            case tok_t_true: // true
                str_append(string, format[_pushs], "bool@true");
                break;
            case tok_t_false: // false
                str_append(string, format[_pushs], "bool@false");
                break;
            case tok_t_unreach: // unreached
                str_append(string, format[_pushs], "string@panic:\\032reached\\032unreachable\\032code\n");
                break;
            case tok_t_sym: // symbol
                DLL_First(sym_list);
                symtable = DLL_GetCurrent(sym_list);

                // Vyhledání proměnné v tabulce symbolů
                data_t* data = symtable_get_item(symtable, postfix[index]->attribute, error);
                if(*error != err_none) {
                    return;
                }
                if (data != NULL) { // Pokud je symbol nalezen jedná se o funkci a její návratová hodnota je přidána na zásobník
                    sprintf(buffer, "LF@%%retval%i", functions_retval->data[functions_retval_index++]);
                    str_append(string, format[_pushs], buffer);
                } else { 
                    DLL_Last(sym_list);
                    symtable = DLL_GetCurrent(sym_list);
                    while(sym_list->current != sym_list->first){ // Vyhledání proměnné v mezi tabulkami symbolů
                        data = symtable_get_item(symtable, postfix[index]->attribute, error);
                            if(*error != err_none)
                                return;
                        if (data != NULL) { // Přidání hodnoty dané proměnné na zásobník
                            sprintf(buffer, "LF@%s", data->id);
                            str_append(string, format[_pushs], buffer);
                            break;
                        } else { // Přechod na tabulku symbolů vyšší úrovně
                            DLL_Prev(sym_list);
                            symtable = DLL_GetCurrent(sym_list);
                        }
                    }
                }
                DLL_Last(sym_list);
                break;
            case tok_t_plus: // +
                str_append(string, format[_call], "$$$retype");
                str_append(string, "%s", format[_adds]);
                break;
            case tok_t_minus: // -
                str_append(string, format[_call], "$$$retype");
                str_append(string, "%s", format[_subs]);
                break;
            case tok_t_times: // *
                str_append(string, format[_call], "$$$retype");
                str_append(string, "%s", format[_muls]);
                break;
            case tok_t_divide: // /
                str_append(string, format[_call], "$$$divs");
                break;
            case tok_t_eq: // ==
                str_append(string, format[_call], "$$$retype");
                str_append(string, format[_call], "$$$null");
                str_append(string, "%s", format[_eqs]);
                break;
            case tok_t_neq: // !=
                str_append(string, format[_call], "$$$retype");
                str_append(string, format[_call], "$$$null");
                str_append(string, "%s", format[_eqs]);
                str_append(string, "%s", format[_nots]);
                break;
            case tok_t_lt: // <
                str_append(string, format[_call], "$$$retype");
                str_append(string, "%s", format[_lts]);
                break;
            case tok_t_gt: // >
                str_append(string, format[_call], "$$$retype");
                str_append(string, "%s", format[_gts]);
                break;
            case tok_t_leq: // <=
                str_append(string, format[_call], "$$$retype");
                str_append(string, "%s", format[_gts]);
                str_append(string, "%s", format[_nots]);
                break;
            case tok_t_geq: // >=
                str_append(string, format[_call], "$$$retype");
                str_append(string, "%s", format[_lts]);
                str_append(string, "%s", format[_nots]);
                break;
            case tok_t_and: // and
                str_append(string, "%s", format[_ands]);
                break;
            case tok_t_or: // or
                str_append(string, "%s", format[_ors]);
                break;
            case tok_t_not: // !
                str_append(string, "%s", format[_nots]);
                break;
            case tok_t_orelse: // orelse
                str_append(string, format[_call], "$$$orelse");
                break;
            case tok_t_orelse_un: // .?
                str_append(string, format[_pushs], "string@panic:\\032reached\\032unreachable\\032code\n");
                str_append(string, format[_call], "$$$orelse");
                break;
            default:
                break;
        }
    }
}

// Funkce pro přednostní výpis instrukcí definicí proměnných, pokud ještě nebyly definovány.
void printi_defvar(str_t* string, str_t* defvar, const char* source, ...) {
    char definition[MAX_STRING_LEN + 30]; // Pomocný buffer pro výpis generovaných řetězců
    char definition_search[MAX_STRING_LEN + 35]; // Pomocný buffer pro vyhledávání již definovaných proměnných
    va_list args;
    va_start(args, source);
    vsprintf(definition, source, args);
    va_end(args);
    sprintf(definition_search, ",%s,", definition);
    if (!str_search(defvar, definition_search)) {
        str_append(defvar, "%s", definition_search);
        str_append(string, "%s", definition);
    }
}

// Funkce pro výpis posloupnosti instrukcí pro vyhodnocení podmínky skoku v programu.
void printi_condition_jump(str_t* string, const char *name, int number) {
    str_append(string, "\
CREATEFRAME\n\
DEFVAR TF@%%type\n\
TYPE TF@%%type LF@%%%s%i\n\
JUMPIFEQ !$%s%i TF@%%type string@nil\n\
JUMPIFNEQ $$$$%s%i TF@%%type string@bool\n\
JUMPIFEQ !$%s%i LF@%%%s%i bool@false\n\
LABEL $$$$%s%i\n", name, number, name, number, name, number, name, number, name, number, name, number);
}

// Funkce pro výpis posloupnosti instrukcí vestavěných funkcí a pomocných funkcí vyhodnocení výrazu.
void printi_builtin(str_t* string) {
    // Pomocná funkce pro přetypování hodnoty
    str_append(string, "\
LABEL $$$retype\n\
CREATEFRAME\n\
PUSHFRAME\n\
CREATEFRAME\n\
DEFVAR TF@%%1\n\
POPS TF@%%1\n\
DEFVAR TF@%%type1\n\
TYPE TF@%%type1 TF@%%1\n\
DEFVAR TF@%%0\n\
POPS TF@%%0\n\
DEFVAR TF@%%type0\n\
TYPE TF@%%type0 TF@%%0\n\
JUMPIFEQ $$$$retype_ok1 TF@%%type1 string@float\n\
JUMPIFEQ $$$$retype_ok1 TF@%%type1 string@int\n\
JUMP $$$$retype_end\n\
LABEL $$$$retype_ok1\n\
JUMPIFNEQ $$$$retype_ok2 TF@%%type0 string@float\n\
JUMPIFNEQ $$$$retype_ok2 TF@%%type0 string@int\n\
JUMP $$$$retype_end\n\
LABEL $$$$retype_ok2\n\
JUMPIFNEQ $$$$retype_uncompatible TF@%%type0 TF@%%type1\n\
JUMP $$$$retype_end\n\
LABEL $$$$retype_uncompatible\n\
JUMPIFEQ $$$$retype_float TF@%%type0 string@float\n\
INT2FLOAT TF@%%0 TF@%%0\n\
JUMP $$$$retype_end\n\
LABEL $$$$retype_float\n\
INT2FLOAT TF@%%1 TF@%%1\n\
LABEL $$$$retype_end\n\
PUSHS TF@%%0\n\
PUSHS TF@%%1\n\
POPFRAME\n\
RETURN\n");

    // Pomocná funkce pro vyhodnocení nulových hodnot
    str_append(string, "\
LABEL $$$null\n\
CREATEFRAME\n\
PUSHFRAME\n\
CREATEFRAME\n\
DEFVAR TF@%%1\n\
POPS TF@%%1\n\
DEFVAR TF@%%type1\n\
TYPE TF@%%type1 TF@%%1\n\
DEFVAR TF@%%0\n\
POPS TF@%%0\n\
DEFVAR TF@%%type0\n\
TYPE TF@%%type0 TF@%%0\n\
JUMPIFEQ $$$$null_yes TF@%%type1 string@nil\n\
JUMPIFEQ $$$$null_yes TF@%%type0 string@nil\n\
JUMP $$$$null_no\n\
LABEL $$$$null_yes\n\
MOVE TF@%%0 TF@%%type0\n\
MOVE TF@%%1 TF@%%type1\n\
LABEL $$$$null_no\n\
PUSHS TF@%%0\n\
PUSHS TF@%%1\n\
POPFRAME\n\
RETURN\n");

    // Pomocná funkce pro dělení
   str_append(string, "\
LABEL $$$divs\n\
CREATEFRAME\n\
PUSHFRAME\n\
CREATEFRAME\n\
DEFVAR TF@%%1\n\
POPS TF@%%1\n\
DEFVAR TF@%%type1\n\
TYPE TF@%%type1 TF@%%1\n\
DEFVAR TF@%%0\n\
POPS TF@%%0\n\
DEFVAR TF@%%type0\n\
TYPE TF@%%type0 TF@%%0\n\
PUSHS TF@%%0\n\
PUSHS TF@%%1\n\
JUMPIFNEQ $$$$divs_error TF@%%type1 TF@%%type0\n\
JUMPIFEQ $$$$divs_int TF@%%type1 string@int\n\
DIVS\n\
JUMP $$$$divs_end\n\
LABEL $$$$divs_int\n\
IDIVS\n\
JUMP $$$$divs_end\n\
LABEL $$$$divs_error\n\
EXIT int@57\n\
LABEL $$$$divs_end\n\
POPFRAME\n\
RETURN\n");

    // Pomocná funkce pro operátor orelse
    str_append(string, "\
LABEL $$$orelse\n\
CREATEFRAME\n\
PUSHFRAME\n\
CREATEFRAME\n\
DEFVAR TF@%%1\n\
POPS TF@%%1\n\
DEFVAR TF@%%0\n\
POPS TF@%%0\n\
DEFVAR TF@%%type\n\
TYPE TF@%%type TF@%%0\n\
JUMPIFEQ $$$$orelse_nil TF@%%type string@nil\n\
PUSHS TF@%%0\n\
JUMP $$$$orelse_end\n\
LABEL $$$$orelse_nil\n\
TYPE TF@%%type TF@%%1\n\
JUMPIFEQ $$$$orelse_unreachable TF@%%type string@string\n\
JUMP $$$$orelse_end2\n\
LABEL $$$$orelse_unreachable\n\
JUMPIFNEQ $$$$orelse_end2 TF@%%1 string@panic:\\032reached\\032unreachable\\032code\n\
WRITE string@panic:\\032reached\\032unreachable\\032code\n\
EXIT int@57\n\
LABEL $$$$orelse_end2\n\
PUSHS TF@%%1\n\
LABEL $$$$orelse_end\n\
POPFRAME\n\
RETURN\n");

    // Vestavěná funkce ifj.readstr
    str_append(string, "\
LABEL $$$readstr\n\
PUSHFRAME\n\
DEFVAR LF@%%retval\n\
READ LF@%%retval string\n\
POPFRAME\n\
RETURN\n");
    
    // Vestavěná funkce ifj.readi32
    str_append(string, "\
LABEL $$$readi32\n\
PUSHFRAME\n\
DEFVAR LF@%%retval\n\
READ LF@%%retval int\n\
POPFRAME\n\
RETURN\n");

    // Vestavěná funkce ifj.readf64
    str_append(string, "\
LABEL $$$readf64\n\
PUSHFRAME\n\
DEFVAR LF@%%retval\n\
READ LF@%%retval float\n\
POPFRAME\n\
RETURN\n");
    
    // Vestavěná funkce ifj.write
    str_append(string, "\
LABEL $$$write\n\
PUSHFRAME\n\
DEFVAR LF@%%0\n\
POPS LF@%%0\n\
WRITE LF@%%0\n\
POPFRAME\n\
RETURN\n");

    // Vestavěná funkce ifj.i2f
    str_append(string, "\
LABEL $$$i2f\n\
PUSHFRAME\n\
DEFVAR LF@%%0\n\
POPS LF@%%0\n\
DEFVAR LF@%%retval\n\
DEFVAR LF@%%type\n\
TYPE LF@%%type LF@%%0\n\
JUMPIFEQ $$$$i2f_int LF@%%type string@int\n\
JUMPIFEQ $$$$i2f_float LF@%%type string@float\n\
MOVE LF@%%retval nil@nil\n\
EXIT int@4\n\
JUMP $$$$i2f_end\n\
LABEL $$$$i2f_float\n\
MOVE LF@%%retval LF@%%0\n\
JUMP $$$$i2f_end\n\
LABEL $$$$i2f_int\n\
INT2FLOAT LF@%%retval LF@%%0\n\
LABEL $$$$i2f_end\n\
POPFRAME\n\
RETURN\n");
    
    // Vestaevná funkce ifj.f2i
    str_append(string, "\
LABEL $$$f2i\n\
PUSHFRAME\n\
DEFVAR LF@%%0\n\
POPS LF@%%0\n\
DEFVAR LF@%%retval\n\
DEFVAR LF@%%type\n\
TYPE LF@%%type LF@%%0\n\
JUMPIFEQ $$$$f2i_float LF@%%type string@float\n\
JUMPIFEQ $$$$f2i_int LF@%%type string@int\n\
MOVE LF@%%retval nil@nil\n\
EXIT int@4\n\
JUMP $$$$f2i_end\n\
LABEL $$$$f2i_int\n\
MOVE LF@%%retval LF@%%0\n\
JUMP $$$$f2i_end\n\
LABEL $$$$f2i_float\n\
FLOAT2INT LF@%%retval LF@%%0\n\
LABEL $$$$f2i_end\n\
POPFRAME\n\
RETURN\n");

    // Vestavěná funkce ifj.string
    str_append(string, "\
LABEL $$$string\n\
PUSHFRAME\n\
DEFVAR LF@%%0\n\
POPS LF@%%0\n\
DEFVAR LF@%%retval\n\
DEFVAR LF@%%type\n\
TYPE LF@%%type LF@%%0\n\
JUMPIFNEQ $$$$string_other LF@%%type string@string\n\
MOVE LF@%%retval LF@%%0\n\
JUMP $$$$string_end\n\
LABEL $$$$string_other\n\
MOVE LF@%%retval nil@nil\n\
EXIT int@4\n\
LABEL $$$$string_end\n\
POPFRAME\n\
RETURN\n");

    // Vestavěná funkce ifj.length
    str_append(string, "\
LABEL $$$length\n\
PUSHFRAME\n\
DEFVAR LF@%%0\n\
POPS LF@%%0\n\
DEFVAR LF@%%retval\n\
DEFVAR LF@%%type\n\
TYPE LF@%%type LF@%%0\n\
JUMPIFNEQ $$$$length_other LF@%%type string@string\n\
STRLEN LF@%%retval LF@%%0\n\
JUMP $$$$length_end\n\
LABEL $$$$length_other\n\
MOVE LF@%%retval nil@nil\n\
EXIT int@4\n\
LABEL $$$$length_end\n\
POPFRAME\n\
RETURN\n");

    // Vestavěná funkce ifj.concat
    str_append(string, "\
LABEL $$$concat\n\
PUSHFRAME\n\
DEFVAR LF@%%0\n\
POPS LF@%%0\n\
DEFVAR LF@%%1\n\
POPS LF@%%1\n\
DEFVAR LF@%%retval\n\
DEFVAR LF@%%type\n\
DEFVAR LF@%%len\n\
TYPE LF@%%type LF@%%0\n\
JUMPIFNEQ $$$$concat_other LF@%%type string@string\n\
TYPE LF@%%type LF@%%1\n\
JUMPIFNEQ $$$$concat_other LF@%%type string@string\n\
CONCAT LF@%%retval LF@%%0 LF@%%1\n\
JUMP $$$$concat_end\n\
LABEL $$$$concat_other\n\
PUSHS nil@nil\n\
EXIT int@4\n\
LABEL $$$$concat_end\n\
POPFRAME\n\
RETURN\n");
    
    // Vestavěná funkce ifj.substring
    str_append(string, "\
LABEL $$$substring\n\
PUSHFRAME\n\
DEFVAR LF@%%0\n\
POPS LF@%%0\n\
DEFVAR LF@%%1\n\
POPS LF@%%1\n\
DEFVAR LF@%%2\n\
POPS LF@%%2\n\
DEFVAR LF@%%retval\n\
DEFVAR LF@%%type\n\
TYPE LF@%%type LF@%%0\n\
JUMPIFNEQ $$$$substring_other LF@%%type string@string\n\
TYPE LF@%%type LF@%%1\n\
JUMPIFEQ $$$$substring_int1 LF@%%type string@int\n\
JUMPIFNEQ $$$$substring_other LF@%%type string@float\n\
FLOAT2INT LF@%%1 LF@%%1\n\
LABEL $$$$substring_int1\n\
TYPE LF@%%type LF@%%2\n\
JUMPIFEQ $$$$substring_int2 LF@%%type string@int\n\
JUMPIFNEQ $$$$substring_other LF@%%type string@float\n\
FLOAT2INT LF@%%2 LF@%%2\n\
LABEL $$$$substring_int2\n\
DEFVAR LF@%%len\n\
STRLEN LF@%%len LF@%%0\n\
DEFVAR LF@%%cond\n\
LT LF@%%cond LF@%%1 int@0\n\
JUMPIFEQ $$$$substring_null LF@%%cond bool@true\n\
LT LF@%%cond LF@%%2 int@0\n\
JUMPIFEQ $$$$substring_null LF@%%cond bool@true\n\
GT LF@%%cond LF@%%1 LF@%%2\n\
JUMPIFEQ $$$$substring_null LF@%%cond bool@true\n\
LT LF@%%cond LF@%%1 LF@%%len\n\
JUMPIFEQ $$$$substring_null LF@%%cond bool@false\n\
GT LF@%%cond LF@%%2 LF@%%len\n\
JUMPIFEQ $$$$substring_null LF@%%cond bool@true\n\
MOVE LF@%%retval string@\n\
DEFVAR LF@%%tmp\n\
LABEL $$$$substring_while_start\n\
JUMPIFEQ $$$$substring_while_end LF@%%1 LF@%%2\n\
GETCHAR LF@%%tmp LF@%%0 LF@%%1\n\
CONCAT LF@%%retval LF@%%retval LF@%%tmp\n\
ADD LF@%%1 LF@%%1 int@1\n\
JUMP $$$$substring_while_start\n\
LABEL $$$$substring_while_end\n\
JUMP $$$$substring_end\n\
LABEL $$$$substring_other\n\
MOVE LF@%%retval nil@nil\n\
EXIT int@4\n\
JUMP $$$$substring_end\n\
LABEL $$$$substring_null\n\
MOVE LF@%%retval nil@nil\n\
LABEL $$$$substring_end\n\
POPFRAME\n\
RETURN\n");

    // Vestavěná funkce ifj.strcmp
    str_append(string, "\
LABEL $$$strcmp\n\
PUSHFRAME\n\
DEFVAR LF@%%1\n\
POPS LF@%%1\n\
DEFVAR LF@%%0\n\
POPS LF@%%0\n\
DEFVAR LF@%%retval\n\
DEFVAR LF@%%type\n\
TYPE LF@%%type LF@%%0\n\
JUMPIFNEQ $$$$strcmp_other LF@%%type string@string\n\
TYPE LF@%%type LF@%%1\n\
JUMPIFNEQ $$$$strcmp_other LF@%%type string@string\n\
DEFVAR LF@%%len1\n\
DEFVAR LF@%%len2\n\
STRLEN LF@%%len1 LF@%%0\n\
STRLEN LF@%%len2 LF@%%1\n\
DEFVAR LF@%%cond\n\
LT LF@%%cond LF@%%len1 LF@%%len2\n\
JUMPIFEQ $$$$strcmp_len LF@%%cond bool@true\n\
MOVE LF@%%len1 LF@%%len2\n\
LABEL $$$$strcmp_len\n\
DEFVAR LF@tmp1\n\
DEFVAR LF@tmp1_counter\n\
DEFVAR LF@tmp2\n\
DEFVAR LF@tmp_counter\n\
MOVE LF@tmp_counter int@0\n\
LABEL $$$$strcmp_while_start\n\
JUMPIFEQ $$$$strcmp_eq LF@%%len1 int@0\n\
SUB LF@%%len1 LF@%%len1 int@1\n\
STRI2INT LF@tmp1 LF@%%0 LF@tmp_counter\n\
STRI2INT LF@tmp2 LF@%%1 LF@tmp_counter\n\
LT LF@%%cond LF@tmp1 LF@tmp2\n\
JUMPIFEQ $$$$strcmp_1lt2 LF@%%cond bool@true\n\
LT LF@%%cond LF@tmp2 LF@tmp1\n\
JUMPIFEQ $$$$strcmp_2lt1 LF@%%cond bool@true\n\
ADD LF@tmp_counter LF@tmp_counter int@1\n\
JUMP $$$$strcmp_while_start\n\
LABEL $$$$strcmp_eq\n\
MOVE LF@%%retval int@0\n\
JUMP $$$$strcmp_end\n\
LABEL $$$$strcmp_1lt2\n\
MOVE LF@%%retval int@1\n\
JUMP $$$$strcmp_end\n\
LABEL $$$$strcmp_2lt1\n\
MOVE LF@%%retval int@-1\n\
JUMP $$$$strcmp_end\n\
LABEL $$$$strcmp_other\n\
MOVE LF@%%retval nil@nil\n\
EXIT int@4\n\
LABEL $$$$strcmp_end\n\
POPFRAME\n\
RETURN\n");

    // Vestavěná funkce ifj.ord
    str_append(string, "\
LABEL $$$ord\n\
PUSHFRAME\n\
DEFVAR LF@%%0\n\
POPS LF@%%0\n\
DEFVAR LF@%%1\n\
POPS LF@%%1\n\
DEFVAR LF@%%retval\n\
DEFVAR LF@%%type\n\
TYPE LF@%%type LF@%%0\n\
JUMPIFNEQ $$$$ord_other LF@%%type string@string\n\
TYPE LF@%%type LF@%%1\n\
JUMPIFEQ $$$$ord_int LF@%%type string@int\n\
JUMPIFNEQ $$$$ord_other LF@%%type string@float\n\
FLOAT2INT LF@%%retval LF@%%1\n\
LABEL $$$$ord_int\n\
DEFVAR LF@%%len\n\
STRLEN LF@%%len LF@%%0\n\
DEFVAR LF@%%cond\n\
LT LF@%%cond LF@%%1 int@0\n\
JUMPIFEQ $$$$ord_null LF@%%cond bool@true\n\
LT LF@%%cond LF@%%1 LF@%%len\n\
JUMPIFEQ $$$$ord_null LF@%%cond bool@false\n\
STRI2INT LF@%%retval LF@%%0 LF@%%1\n\
JUMP $$$$ord_end\n\
LABEL $$$$ord_other\n\
MOVE LF@%%retval nil@nil\n\
EXIT int@4\n\
JUMP $$$$ord_end\n\
LABEL $$$$ord_null\n\
MOVE LF@%%retval int@0\n\
LABEL $$$$ord_end\n\
POPFRAME\n\
RETURN\n");
    
    str_append(string, "\
LABEL $$$chr\n\
PUSHFRAME\n\
DEFVAR LF@%%0\n\
POPS LF@%%0\n\
DEFVAR LF@%%retval\n\
DEFVAR LF@%%type\n\
TYPE LF@%%type LF@%%0\n\
JUMPIFEQ $$$$chr_int LF@%%type string@int\n\
JUMPIFNEQ $$$$chr_other LF@%%type string@float\n\
FLOAT2INT LF@%%0 LF@%%0\n\
LABEL $$$$chr_int\n\
INT2CHAR LF@%%retval LF@%%0\n\
JUMP $$$$chr_end\n\
LABEL $$$$chr_other\n\
MOVE LF@%%retval nil@nil\n\
EXIT int@4\n\
LABEL $$$$chr_end\n\
POPFRAME\n\
RETURN\n");
}

/*** Konec souboru codegen.c ***/