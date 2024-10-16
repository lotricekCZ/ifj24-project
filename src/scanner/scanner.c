/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "scanner.h"
#include "scan_state.h"

SCA_MATCH_DECL(underscore, '_')
SCA_MATCH_DECL(question_mark, '?')
SCA_MATCH_DECL(brace_open, '[')
SCA_MATCH_DECL(brace_close, ']')
SCA_MATCH_DECL(at, '@')
SCA_MATCH_DECL(dot, '.')
SCA_MATCH_DECL(comma, ',')
SCA_MATCH_DECL(quote, '"')
SCA_MATCH_DECL(backslash, '\\')
SCA_MATCH_DECL(eof, EOF)
SCA_MATCH_DECL(end, '\0')
SCA_MATCH_DECL(curlybrace_close, '}')
SCA_MATCH_DECL(curlybrace_open, '{')
SCA_MATCH_DECL(parenthese_open, ')')
SCA_MATCH_DECL(parenthese_close, '(')
SCA_MATCH_DECL(semicolon, ';')
SCA_MATCH_DECL(slash, '/')
SCA_MATCH_DECL(vertical, '|')
SCA_MATCH_DECL(colon, ':')
SCA_MATCH_DECL(exclamation, '!')
SCA_MATCH_DECL(greater, '>')
SCA_MATCH_DECL(equal, '=')
SCA_MATCH_DECL(less, '<')
SCA_MATCH_DECL(asterisk, '*')
SCA_MATCH_DECL(minus, '-')
SCA_MATCH_DECL(plus, '+')
SCA_MATCH_DECL(u, 'u')
SCA_MATCH_DECL(x, 'x')
SCA_MATCH_DECL(E, 'E')
SCA_MATCH_DECL(e, 'e')
SCA_MATCH_DECL(hashtag, '#')
SCA_GREATER_DECL(text, 34)
SCA_GREATER_DECL(comments, 31) // for comments

// initial node for all tokens
Scan_node sca_init = {.state = sca_s_init, .children = NULL, .count = 0};

Scan_node sca_underscore = {.state = sca_s_underscore, .children = NULL, .count = 0};
Scan_node sca_lexeme = {.state = sca_s_lexeme, .children = NULL, .count = 0};
Scan_node sca_opt_str_lexeme = {.state = sca_s_opt_str_lexeme, .children = NULL, .count = 0}; // optional or string lexeme

Scan_node sca_qm = {.state = sca_s_qm, .children = NULL, .count = 0};
Scan_node sca_bro = {.state = sca_s_bro, .children = NULL, .count = 0};
Scan_node sca_brc = {.state = sca_s_brc, .children = NULL, .count = 0};

Scan_node sca_at = {.state = sca_s_at, .children = NULL, .count = 0};
Scan_node sca_atid = {.state = sca_s_atid, .children = NULL, .count = 0};

Scan_node sca_dt = {.state = sca_s_dt, .children = NULL, .count = 0};

Scan_node sca_int = {.state = sca_s_int, .children = NULL, .count = 0};
Scan_node sca_int2 = {.state = sca_s_int2, .children = NULL, .count = 0};
Scan_node sca_intdt = {.state = sca_s_intdt, .children = NULL, .count = 0};
Scan_node sca_intexp = {.state = sca_s_intexp, .children = NULL, .count = 0};
Scan_node sca_dec = {.state = sca_s_dec, .children = NULL, .count = 0};
Scan_node sca_dec2 = {.state = sca_s_dec2, .children = NULL, .count = 0};

Scan_node sca_s1 = {.state = sca_s_s1, .children = NULL, .count = 0};
Scan_node sca_s2 = {.state = sca_s_s2, .children = NULL, .count = 0};
Scan_node sca_s3 = {.state = sca_s_s3, .children = NULL, .count = 0};
Scan_node sca_s_max1 = {.state = sca_s_s_max1, .children = NULL, .count = 0};
Scan_node sca_s_max2 = {.state = sca_s_s_max2, .children = NULL, .count = 0};
Scan_node sca_s5 = {.state = sca_s_s5, .children = NULL, .count = 0};
Scan_node sca_str = {.state = sca_s_str, .children = NULL, .count = 0};

