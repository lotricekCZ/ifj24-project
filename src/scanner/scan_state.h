#ifndef SCAN_STATE_H
#define SCAN_STATE_H

typedef enum
{
	scn_t_init,		//
	scn_t_string,	// anything between " and "
	scn_t_comment,	// anything between // and \n
	scn_t_operator, // +, -, *, /, =, ==, !=, >, <, >=, <=, &&, ||
	scn_t_number,	// 1, 2e-9, 1.1, 1.1e-9 -42
	scn_t_white,	// space, tab, \n, \r
	scn_t_braces,	// {, }, (, ), [, ], ., ;, |
	scn_t_lexeme,	// any literal symbol not enclosed in quotes and comment, after scan it is determined whether it is a keyword or not
} scn_state_t;

#endif