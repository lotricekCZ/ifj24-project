/** IFJ2024
 * xsidlil00; Lukáš Šidlík
 */

#ifndef EXPRESION_H
#define EXPRESION_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "dymString.h"
#include "token_types.h"
#include "token.h"
#include "stack.h"
#include "symtable.h"
#include "dymString.h"
#include "symDLList.h"

data_t * postfix_semantic(Token_ptr *postfix, int postfix_index, DLList sym_list, symtable_t *symtable, bool *convert);

#endif