Scan_node sca_backslash = {.state = sca_s_backslash, .children = NULL, .count = 0};
Scan_node sca_ml1 = {.state = sca_s_ml1, .children = NULL, .count = 0};
Scan_node sca_ml2 = {.state = sca_s_ml2, .children = NULL, .count = 0};
Scan_node sca_ml3 = {.state = sca_s_ml3, .children = NULL, .count = 0};
Scan_node sca_ml_max1 = {.state = sca_s_ml_max1, .children = NULL, .count = 0};
Scan_node sca_ml_max2 = {.state = sca_s_ml_max2, .children = NULL, .count = 0};
Scan_node sca_ml5 = {.state = sca_s_ml5, .children = NULL, .count = 0};
Scan_node sca_ml6 = {.state = sca_s_ml6, .children = NULL, .count = 0};
Scan_node sca_ml_str = {.state = sca_s_ml_str, .children = NULL, .count = 0};

Scan_node sca_eof = {.state = sca_s_eof, .children = NULL, .count = 0};
Scan_node sca_curlybrace_close = {.state = sca_s_curlybrace_close, .children = NULL, .count = 0};
Scan_node sca_curlybrace_open = {.state = sca_s_curlybrace_open, .children = NULL, .count = 0};
Scan_node sca_parenthese_open = {.state = sca_s_parenthese_open, .children = NULL, .count = 0};
Scan_node sca_parenthese_close = {.state = sca_s_parenthese_close, .children = NULL, .count = 0};
Scan_node sca_semicolon = {.state = sca_s_semicolon, .children = NULL, .count = 0};
Scan_node sca_slash = {.state = sca_s_slash, .children = NULL, .count = 0};
Scan_node sca_comment = {.state = sca_s_comment, .children = NULL, .count = 0};
Scan_node sca_vertical = {.state = sca_s_vertical, .children = NULL, .count = 0};
Scan_node sca_colon = {.state = sca_s_colon, .children = NULL, .count = 0};
Scan_node sca_exclamation = {.state = sca_s_exclamation, .children = NULL, .count = 0};
Scan_node sca_notequal = {.state = sca_s_notequal, .children = NULL, .count = 0};
Scan_node sca_greater = {.state = sca_s_greater, .children = NULL, .count = 0};
Scan_node sca_greaterequal = {.state = sca_s_greaterequal, .children = NULL, .count = 0};
Scan_node sca_assign = {.state = sca_s_assign, .children = NULL, .count = 0};
Scan_node sca_equal = {.state = sca_s_equal, .children = NULL, .count = 0};
Scan_node sca_less = {.state = sca_s_less, .children = NULL, .count = 0};
Scan_node sca_lessequal = {.state = sca_s_lessequal, .children = NULL, .count = 0};
Scan_node sca_asterisk = {.state = sca_s_asterisk, .children = NULL, .count = 0};
Scan_node sca_minus = {.state = sca_s_minus, .children = NULL, .count = 0};
Scan_node sca_plus = {.state = sca_s_plus, .children = NULL, .count = 0};
Scan_node sca_comma = {.state = sca_s_comma, .children = NULL, .count = 0};
Scan_node sca_hashtag = {.state = sca_s_hashtag, .children = NULL, .count = 0};

// init to init
SCA_PATH_DEF(sca_init, sca_init)

// init to lexeme identification paths
SCA_PATH_DEF(sca_init, sca_underscore)
SCA_PATH_DEF(sca_init, sca_lexeme)
SCA_PATH_DEF(sca_lexeme, sca_lexeme)
SCA_PATH_DEF(sca_underscore, sca_lexeme)

// init to optional/string data type paths
SCA_PATH_DEF(sca_init, sca_qm)
SCA_PATH_DEF(sca_init, sca_bro)
SCA_PATH_DEF(sca_qm, sca_bro)
SCA_PATH_DEF(sca_bro, sca_brc)
SCA_PATH_DEF(sca_brc, sca_opt_str_lexeme)
SCA_PATH_DEF(sca_qm, sca_opt_str_lexeme)
SCA_PATH_DEF(sca_opt_str_lexeme, sca_opt_str_lexeme)

