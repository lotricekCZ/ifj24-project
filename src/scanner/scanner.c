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
SCA_MATCH_DECL(at, '@')
SCA_MATCH_DECL(dot, '.')
SCA_MATCH_DECL(comma, ',')
SCA_MATCH_DECL(quote, '"')
SCA_MATCH_DECL(backslash, '\\')
SCA_MATCH_DECL(eof, EOF)
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
SCA_MATCH_DECL(hashtag, '#')
SCA_GREATER_DECL(text, 34)


Scanner_ptr scn_init(char *filename)
{
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
	scanner->list = tok_dll_init();
	/*
	test/scanner/test0.zig program in tokens

	const ifj = @import("ifj24.zig");

	pub fn bar(param : []u8) []u8 {
		const r = foo(param);
		return r;
	}

	pub fn foo(par : []u8) []u8 {
		const ret = bar(par);
		return ret;
	}

	pub fn main() void {
		const par = ifj.string("ahoj");
		_ = bar(par);
	}
	 */
	Token_ptr token[] = {
		// line 1
		tok_init(tok_t_const),
		tok_init(tok_t_sym), // here ifj
		tok_init(tok_t_ass),
		tok_init(tok_t_import),
		tok_init(tok_t_lpa),
		tok_init(tok_t_str), // here ifj24.zig
		tok_init(tok_t_rpa),
		tok_init(tok_t_semicolon),
		tok_init(tok_t_nl),
		// line 2
		tok_init(tok_t_nl),
		// line 3
		/*
		pub fn bar(param : []u8) []u8 {
			const r = foo(param);
			return r;
		}
		*/
		tok_init(tok_t_pub),
		tok_init(tok_t_fn),
		tok_init(tok_t_sym), // here bar
		tok_init(tok_t_lpa),
		tok_init(tok_t_sym), // here param
		tok_init(tok_t_colon),
		tok_init(tok_t_u8),
		tok_init(tok_t_rpa),
		tok_init(tok_t_u8),
		tok_init(tok_t_lcbr),
		tok_init(tok_t_nl),
		// line 4
		tok_init(tok_t_const),
		tok_init(tok_t_sym), // here r
		tok_init(tok_t_ass),
		tok_init(tok_t_sym), // here foo
		tok_init(tok_t_lpa),
		tok_init(tok_t_sym), // here param
		tok_init(tok_t_rpa),
		tok_init(tok_t_semicolon),
		tok_init(tok_t_nl),
		// line 5
		tok_init(tok_t_return),
		tok_init(tok_t_sym), // here r
		tok_init(tok_t_semicolon),
		tok_init(tok_t_nl),
		// line 6
		tok_init(tok_t_rcbr),
		tok_init(tok_t_nl),
		// line 7
		tok_init(tok_t_nl),
		// line 8
		/*
		pub fn foo(par : []u8) []u8 {
			const ret = bar(par);
			return ret;
		}
		*/
		tok_init(tok_t_pub),
		tok_init(tok_t_fn),
		tok_init(tok_t_sym), // here foo
		tok_init(tok_t_lpa),
		tok_init(tok_t_sym), // here par
		tok_init(tok_t_colon),
		tok_init(tok_t_u8),
		tok_init(tok_t_rpa),
		tok_init(tok_t_u8),
		tok_init(tok_t_lcbr),
		tok_init(tok_t_nl),
		// line 9
		tok_init(tok_t_const),
		tok_init(tok_t_sym), // here ret
		tok_init(tok_t_ass),
		tok_init(tok_t_sym), // here bar
		tok_init(tok_t_lpa),
		tok_init(tok_t_sym), // here par
		tok_init(tok_t_rpa),
		tok_init(tok_t_semicolon),
		tok_init(tok_t_nl),
		// line 10
		tok_init(tok_t_return),
		tok_init(tok_t_sym), // here ret
		tok_init(tok_t_semicolon),
		tok_init(tok_t_nl),
		// line 11
		tok_init(tok_t_rcbr),
		tok_init(tok_t_nl),
		// line 12
		tok_init(tok_t_nl),
		// line 13
		/*
		pub fn main() void {
			const par = ifj.string("ahoj");
			_ = bar(par);
		}
		*/
		tok_init(tok_t_pub),
		tok_init(tok_t_fn),
		tok_init(tok_t_sym), // here main
		tok_init(tok_t_lpa),
		tok_init(tok_t_rpa),
		tok_init(tok_t_void),
		tok_init(tok_t_lcbr),
		tok_init(tok_t_nl),
		// line 14
		tok_init(tok_t_const),
		tok_init(tok_t_sym), // here par
		tok_init(tok_t_ass),
		tok_init(tok_t_sym), // here ifj
		tok_init(tok_t_com),
		tok_init(tok_t_fnbui), // here string
		tok_init(tok_t_rpa),
		tok_init(tok_t_str), // here ahoj
		tok_init(tok_t_rpa),
		tok_init(tok_t_semicolon),
		tok_init(tok_t_nl),
		// line 15
		tok_init(tok_t_sym), // here _
		tok_init(tok_t_ass),
		tok_init(tok_t_sym), // here bar
		tok_init(tok_t_rpa),
		tok_init(tok_t_str), // here par
		tok_init(tok_t_rpa),
		tok_init(tok_t_semicolon),
		tok_init(tok_t_nl),
		// line 15
		tok_init(tok_t_rcbr),
		tok_init(tok_t_nl),
		tok_init(tok_t_eof)
		};
	
	tok_set_attribute(token[1], "ifj");
	tok_set_attribute(token[5], "ifj24.zig");
	tok_set_attribute(token[12], "bar");
	tok_set_attribute(token[14], "param");
	tok_set_attribute(token[22], "r");
	tok_set_attribute(token[24], "foo");
	tok_set_attribute(token[26], "param");
	tok_set_attribute(token[31], "r");
	tok_set_attribute(token[39], "foo");
	tok_set_attribute(token[41], "par");
	tok_set_attribute(token[49], "ret");
	tok_set_attribute(token[51], "bar");
	tok_set_attribute(token[53], "par");
	tok_set_attribute(token[58], "ret");
	tok_set_attribute(token[66], "main");
	tok_set_attribute(token[73], "par");
	tok_set_attribute(token[75], "ifj");
	tok_set_attribute(token[77], "string");
	tok_set_attribute(token[79], "ahoj");
	tok_set_attribute(token[83], "_");
	tok_set_attribute(token[85], "bar");
	tok_set_attribute(token[87], "par");
	for (int i = 0; i < 94; i++)
	{
		tok_dll_push_back(scanner->list, *token[i]);
		tok_free(token[i]);
	}
	tok_dll_first(scanner->list);
	return scanner;
}

