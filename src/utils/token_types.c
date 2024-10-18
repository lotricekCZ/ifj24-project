/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

#include "token_types.h"
#include <stddef.h>

Keyword keywords[] = {
    {tok_t_const, "const"},
    {tok_t_else, "else"},
    {tok_t_fn, "fn"},
    {tok_t_if, "if"},
    {tok_t_i32, "i32"},
    {tok_t_f64, "f64"},
    {tok_t_null, "null"},
    {tok_t_pub, "pub"},
    {tok_t_return, "return"},
    {tok_t_u8, "[]u8"},
    {tok_t_var, "var"},
    {tok_t_void, "void"},
    {tok_t_while, "while"},
    {tok_t_i32_opt, "?i32"},
    {tok_t_f64_opt, "?f64"},
    {tok_t_u8_opt, "?[]u8"},
    {tok_t_import, "@import"},
    {tok_t_as, "@as"},
    {tok_t_for, "for"},
    {tok_t_break, "break"},
    {tok_t_continue, "continue"},
    {tok_t_bool, "bool"},
    {tok_t_or, "or"},
    {tok_t_and, "and"},
    {tok_t_orelse, "orelse"},
    {tok_t_unreach, "unreachable"},
};

/**
 * @brief Vraci lexem dle typu tokenu.
 * @param type typ tokenu
 * @return lexem tokenu nebo NULL, pokud nebyl nalezen
 */
char *kw_get_lexeme(token_type type)
{
    for (int i = 0; i < sizeof(keywords) / sizeof(Keyword); i++)
    {
        if (keywords[i].type == type)
        {
            return keywords[i].lexeme;
        }
    }
    return NULL;
}

/**
 * @brief Najde typ tokenu odpovidajici danemu lexemu.
 * @param lexeme lexem, pro ktery se hleda typ tokenu
 * @return typ tokenu, nebo tok_t_sym, pokud nebyl nalezen
 */
token_type kw_get_type(char *lexeme)
{
    for (int i = 0; i < sizeof(keywords) / sizeof(Keyword); i++)
    {
        if (strcmp(keywords[i].lexeme, lexeme) == 0)
        {
            return keywords[i].type;
        }
    }
    return tok_t_sym;
}