// init to import and as keywords
SCA_PATH_DEF(sca_init, sca_at)
SCA_PATH_DEF(sca_at, sca_atid)
SCA_PATH_DEF(sca_atid, sca_atid)

// init to builtin function paths
SCA_PATH_DEF(sca_init, sca_dt)

// init to number paths
SCA_PATH_DEF(sca_init, sca_int)
SCA_PATH_DEF(sca_int, sca_int2)
SCA_PATH_DEF(sca_int, sca_intdt)
SCA_PATH_DEF(sca_int, sca_intexp)

SCA_PATH_DEF(sca_int2, sca_int2)
SCA_PATH_DEF(sca_int2, sca_intdt)
SCA_PATH_DEF(sca_int2, sca_intexp)
SCA_PATH_DEF(sca_intexp, sca_dec)

SCA_PATH_DEF(sca_dec, sca_dec)
SCA_PATH_DEF(sca_intdt, sca_dec2)
SCA_PATH_DEF(sca_dec2, sca_dec2)
SCA_PATH_DEF(sca_dec2, sca_intexp)

// init to string paths
SCA_PATH_DEF(sca_init, sca_s1)
SCA_PATH_DEF(sca_s1, sca_s5)
SCA_PATH_DEF(sca_s1, sca_s2)
SCA_PATH_DEF(sca_s2, sca_s3)

SCA_PATH_DEF(sca_s3, sca_s_max1)
SCA_PATH_DEF(sca_s_max1, sca_s_max2)
SCA_PATH_DEF(sca_s_max2, sca_s2)
SCA_PATH_DEF(sca_s_max2, sca_s5)

SCA_PATH_DEF(sca_s5, sca_s5)
SCA_PATH_DEF(sca_s5, sca_s2)
SCA_PATH_DEF(sca_s2, sca_s5)
SCA_PATH_DEF(sca_s1, sca_str)

SCA_PATH_DEF(sca_s5, sca_str)
SCA_PATH_DEF(sca_s_max2, sca_str)

// init to multiline paths (reserved for future implementation)

// init to eof
SCA_PATH_DEF(sca_init, sca_eof)

// init to curly brace paths
SCA_PATH_DEF(sca_init, sca_curlybrace_close)
SCA_PATH_DEF(sca_init, sca_curlybrace_open)

// init to parenthese paths
SCA_PATH_DEF(sca_init, sca_parenthese_close)
SCA_PATH_DEF(sca_init, sca_parenthese_open)

// init to semicolon paths
SCA_PATH_DEF(sca_init, sca_semicolon)

// init to slash paths
SCA_PATH_DEF(sca_init, sca_slash)
SCA_PATH_DEF(sca_slash, sca_comment)
SCA_PATH_DEF(sca_comment, sca_comment)

// init to vertical path
SCA_PATH_DEF(sca_init, sca_vertical)

// init to colon paths
SCA_PATH_DEF(sca_init, sca_colon)

// init to exclamation paths
SCA_PATH_DEF(sca_init, sca_exclamation)
SCA_PATH_DEF(sca_exclamation, sca_notequal)

// init to greater paths
SCA_PATH_DEF(sca_init, sca_greater)
SCA_PATH_DEF(sca_greater, sca_greaterequal)

// init to equal paths
SCA_PATH_DEF(sca_init, sca_assign)
SCA_PATH_DEF(sca_assign, sca_equal)

// init to less paths
SCA_PATH_DEF(sca_init, sca_less)
SCA_PATH_DEF(sca_less, sca_lessequal)

// init to asterisk path
SCA_PATH_DEF(sca_init, sca_asterisk)

// init to plus path
SCA_PATH_DEF(sca_init, sca_plus)

// init to minus path
SCA_PATH_DEF(sca_init, sca_minus)

// init to comma path
SCA_PATH_DEF(sca_init, sca_comma)

