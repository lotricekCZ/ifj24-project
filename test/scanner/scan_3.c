#include "../../src/scanner/scanner.h"

int main(int argc, char **argv)
{
	Scanner_ptr scanner = scn_init(argv[1]);
	size_t i = 0;
	Token_ptr element = NULL;
	do {
		if(element != NULL)
			tok_free(element);
		element = scn_scan(scanner);
	} while (element->type != tok_t_eof && element->type != tok_t_error);
	tok_free(element);
	scn_free(scanner);
	return 0;
}