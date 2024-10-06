#include "../../src/scanner/scanner.h"

int main() {
	Scanner_ptr scanner = scn_init("./test/scanner/test_com_str.zig");
	scn_scan(scanner);
	scn_free(scanner);
	return 0;
}