// init to hashtag path
SCA_PATH_DEF(sca_init, sca_hashtag)

Scan_path sca_paths[] = {

};

/**
 * @brief Assigns children to a Scan_node.
 * @details This function uses a varargs list to assign children to a Scan_node.
 * The first argument is the size of the array, and the rest of the arguments are
 * pointers to Scan_path structures.
 * @param node The Scan_node to assign children to.
 */
void sca_assign_children(Scan_node_ptr node, int argc, ...)
{
	va_list args;
	va_start(args, argc);
	node->children = malloc(argc * sizeof(Scan_path *));
	node->count = argc;
	for (size_t i = 0; i < argc; i++)
		node->children[i] = va_arg(args, Scan_path *);
	va_end(args);
}

/**
 * @brief Frees memory allocated for a Scan_node and all its children.
 * @details Calls SCA_PATH_DEINIT on each child and then frees the memory allocated for children array and the node itself.
 * @param node The Scan_node to free.
 */
void sca_free(Scan_node_ptr node)
{

	for (size_t i = 0; i < node->count; i++)
	{
		SCA_PATH_DEINIT((*node->children[i]))
	}
	free(node->children);
}

Scanner_ptr scn_init(char *filename)
{
	// scanner setup
	Scanner_ptr scanner = malloc(sizeof(Scanner));

	if (scanner == NULL)
	{
		// TODO: throw error err_internal
		return NULL;
	}
	scanner->file_name = malloc(strlen(filename) + 1);
	if (scanner->file_name == NULL)
	{
		// TODO: throw error err_internal
		return NULL;
	}
	strcpy(scanner->file_name, filename);

	scanner->source = scn_open_file(scanner);
	if (scanner->source == NULL)
	{
		// TODO: throw error err_internal
		return false;
	}
	scanner->source_size = strlen(scanner->source);
	scanner->source_index = 0;
	
	scanner->list = tok_dll_init();

	/** scanner graph path config */
	// init to init
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_init), isspace)

	// init to lexeme identification paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_underscore), SCA_MATCH(underscore))
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_lexeme), isalpha)
	SCA_PATH_INIT(SCA_PATH(sca_lexeme, sca_lexeme), isalnum, SCA_MATCH(underscore))
	SCA_PATH_INIT(SCA_PATH(sca_underscore, sca_lexeme), isalnum, SCA_MATCH(underscore))

	// init to optional/string data type paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_qm), SCA_MATCH(question_mark))
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_bro), SCA_MATCH(brace_open))
	SCA_PATH_INIT(SCA_PATH(sca_qm, sca_bro), SCA_MATCH(brace_open))
	SCA_PATH_INIT(SCA_PATH(sca_bro, sca_brc), SCA_MATCH(brace_close))
	SCA_PATH_INIT(SCA_PATH(sca_brc, sca_opt_str_lexeme), SCA_MATCH(u))
	SCA_PATH_INIT(SCA_PATH(sca_qm, sca_opt_str_lexeme), isalnum)
	SCA_PATH_INIT(SCA_PATH(sca_opt_str_lexeme, sca_opt_str_lexeme), isalnum)

	// init to import and as keywords
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_at), SCA_MATCH(at))
	SCA_PATH_INIT(SCA_PATH(sca_at, sca_atid), isalpha)
	SCA_PATH_INIT(SCA_PATH(sca_atid, sca_atid), isalpha)

	// init to builtin function paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_dt), SCA_MATCH(dot))

	// init to number paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_int), isdigit)
	SCA_PATH_INIT(SCA_PATH(sca_int, sca_int2), isdigit)
	SCA_PATH_INIT(SCA_PATH(sca_int, sca_intdt), SCA_MATCH(dot))
	SCA_PATH_INIT(SCA_PATH(sca_int, sca_intexp), SCA_MATCH(e), SCA_MATCH(E))

	SCA_PATH_INIT(SCA_PATH(sca_int2, sca_int2), isdigit)
	SCA_PATH_INIT(SCA_PATH(sca_int2, sca_intdt), SCA_MATCH(dot))
	SCA_PATH_INIT(SCA_PATH(sca_int2, sca_intexp), SCA_MATCH(e), SCA_MATCH(E))
	SCA_PATH_INIT(SCA_PATH(sca_intexp, sca_dec), isdigit)

	SCA_PATH_INIT(SCA_PATH(sca_dec, sca_dec), isdigit)
	SCA_PATH_INIT(SCA_PATH(sca_intdt, sca_dec2), isdigit)
	SCA_PATH_INIT(SCA_PATH(sca_dec2, sca_dec2), isdigit)
	SCA_PATH_INIT(SCA_PATH(sca_dec2, sca_intexp), SCA_MATCH(e), SCA_MATCH(E))

	// init to string paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_s1), SCA_MATCH(quote))
	SCA_PATH_INIT(SCA_PATH(sca_s1, sca_s5), SCA_GREATER(text), SCA_MATCH(exclamation), isblank)
	SCA_PATH_INIT(SCA_PATH(sca_s1, sca_s2), SCA_MATCH(backslash))
	SCA_PATH_INIT(SCA_PATH(sca_s2, sca_s3), SCA_MATCH(x))

	SCA_PATH_INIT(SCA_PATH(sca_s3, sca_s_max1), isxdigit)
	SCA_PATH_INIT(SCA_PATH(sca_s_max1, sca_s_max2), isxdigit)
	SCA_PATH_INIT(SCA_PATH(sca_s_max2, sca_s2), SCA_MATCH(backslash))
	SCA_PATH_INIT(SCA_PATH(sca_s_max2, sca_s5), SCA_GREATER(text), SCA_MATCH(exclamation), isblank)

	SCA_PATH_INIT(SCA_PATH(sca_s5, sca_s5), SCA_GREATER(text), SCA_MATCH(exclamation), isblank)
	SCA_PATH_INIT(SCA_PATH(sca_s5, sca_s2), SCA_MATCH(backslash))
	SCA_PATH_INIT(SCA_PATH(sca_s2, sca_s5), SCA_GREATER(text), SCA_MATCH(exclamation), isblank)
	SCA_PATH_INIT(SCA_PATH(sca_s1, sca_str), SCA_MATCH(quote))

	SCA_PATH_INIT(SCA_PATH(sca_s5, sca_str), SCA_MATCH(quote))
	SCA_PATH_INIT(SCA_PATH(sca_s_max2, sca_str), SCA_MATCH(quote))

	// init to multiline paths (reserved for future implementation))

	// init to eof
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_eof), SCA_MATCH(eof), SCA_MATCH(end))

	// init to curly brace paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_curlybrace_close), SCA_MATCH(curlybrace_close))
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_curlybrace_open), SCA_MATCH(curlybrace_open))

	// init to parenthese paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_parenthese_close), SCA_MATCH(parenthese_close))
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_parenthese_open), SCA_MATCH(parenthese_open))

	// init to semicolon paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_semicolon), SCA_MATCH(semicolon))

	// init to slash paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_slash), SCA_MATCH(slash))
	SCA_PATH_INIT(SCA_PATH(sca_slash, sca_comment), SCA_MATCH(slash))
	SCA_PATH_INIT(SCA_PATH(sca_comment, sca_comment), SCA_GREATER(comments))

	// init to vertical path
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_vertical), SCA_MATCH(vertical))

	// init to colon paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_colon), SCA_MATCH(colon))

	// init to exclamation paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_exclamation), SCA_MATCH(exclamation))
	SCA_PATH_INIT(SCA_PATH(sca_exclamation, sca_notequal), SCA_MATCH(equal))

	// init to greater paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_greater), SCA_MATCH(greater))
	SCA_PATH_INIT(SCA_PATH(sca_greater, sca_greaterequal), SCA_MATCH(equal))

	// init to equal paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_assign), SCA_MATCH(equal))
	SCA_PATH_INIT(SCA_PATH(sca_assign, sca_equal), SCA_MATCH(equal))

	// init to less paths
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_less), SCA_MATCH(less))
	SCA_PATH_INIT(SCA_PATH(sca_less, sca_lessequal), SCA_MATCH(equal))

	// init to asterisk path
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_asterisk), SCA_MATCH(asterisk))

	// init to plus path
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_plus), SCA_MATCH(plus))

	// init to minus path
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_minus), SCA_MATCH(minus))

	// init to comma path
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_comma), SCA_MATCH(comma))

	// init to hashtag path
	SCA_PATH_INIT(SCA_PATH(sca_init, sca_hashtag), SCA_MATCH(hashtag))

	/** scanner graph node config */
	// scanner graph node config
	sca_assign_children(&sca_init, 27 /* 28 TODO: with multiline */, &SCA_PATH(sca_init, sca_init),
						&SCA_PATH(sca_init, sca_underscore), &SCA_PATH(sca_init, sca_lexeme), &SCA_PATH(sca_init, sca_qm),
						&SCA_PATH(sca_init, sca_bro), &SCA_PATH(sca_init, sca_at), &SCA_PATH(sca_init, sca_dt),
						&SCA_PATH(sca_init, sca_int), &SCA_PATH(sca_init, sca_s1), /* &SCA_PATH(sca_init, TODO: MULTILINE),*/
						&SCA_PATH(sca_init, sca_comma), &SCA_PATH(sca_init, sca_eof), &SCA_PATH(sca_init, sca_curlybrace_open),
						&SCA_PATH(sca_init, sca_curlybrace_close), &SCA_PATH(sca_init, sca_parenthese_open), &SCA_PATH(sca_init, sca_parenthese_close),
						&SCA_PATH(sca_init, sca_semicolon), &SCA_PATH(sca_init, sca_slash), &SCA_PATH(sca_init, sca_vertical),
						&SCA_PATH(sca_init, sca_colon), &SCA_PATH(sca_init, sca_exclamation), &SCA_PATH(sca_init, sca_greater),
						&SCA_PATH(sca_init, sca_assign), &SCA_PATH(sca_init, sca_less), &SCA_PATH(sca_init, sca_asterisk),
						&SCA_PATH(sca_init, sca_minus), &SCA_PATH(sca_init, sca_plus), &SCA_PATH(sca_init, sca_hashtag));

	sca_assign_children(&sca_underscore, 1, &SCA_PATH(sca_underscore, sca_lexeme));
	sca_assign_children(&sca_lexeme, 1, &SCA_PATH(sca_lexeme, sca_lexeme));
	sca_assign_children(&sca_opt_str_lexeme, 1, &SCA_PATH(sca_opt_str_lexeme, sca_opt_str_lexeme));

	sca_assign_children(&sca_qm, 2, &SCA_PATH(sca_qm, sca_opt_str_lexeme), &SCA_PATH(sca_qm, sca_bro));
	sca_assign_children(&sca_bro, 1, &SCA_PATH(sca_bro, sca_brc));
	sca_assign_children(&sca_brc, 1, &SCA_PATH(sca_brc, sca_opt_str_lexeme));

	sca_assign_children(&sca_at, 1, &SCA_PATH(sca_at, sca_atid));
	sca_assign_children(&sca_atid, 1, &SCA_PATH(sca_atid, sca_atid));

	sca_assign_children(&sca_int, 3, &SCA_PATH(sca_int, sca_intdt), &SCA_PATH(sca_int, sca_int2), &SCA_PATH(sca_int, sca_intexp));
	sca_assign_children(&sca_int2, 3, &SCA_PATH(sca_int2, sca_intdt), &SCA_PATH(sca_int2, sca_int2), &SCA_PATH(sca_int2, sca_intexp));
	sca_assign_children(&sca_intdt, 1, &SCA_PATH(sca_intdt, sca_dec2));
	sca_assign_children(&sca_intexp, 1, &SCA_PATH(sca_intexp, sca_dec));
	sca_assign_children(&sca_dec, 1, &SCA_PATH(sca_dec, sca_dec));
	sca_assign_children(&sca_dec2, 2, &SCA_PATH(sca_dec2, sca_dec2), &SCA_PATH(sca_dec2, sca_intexp));

	sca_assign_children(&sca_s1, 3, &SCA_PATH(sca_s1, sca_s5), &SCA_PATH(sca_s1, sca_s2), &SCA_PATH(sca_s1, sca_str));
	sca_assign_children(&sca_s2, 2, &SCA_PATH(sca_s2, sca_s3), &SCA_PATH(sca_s2, sca_s5));
	sca_assign_children(&sca_s3, 1, &SCA_PATH(sca_s3, sca_s_max1));
	sca_assign_children(&sca_s_max1, 1, &SCA_PATH(sca_s_max1, sca_s_max2));
	sca_assign_children(&sca_s_max2, 3, &SCA_PATH(sca_s_max2, sca_s5), &SCA_PATH(sca_s_max2, sca_s2), &SCA_PATH(sca_s_max2, sca_str));
	sca_assign_children(&sca_s5, 3, &SCA_PATH(sca_s5, sca_s5), &SCA_PATH(sca_s5, sca_s2), &SCA_PATH(sca_s5, sca_str));

	// TODO: Multiline string
	// sca_assign_children(&sca_backslash);
	// sca_assign_children(&sca_ml1);
	// sca_assign_children(&sca_ml2);
	// sca_assign_children(&sca_ml3);
	// sca_assign_children(&sca_ml_max1);
	// sca_assign_children(&sca_ml_max2);
	// sca_assign_children(&sca_ml5);
	// sca_assign_children(&sca_ml6);
	// sca_assign_children(&sca_ml_str);

	sca_assign_children(&sca_slash, 1, &SCA_PATH(sca_slash, sca_comment));
	sca_assign_children(&sca_comment, 1, &SCA_PATH(sca_comment, sca_comment));
	sca_assign_children(&sca_exclamation, 1, &SCA_PATH(sca_exclamation, sca_notequal));
	sca_assign_children(&sca_greater, 1, &SCA_PATH(sca_greater, sca_greaterequal));
	sca_assign_children(&sca_assign, 1, &SCA_PATH(sca_assign, sca_equal));
	sca_assign_children(&sca_less, 1, &SCA_PATH(sca_less, sca_lessequal));
	// sca_assign_children(&sca_hashtag);

	return scanner;
}

