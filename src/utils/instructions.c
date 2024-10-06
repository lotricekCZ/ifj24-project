/** IFJ2024 xhubacv00
 * @file instructions.c
 * @brief Implementace zpracování instrukcí strojového kódu
 * @author Vojtěch Hubacek (xhubacv00)
 */

#include "instructions.h"
#include <stdarg.h>

/** Formáty jednotlivých instrukcí */
const char *formats[INSTRUCTION_SIZE] = {
    {"MOVE %s %s\n"},           // MOVE ⟨var⟩ ⟨symb⟩
    {"CREATEFRAME\n"},          // CREATEFRAME
    {"PUSHFRAME\n"},            // PUSHFRAME
    {"POPFRAME\n"},             // POPFRAME
    {"DEFVAR %s\n"},            // DEFVAR ⟨var⟩
    {"CALL %s\n"},              // CALL ⟨label⟩
    {"RETURN\n"},               // RETURN
    {"PUSHS %s\n"},             // PUSHS ⟨symb⟩
    {"POPS %s\n"},              // POPS ⟨var⟩
    {"CLEARS\n"},               // CLEARS
    {"ADD %s %s %s\n"},         // ADD ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"SUB %s %s %s\n"},         // ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"MUL %s %s %s\n"},         // ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"DIV %s %s %s\n"},         // ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"IDIV %s %s %s\n"},        // ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"ADDS\n"},                 // ADDS
    {"SUBS\n"},                 // SUBS
    {"MULS\n"},                 // MULS
    {"DIVS\n"},                 // DIVS
    {"IDIVS %s\n"},             // IDIVS ⟨var⟩
    {"LT %s %s %s\n"},          // LT ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"GT %s %s %s\n"},          // GT ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"EQ %s %s %s\n"},          // EQ ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"LTS %s %s %s\n"},         // LTS ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"GTS %s %s %s\n"},         // GTS ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"EQS %s %s %s\n"},         // EQS ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"AND %s %s %s\n"},         // AND ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"OR %s %s %s\n"},          // OR ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"NOT %s %s %s\n"},         // NOT ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"ANDS %s %s %s\n"},        // ANDS ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"ORS %s %s %s\n"},         // ORS ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"NOTS %s %s %s\n"},        // NOTS ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"INT2FLOAT %s %s\n"},      // INT2FLOAT ⟨var⟩ ⟨symb⟩
    {"FLOAT2INT %s %s\n"},      // FLOAT2INT ⟨var⟩ ⟨symb⟩
    {"INT2CHAR %s %s\n"},       // INT2CHAR ⟨var⟩ ⟨symb⟩
    {"STRI2INT %s %s %s\n"},    // STRI2INT ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"INT2FLOATS %s\n"},        // INT2FLOATS ⟨var⟩
    {"FLOAT2INTS %s\n"},        // FLOAT2INTS ⟨var⟩
    {"INT2CHARS %s\n"},         // INT2CHARS ⟨var⟩
    {"STRI2INTS %s\n"},         // STRI2INTS ⟨var⟩
    {"READ %s %s\n"},           // READ ⟨var⟩ ⟨type⟩
    {"WRITE %s\n"},             // WRITE ⟨symb⟩
    {"CONCAT %s %s %s\n"},      // CONCAT ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"STRLEN %s %s\n"},         // STRLEN ⟨var⟩ ⟨symb⟩
    {"GETCHAR %s %s %s\n"},     // GETCHAR ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"SETCHAR %s %s %s\n"},     // SETCHAR ⟨var⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"TYPE %s %s\n"},           // TYPE ⟨var⟩ ⟨symb⟩
    {"LABEL %s\n"},             // LABEL ⟨label⟩
    {"JUMP %s\n"},              // JUMP ⟨label⟩
    {"JUMPIFEQ %s %s %s\n"},    // JUMPIFEQ ⟨label⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"JUMPIFNEQ %s %s %s\n"},   // JUMPIFNEQ ⟨label⟩ ⟨symb 1⟩ ⟨symb 2⟩
    {"JUMPIFEQS %s\n"},         // JUMPIFEQS ⟨label⟩
    {"JUMPIFNEQS %s\n"},        // JUMPIFNEQS ⟨label⟩
    {"EXIT %s\n"},              // EXIT ⟨symb⟩
    {"BREAK\n"},                // BREAK
    {"DPRINT %s\n"},            // DPRINT ⟨symb⟩
};

/**
 * @brief Zápis předávané instrukce
 * @param type Typ instrukce
 * @param ... Argumenty instrukce
 * @return char* Vraci retezec s instrukci
*/
char *write(instruction type, ...) {
    if (type > _dprint) {
        return "Unknown instruction type.";
    }
    va_list args;
    va_start(args, formats[type]);
    vprintf(formats[type], args);
    va_end(args);
    /// TODO

    return "";
}