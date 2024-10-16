#include "../../src/scanner/scanner.h"

int main()
{
	Scanner_ptr scanner = scn_init("./test/scanner/test0.zig");
	size_t i = 0;
	Token_ptr element = NULL;
	do {
		if(element != NULL)
			tok_free(element);
		element = scn_scan(scanner);
	} while (element->type != tok_t_eof);
	tok_free(element);
	scn_free(scanner);
	return 0;
}