void scn_free(Scanner_ptr scanner)
{
	sca_free(&sca_init);
	sca_free(&sca_underscore);
	sca_free(&sca_lexeme);
	sca_free(&sca_opt_str_lexeme);

	sca_free(&sca_qm);
	sca_free(&sca_bro);
	sca_free(&sca_brc);

	sca_free(&sca_at);
	sca_free(&sca_atid);

	sca_free(&sca_int);
	sca_free(&sca_int2);
	sca_free(&sca_intdt);
	sca_free(&sca_intexp);
	sca_free(&sca_dec);
	sca_free(&sca_dec2);

	sca_free(&sca_s1);
	sca_free(&sca_s2);
	sca_free(&sca_s3);
	sca_free(&sca_s_max1);
	sca_free(&sca_s_max2);
	sca_free(&sca_s5);

	sca_free(&sca_slash);
	sca_free(&sca_comment);
	sca_free(&sca_exclamation);
	sca_free(&sca_greater);
	sca_free(&sca_assign);
	sca_free(&sca_less);
	tok_dll_dispose(scanner->list);
	free(scanner->file_name);
	free(scanner->source);
	free(scanner);
}
/**
 * @brief Vyhleda v poli podminek, zdali je splnena nejaka podminka.
 * @details Vyhleda v poli podminek, zdali je splnena nejaka podminka.
 * Podminky jsou v OR vztahu, takze jakmile najde splnenou podminku, funkce
 * skonci a vraci true. Pokud nesplni zadnou podminku, vraci false.
 * @param path Ukazatel na strukturu Scan_path, ktera obsahuje pole podminek.
 * @param c Char, ktery se ma zkontrolovat.
 * @returns true, pokud byla splnena nejaka podminka, jinak false.
 */
