#include "../../src/scanner/scanner.h"

int main()
{
	Scanner_ptr scanner = scn_init("./test/scanner/test_com_str.zig");
	size_t i = 0;
	while (scanner->list->activeElement != NULL)
	{
		Token_ptr element = scn_next(scanner);
		printf("%4d | %14s |\t%s\n", i++, kw_get_lexeme(element->type) != NULL ? kw_get_lexeme(element->type) : "", element->attribute != NULL ? element->attribute : "");
	}
	scn_free(scanner);
	return 0;
}