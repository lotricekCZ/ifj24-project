/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

#ifndef KEYWORDS_H
#define KEYWORDS_H

/**
 * @brief Typ tokenu
 *
 * Enum který reprezentuje dosažitelné typy tokenů
 */
typedef enum _token_type
{
	tok_t_init,	  	// initial token value, used only for initialization
	tok_t_sym, 		// symbol
	tok_t_nl,	 	// newline
	tok_t_const, 	// constant
	tok_t_else,	 	// else
	tok_t_fn,	 	// function
	tok_t_if,	 	// if
	tok_t_i32,	 	// int32
	tok_t_f64,	 	// float64
	tok_t_null,	 	// null
	tok_t_pub,	 	// public
	tok_t_return,	// return
	tok_t_u8,	 	// uint8
	tok_t_var,	 	// variable
	tok_t_void,	 	// void
	tok_t_while, 	// while

	tok_t_val, 		// value like 42, "Foo", or 6.022e-22

	tok_t_flt,		// DOUBLE or floating point value
	tok_t_int,		// INT or integer value
	tok_t_str,		// STRING or anything enclosed in quotes
	tok_t_lbr,		// {, curly brace
	tok_t_rbr,		// }, right curly brace
	tok_t_lpa,		// (, left parenthese
	tok_t_rpa,		// ), right parenthese
	tok_t_ass,		// =, assignment operator
	tok_t_plus, 	// +, plus operator
	tok_t_minus,
	tok_t_times,
	tok_t_divide,

	tok_t_neq, 		// !=, not equal
	tok_t_eq,  		// ==, equal
	tok_t_lt,  		// <, less than
	tok_t_gt,  		// >, greater than
	tok_t_leq, 		// <=, less or equal
	tok_t_geq, 		// >=, greater or equal

	tok_t_doc, 		// comment

	tok_t_quot,	 	// quote
	tok_t_colon, 	// :
	tok_t_com	 	// comma
} token_type;


typedef struct _Keywords
{
	token_type type;
	char *lexeme;
} Keyword;

Keyword keywords[];
#endif // KEYWORDS_H