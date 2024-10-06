/** IFJ2024 xhubacv00
 * @file instructions.h
 * @brief Implementace zpracování instrukcí strojového kódu
 * @author Vojtěch Hubacek (xhubacv00)
 */

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#define INSTRUCTION_SIZE 56
/** Výčet instrukcí strojového kódu */
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
} instruction;

char* instructions[INSTRUCTION_SIZE];

char *write(instruction, ...);

#endif