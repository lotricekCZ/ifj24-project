/** IFJ2024 xhubacv00
 * @file instructions.c
 * @brief Implementace zpracování instrukcí strojového kódu
 * @author Vojtěch Hubacek (xhubacv00)
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "codegen.h"
// #include <stdarg.h>

/** Formáty jednotlivých instrukcí */
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
    "# %s\n"                  // # ⟨string⟩
};

void printi_string(char* string) {
    for (int index = 0; index < strlen(string); index++) {
        // Detekce sekvence \x následované 2 hex číslicemi
        if (string[index] == 92 && string[index + 1] == 'x' && index + 3 < strlen(string)) {
            if (isxdigit(string[index + 2]) && isxdigit(string[index + 3])) {
                char high = string[index + 2];
                char low = string[index + 3];

                int value;
                if (isdigit(high))
                    value = (high - '0') * 16;
                else
                    value = (toupper(high) - 'A' + 10) * 16;

                if (isdigit(low))
                    value += low - '0';
                else
                    value += toupper(low) - 'A' + 10;

                string[index + 3] = (char)value; // Uložení výsledné hodnoty
                index += 3;                            // Přeskočení zpracovaných znaků
            }
        } else if (string[index] == 92 && string[index + 1] == 'n' && index + 1 < strlen(string)) {
            string[index + 1] = (char)10;
            index++;
        } else if (string[index] == 92 && string[index + 1] == 't' && index + 1 < strlen(string)) {
            string[index + 1] = (char)9;
            index++;
        } else if (string[index] == 92 && string[index + 1] == 92 && index + 1 < strlen(string)) {
            string[index + 1] = (char)92;
            index++;
        } else if (string[index] == 92 && string[index + 1] == 34 && index + 1 < strlen(string)) {
            string[index + 1] = (char)34;
            index++;
        }

        if ((string[index] >= 0 && string[index] <= 32) || string[index] == 35 || string[index] == 92) {
            printf("\\%03d", string[index]);
        }
        else {
            printf("%c", string[index]);
        }
    }
}

void printi_postfix(Token_ptr *postfix, int postfix_index) {
    int counter = 0;
    for (size_t index = 0; index < postfix_index; index++) {
        char buffer[MAX_STRING_LENGTH];
        switch (postfix[index]->type) {
            case tok_t_null:
                printf(format[_pushs], "nil@nil");
                break;
            case tok_t_int:
                sprintf(buffer, "int@%i", atoi(postfix[index]->attribute));
                printf(format[_pushs], buffer);
                break;
            case tok_t_flt:
                sprintf(buffer, "float@%a", atof(postfix[index]->attribute));
                printf(format[_pushs], buffer);
                break;
            case tok_t_bool:
                sprintf(buffer, "bool@%s", postfix[index]->attribute);
                printf(format[_pushs], buffer);
                break;
            case tok_t_sym:
                sprintf(buffer, "LF@%s", postfix[index]->attribute);
                printf(format[_pushs], buffer);
                break;
            case tok_t_plus:
                printf("%s", format[_adds]);
                break;
            case tok_t_minus:
                printf("%s", format[_subs]);
                break;
            case tok_t_times:
                printf("%s", format[_muls]);
                break;
            case tok_t_divide:
                printf("%s", format[_divs]);
                break;
            case tok_t_eq:
                printf("%s", format[_eqs]);
                break;
            case tok_t_neq:
                printf("%s", format[_eqs]);
                printf("%s", format[_nots]);
                break;
            case tok_t_lt:
                printf("%s", format[_lts]);
                break;
            case tok_t_gt:
                printf("%s", format[_gts]);
                break;
            case tok_t_leq:
                printf("%s", format[_gts]);
                printf("%s", format[_nots]);
                break;
            case tok_t_geq:
                printf("%s", format[_lts]);
                printf("%s", format[_nots]);
                break;
            case tok_t_and:
                printf("%s", format[_ands]);
                break;
            case tok_t_or:
                printf("%s", format[_ors]);
                break;
            case tok_t_not:
                printf("%s", format[_nots]);
                break;
            default:
                break;
        }
    }
}

void printi_condition_jump(char *name, int number) {
    printf("\
CREATEFRAME\n\
DEFVAR TF@%%type\n\
TYPE TF@%%type LF@%%%s%i\n\
JUMPIFEQ !$%s%i TF@%%type string@nil\n\
JUMPIFNEQ $$$$%s%i TF@%%type string@bool\n\
JUMPIFEQ !$%s%i LF@%%%s%i bool@false\n\
LABEL $$$$%s%i\n", name, number, name, number, name, number, name, number, name, number, name, number);
}

