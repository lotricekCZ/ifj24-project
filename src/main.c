/** IFJ2024
 * xramas01; Jakub Ramaseuski
 */

#include <stdio.h>
#include "utils/token_types.h"
#include "parser/parser.h"
#include "utils/errors.h"

int main(int argc, char *argv[])
{
    if (argc >= 2) {
        scanner = scn_init(argv[1]);
    } else {
        // Initialize the scanner with the source code file.
        scanner = scn_init("../test/codegen/funcs.zig");
        //fprintf(stderr, "No source code file specified.\n");
        //return err_internal;
    }
    if (scanner == NULL) {
        fprintf(stderr, "Failed to initialize scanner.\n");
        return err_internal;
    }

    // Parse the source code.
    err_codes error = parse();

    // Free the scanner.
    scn_free(scanner);

    return error;
}