bool sca_p_has_match(Scan_path *path, char c)
{
	for (size_t index = 0; index < path->count; index++)
		if ((bool)(path->matches[index])((int)c))
			return true; // no need to evaluate further, conditions are in an OR relation
	return false;
}

/**
 * @brief Najde cestu z uzlu, ktera vyhovuje danemu znaku.
 * @details Iterativne projde cesty a hleda takovou, ktera vyhovuje danemu znaku.
 * Pokud takova cesta je nalezena, vraci ji. Pokud ne, vraci NULL.
 * @param node Uzel, ktery se ma prohledat.
 * @param c Znak, ktery se ma hledat.
 * @returns Cesta, ktera vyhovuje danemu znaku, nebo NULL, pokud takova cesta neexistuje.
 */
Scan_path *sca_n_has_match(Scan_node *node, char c)
{
	Scan_path *result = NULL;
	for (int index = 0; index < node->count; index++)
		if (sca_p_has_match(node->children[index], c))
		{
#ifdef DEBUG
			if (result != NULL)
			{
				fprintf(stderr, "Multiple matches found for character '%c', such behaviour is not NONDETERMINISTIC\n", c);
				return NULL;
			}
			result = node->children[index];
#else
			return node->children[index];
#endif
		}
#ifdef DEBUG
	return result;
#else
	return NULL;
#endif
}
Token_ptr scn_scan(Scanner_ptr scanner)
{
	// printf("%s\n", scanner->source);

	// scn_state_t state = scn_t_init;
	Token_ptr token;

	size_t high = scanner->source_index;
	Scan_node *node = &sca_init;
	bool has_match = true;
	while (has_match)
	{
		Scan_path *path = high <= scanner->source_size ? sca_n_has_match(node, scanner->source[high]) : NULL;
		if (path != NULL)
		{
			node = path->to;
			high++;
			if (path->to == path->from && path->to == &sca_init)
			{
				scanner->source_index = high;
			}
		}
		else
		{
			// TODO: capture token type (probably should be in node data)
			char *token_text = (char *)malloc(sizeof(char) * (high - scanner->source_index + 1));
			memcpy(token_text, scanner->source + scanner->source_index, high - scanner->source_index);
			token_text[high - scanner->source_index] = '\0';
			printf("Token: %s\n", token_text);
			Token_ptr token = tok_init((node->state != sca_s_eof)? tok_t_init: tok_t_eof);
			tok_set_attribute(token, token_text);
			scanner->source_index = high;
			has_match = false;
			free(token_text);
			return token;
		}
	}
	return NULL;
}

