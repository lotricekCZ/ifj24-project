#include "../../src/scanner/scanner.h"

int main()
{
	Scanner_ptr scanner = scn_init("./test/scanner/test_com_str.zig");
	size_t i = 0;
	Token_ptr element = NULL;
	do {
		element = scn_scan(scanner);
	} while (element->type != tok_t_eof);
	scn_free(scanner);
	return 0;
}