/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

#include "keywords.h"
#include <stddef.h>

Keyword keywords[] = {
    // {tok_t_init, ""},
    // {tok_t_sym, ""},
    {tok_t_nl, "\n"},
    {tok_t_const, "const"},
    {tok_t_else, "else"},
    {tok_t_fn, "fn"},
    {tok_t_if, "if"},
    {tok_t_i32, "i32"},
    {tok_t_f64, "f64"},
    {tok_t_null, "null"},
    {tok_t_pub, "pub"},
    {tok_t_return, "return"},
    {tok_t_u8, "u8"},
    {tok_t_var, "var"},
    {tok_t_void, "void"},
    {tok_t_while, "while"},
    // {tok_t_val, ""},
    // {tok_t_flt, ""},
    // {tok_t_int, ""},
    // {tok_t_str, ""},
    {tok_t_lbr, "{"}, 
    {tok_t_rbr, "}"}, 
    {tok_t_lpa, "("}, 
    {tok_t_rpa, ")"}, 
    {tok_t_ass, "="}, 
    {tok_t_plus, "+"}, 
    {tok_t_minus, "-"}, 
    {tok_t_times, "*"}, 
    {tok_t_divide, "/"}, 
    {tok_t_neq, "!="},
    {tok_t_eq, "=="},
    {tok_t_lt, "<"},
    {tok_t_gt, ">"},
    {tok_t_leq, "<="},
    {tok_t_geq, ">="},
    {tok_t_doc, "//"},
    {tok_t_quot, "\""},
    {tok_t_colon, ":"},
    {tok_t_semicolon, ";"},
    {tok_t_com, "."}
};


/**
 * @brief Vraci lexem dle typu tokenu.
 * @param type typ tokenu
 * @return lexem tokenu nebo NULL, pokud nebyl nalezen
 */
char* kw_get_lexeme(token_type type) {
    for (int i = 0; i < sizeof(keywords) / sizeof(Keyword); i++) {
        if (keywords[i].type == type) {
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
token_type kw_get_type(char *lexeme) {
    for (int i = 0; i < sizeof(keywords) / sizeof(Keyword); i++) {
        if (strcmp(keywords[i].lexeme, lexeme) == 0) {
            return keywords[i].type;
        }
    }
    return tok_t_sym;
}