Token_ptr scn_get_token(Scanner_ptr scanner);
Token_ptr scn_previous(Scanner_ptr scanner);

/**
 * @brief Ziskani dalsiho tokenu
 *
 * Funkce vraci dalsi token v seznamu a posune aktivni prvek na dalsi.
 *
 * @param scanner Ukazatel na strukturu reprezentujici scanner
 * @return Ukazatel na strukturu reprezentujici token
 */
Token_ptr scn_next(Scanner_ptr scanner)
{
	Token_ptr token = scanner->list->activeElement->ptr;
	tok_dll_next(scanner->list);
	return token;
}

/**
 * @brief Otevreni souboru se zdrojovym kodem
 *
 * Funkce otevira soubor se zdrojovym kodem, nacte jeho obsah do pameti
 * a vrati ukazatel na dynamicke pole, kde je ulozen zdrojovy kod.
 *
 * @return Ukazatel na dynamicke pole, kde je ulozen zdrojovy kod
 */
char *scn_open_file(Scanner_ptr scanner)
{
	FILE *program = fopen(scanner->file_name, "r");
	if (program == NULL)
	{
		// TODO: throw error err_internal
		return NULL;
	}

	fseek(program, 0, SEEK_END);
	size_t file_size = ftell(program);
	fseek(program, 0, SEEK_SET);

	char *source = malloc(file_size);
	fread(source, 1, file_size, program);
	fclose(program);
	return source;
}