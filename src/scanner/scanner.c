/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */
#include "scanner.h"
#include <stdlib.h>
#include <string.h>
#include "scan_state.h"

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
	return scanner;
}

void scn_free(Scanner_ptr scanner)
{
	tok_dll_dispose(scanner->list);
	free(scanner->file_name);
	free(scanner);
}

bool scn_scan(Scanner_ptr scanner)
{

	tok_dll_clear(scanner->list);
	char *source = scn_open_file(scanner);
	if (source == NULL)
	{
		// TODO: throw error err_internal
		return false;
	}
	size_t source_size = strlen(source);
	printf("%s\n", source);
	scn_state_t state = scn_t_init;
	Token_ptr token;

	size_t low = 0;
	size_t high = 0;

	while (high != source_size)
	{
		char c = (source)[high];
			switch (state)
		{
		case scn_t_init:
		{
			/**
			 *  scn_t_init
			 *	scn_t_string	// anything between " and "
			 *	scn_t_comment	// anything between // and \n
			 *	scn_t_operator	// +, -, *, /, =, ==, !=, >, <, >=, <=, &&, ||
			 *	scn_t_number	// 1, 2e-9, 1.1, 1.1e-9 -42
			 *	scn_t_white		// space, tab, \n, \r
			 *	scn_t_braces	// {, }, (, ), [, ], ., ;, |
			 *	scn_t_lexeme	// any literal symbol not enclosed in quotes and comment, after scan it is determined whether it is a keyword or not
			 */
			low = high;
			if (isalpha(c) || c == '_' || c == '@') // lexeme
				state = scn_t_lexeme;
			else if (isdigit(c)) // number
				state = scn_t_number;
			else if (c == '"') // string
				state = scn_t_string;
			else if (c == '/' && source[high + 1] == '/') // comment
				state = scn_t_comment;
			else if (c == ' ' || c == '\t' || c == '\n' || c == '\r') // white
				state = scn_t_white;
			else if (c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']' || c == '.' || c == ';' || c == '|') // braces
				state = scn_t_braces;
			else if (c == '+' || c == '-' || c == '*' || (c == '/'&& source[high + 1] == '/') || c == '=' || c == '!' || c == '>' || c == '<') // operator
				state = scn_t_operator;
			else{
				state = scn_t_init;
				low = high;
				}
			break;
		}
		case scn_t_string:
		{
			if (c == '"')
			{
				char *lexeme = (char *)malloc(sizeof(char) * (high - low + 2));
				strncpy(lexeme, source + low, high - low + 1);
				lexeme[high - low + 1] = '\0';
				printf("string: %s\n", lexeme);
				free(lexeme);
				state = scn_t_init;
			}
			if (c == '\n')
			{
				// error
				// TODO: throw error err_internal
				return false;
			}

			break;
		}
		case scn_t_comment:
		{
			if (c == '\n')
			{
				char *lexeme = (char *)malloc(sizeof(char) * (high - low + 1));
				strncpy(lexeme, source + low, high - low);
				lexeme[high - low] = '\0';
				printf("comment: %s\n", lexeme);
				free(lexeme);
				state = scn_t_init;
			}
			break;
		}
		case scn_t_operator:
		{
			break;
		}
		case scn_t_number:
		{
			break;
		}
		case scn_t_white:
		{
			high--;
			state = scn_t_init;
			break;
		}
		case scn_t_braces:
		{
			break;
		}
		case scn_t_lexeme:
		{
			break;
		}
		}
		high++;
	}

	free(source);
	return true;
}

Token_ptr scn_get_token(Scanner_ptr scanner);
Token_ptr scn_previous(Scanner_ptr scanner);
Token_ptr scn_next(Scanner_ptr scanner);

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