void printi_builtin() {
    printf("\
LABEL $$$readstr\n\
PUSHFRAME\n\
DEFVAR LF@%%retval\n\
READ LF@%%retval string\n\
POPFRAME\n\
RETURN\n");
    
    printf("\
LABEL $$$readi32\n\
PUSHFRAME\n\
DEFVAR LF@%%retval\n\
READ LF@%%retval int\n\
POPFRAME\n\
RETURN\n");

    printf("\
LABEL $$$readf64\n\
PUSHFRAME\n\
DEFVAR LF@%%retval\n\
READ LF@%%retval float\n\
POPFRAME\n\
RETURN\n");
    
    printf("\
LABEL $$$write\n\
PUSHFRAME\n\
DEFVAR LF@%%0\n\
POPS LF@%%0\n\
DEFVAR LF@%%type\n\
TYPE LF@%%type LF@%%0\n\
JUMPIFEQ $$$$write_nil LF@%%type string@nil\n\
JUMPIFEQ $$$$write_int LF@%%type string@bool\n\
JUMP $$$$write_end\n\
LABEL $$$$write_nil\n\
MOVE LF@%%0 string@null\n\
JUMP $$$$write_end\n\
LABEL $$$$write_bool\n\
JUMPIFEQ $$$$write_true LF@%%0 bool@true\n\
MOVE LF@%%0 string@false\n\
JUMP $$$$write_end\n\
LABEL $$$$write_true\n\
MOVE LF@%%0 string@true\n\
LABEL $$$$write_end\n\
WRITE LF@%%0\n\
POPFRAME\n\
RETURN\n");

    printf("\
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
    
    printf("\
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

    printf("\
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

    printf("\
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

    printf("\
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
            
    printf("\
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

    printf("\
LABEL $$$strcmp\n\
PUSHFRAME\n\
DEFVAR LF@%%0\n\
POPS LF@%%0\n\
DEFVAR LF@%%1\n\
POPS LF@%%1\n\
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
JUMPIFEQ $$$$strcmp_1lt2 LF@%%cond bool@true\n\
LT LF@%%cond LF@%%len2 LF@%%len1\n\
JUMPIFEQ $$$$strcmp_2lt1 LF@%%cond bool@true\n\
DEFVAR LF@tmp1\n\
DEFVAR LF@tmp1_counter\n\
MOVE LF@tmp1_counter int@0\n\
DEFVAR LF@tmp2\n\
DEFVAR LF@tmp2_counter\n\
MOVE LF@tmp2_counter int@0\n\
LABEL $$$$strcmp_while_start\n\
JUMPIFEQ $$$$strcmp_eq LF@%%len1 int@0\n\
SUB LF@%%len1 LF@%%len1 int@1\n\
STRI2INT LF@tmp1 LF@%%0 LF@tmp1_counter\n\
STRI2INT LF@tmp2 LF@%%1 LF@tmp2_counter\n\
LT LF@%%cond LF@tmp1 LF@tmp2\n\
JUMPIFEQ $$$$strcmp_1lt2 LF@%%cond bool@true\n\
LT LF@%%cond LF@tmp2 LF@tmp1\n\
JUMPIFEQ $$$$strcmp_2lt1 LF@%%cond bool@true\n\
ADD LF@tmp1_counter LF@tmp1_counter int@1\n\
ADD LF@tmp2_counter LF@tmp2_counter int@1\n\
JUMP $$$$strcmp_while_start\n\
LABEL $$$$strcmp_eq\n\
MOVE LF@%%retval int@0\n\
JUMP $$$$strcmp_end\n\
LABEL $$$$strcmp_1lt2\n\
MOVE LF@%%retval int@-1\n\
JUMP $$$$strcmp_end\n\
LABEL $$$$strcmp_2lt1\n\
MOVE LF@%%retval int@1\n\
JUMP $$$$strcmp_end\n\
LABEL $$$$strcmp_other\n\
MOVE LF@%%retval nil@nil\n\
EXIT int@4\n\
LABEL $$$$strcmp_end\n\
POPFRAME\n\
RETURN\n");

    printf("\
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
MOVE LF@%%retval nil@nil\n\
LABEL $$$$ord_end\n\
POPFRAME\n\
RETURN\n");
    
    printf("\
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


/**
 * @brief Zápis předávané instrukce
 * @param type Typ instrukce
 * @param ... Argumenty instrukce
 * @return char* Vraci retezec s instrukci
 */
/*
void printi(instruction type, ...) {
  if (type > _comment) {
      return "Unknown instruction type.";
  }
  va_list args;
  va_start(args, format[type]);
  vprintf(format[type], args);
  va_end(args);
  return;
}*/