void scn_free(Scanner_ptr scanner)
{
	tok_dll_dispose(scanner->list);
	free(scanner->file_name);
	free(scanner);
}

// bool scn_scan(Scanner_ptr scanner)
// {

// 	tok_dll_clear(scanner->list);
// 	char *source = scn_open_file(scanner);
// 	if (source == NULL)
// 	{
// 		// TODO: throw error err_internal
// 		return false;
// 	}
// 	size_t source_size = strlen(source);
// 	printf("%s\n", source);
// 	scn_state_t state = scn_t_init;
// 	Token_ptr token;

// 	size_t low = 0;
// 	size_t high = 0;

// 	while (high != source_size)
// 	{
// 		char c = (source)[high];
// 		switch (state)
// 		{
// 		case scn_t_init:
// 		{
// 			/**
// 			 *  scn_t_init
// 			 *	scn_t_string	// anything between " and "
// 			 *	scn_t_comment	// anything between // and \n
// 			 *	scn_t_operator	// +, -, *, /, =, ==, !=, >, <, >=, <=, &&, ||
// 			 *	scn_t_number	// 1, 2e-9, 1.1, 1.1e-9 -42
// 			 *	scn_t_white		// space, tab, \n, \r
// 			 *	scn_t_braces	// {, }, (, ), [, ], ., ;, |
// 			 *	scn_t_lexeme	// any literal symbol not enclosed in quotes and comment, after scan it is determined whether it is a keyword or not
// 			 */
// 			low = high;
// 			if (isalpha(c) || c == '_' || c == '@') // lexeme
// 				state = scn_t_lexeme;
// 			else if (isdigit(c)) // number
// 				state = scn_t_number;
// 			else if (c == '"') // string
// 				state = scn_t_string;
// 			else if (c == '/' && source[high + 1] == '/') // comment
// 				state = scn_t_comment;
// 			else if (c == ' ' || c == '\t' || c == '\n' || c == '\r') // white
// 				state = scn_t_white;
// 			else if (c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']' || c == '.' || c == ';' || c == '|') // braces
// 				state = scn_t_braces;
// 			else if (c == '+' || c == '-' || c == '*' || (c == '/' && source[high + 1] == '/') || c == '=' || c == '!' || c == '>' || c == '<') // operator
// 				state = scn_t_operator;
// 			else
// 			{
// 				state = scn_t_init;
// 				low = high;
// 			}
// 			break;
// 		}
// 		case scn_t_string:
// 		{
// 			if (c == '"')
// 			{
// 				char *lexeme = (char *)malloc(sizeof(char) * (high - low + 2));
// 				strncpy(lexeme, source + low, high - low + 1);
// 				lexeme[high - low + 1] = '\0';
// 				printf("string: %s\n", lexeme);
// 				free(lexeme);
// 				state = scn_t_init;
// 			}
// 			if (c == '\n')
// 			{
// 				// error
// 				// TODO: throw error err_internal
// 				return false;
// 			}

// 			break;
// 		}
// 		case scn_t_comment:
// 		{
// 			if (c == '\n')
// 			{
// 				char *lexeme = (char *)malloc(sizeof(char) * (high - low + 1));
// 				strncpy(lexeme, source + low, high - low);
// 				lexeme[high - low] = '\0';
// 				printf("comment: %s\n", lexeme);
// 				free(lexeme);
// 				state = scn_t_init;
// 			}
// 			break;
// 		}
// 		case scn_t_operator:
// 		{
// 			break;
// 		}
// 		case scn_t_number:
// 		{
// 			break;
// 		}
// 		case scn_t_white:
// 		{
// 			high--;
// 			state = scn_t_init;
// 			break;
// 		}
// 		case scn_t_braces:
// 		{
// 			break;
// 		}
// 		case scn_t_lexeme:
// 		{
// 			break;
// 		}
// 		}
// 		high++;
// 	}

// 	free(source);
// 	return true;
// }

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
Token_ptr scn_next(Scanner_ptr scanner){
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