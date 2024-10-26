/** IFJ2024
 * xondre16; Jozef Ondrejička
 */

#include <stdio.h>
#include "parser.h"
#include <string.h>
#include "../scanner/scanner.h"
#include "../utils/token.h"
#include "../utils/token_types.h"

// Global variables for the current token and scanner
Token_ptr current_token = NULL;
Scanner_ptr scanner = NULL;

// Function to print the type and attribute of the current token
void print_token() {
    printf("%s : %s\n", tok_type_to_str(current_token->type), current_token->attribute ? current_token->attribute : "(null)");
}

void next_token() {
    if (current_token != NULL) {
        tok_free(current_token);
    }
    current_token = scn_scan(scanner);
}

void expect_type(token_type type) {
    printf("Exoect_type\n");
    if (current_token->type != type) {
        fprintf(stderr, "Syntax error: Expected %s, got %s\n", tok_type_to_str(type), tok_type_to_str(current_token->type));
        exit(2); // Replace with better error handling if needed
    }
}

void expect_attribute(const char* attr) {
    printf("Expect_attribute\n");
    if (current_token->attribute == NULL) {
        fprintf(stderr, "Syntax error: Expected %s, but current token has no attribute\n", attr);
        exit(2); // Replace with better error handling if needed
    }
    if (strcmp(current_token->attribute, attr) != 0) {
        fprintf(stderr, "Syntax error: Expected %s, got %s\n", attr, current_token->attribute);
        exit(2); // Replace with better error handling if needed
    }
}

// int statement();
// int statement_next();
// int declaration();
// int assignment();
// int parameter();
// int parameter_next();
// int function_declaration();
// int function_declaration_next();
// int program();
// int prolog();

// Grammar: <program> → <prolog> <function_declaration>
int program() {
    printf("<program>\n");

    prolog();
    
    // function_declaration();
    
    return 0;
}

// Grammar: <prolog> → "const" <ID> "=" "@import" "(" <STR> ")" ";"
int prolog() {
    printf("<prolog>\n");

    // "const"
    print_token();
    expect_type(tok_t_const);  

    // "ifj"
    next_token();
    print_token();
    expect_type(tok_t_sym); 
    expect_attribute("ifj");

    // "="
    next_token();
    print_token();
    expect_type(tok_t_ass);

    // "@import"
    next_token();
    print_token();
    expect_type(tok_t_import); 

    // "("
    next_token();
    print_token();
    expect_type(tok_t_lpa);  

    // "ifj24.zig"
    next_token();
    print_token();
    expect_type(tok_t_str);  
    expect_attribute("ifj24.zig");

    // ")"
    next_token();
    print_token();
    expect_type(tok_t_rpa);

    // ";"
    next_token();
    print_token();
    expect_type(tok_t_semicolon);

    printf("Vamooos");

    return 0;
}

// // Grammar: <function_declaration> → "pub" "fn" <ID> "(" <parameter> ")" <return_type> "{" <statement> "}" <function_declaration_next>
// int function_declaration() {
//     printf("<function_declaration>");
//     // Funkce main nemá definován žádný parametr ani návratovou hodnotu, jinak chyba 4.

//     // // "pub"     
//     // next_token();
//     // print_token();
//     // expect_type(tok_t_pub);  

//     // // "fn"
//     // next_token();
//     // print_token();
//     // expect_type(tok_t_fn);

//     // // <ID>
//     // next_token();
//     // print_token();
//     // expect_type(tok_t_sym); 
//     // // vlozenie do TS

//     // // "("
//     // next_token();
//     // print_token();
//     // expect_type(tok_t_lpa);  

//     // // ")"
//     // next_token();
//     // print_token();
//     // expect_type(tok_t_rpa);



// //     parameter();  // <parameter>
// //     expect(tok_t_rpa);  // ")"
// //     // Assuming a function for return_type parsing exists
// //     // return_type();
// //     expect(tok_t_lcbr);  // "{"
// //     statement();  // <statement>
// //     expect(tok_t_rcbr);  // "}"
// //     function_declaration_next();  // <function_declaration_next>

//     printf("Vamooos!\n");

//     return 0;
// }

// // Grammar: <function_declaration_next> → ε | <function_declaration>
// int function_declaration_next() {
//     if (current_token->type == tok_t_pub) {
//         function_declaration();
//     }
//     // Else, epsilon production, do nothing
//     return 0;
// }

// // Grammar: <parameter> → ɛ | <ID> : <ID_type> <parameter_next>
// int parameter() {
//     if (current_token->type == tok_t_sym) {
//         expect(tok_t_sym);  // <ID>
//         expect(tok_t_colon);  // ":"
//         // Assuming a function for ID_type exists
//         // id_type();
//         parameter_next();  // <parameter_next>
//     }
//     // Else, epsilon production, do nothing
//     return 0;
// }

// // Grammar: <parameter_next> → ɛ | "," <ID> : <ID_type> <parameter_next>
// int parameter_next() {
//     if (current_token->type == tok_t_com) {
//         expect(tok_t_com);  // ","
//         expect(tok_t_sym);  // <ID>
//         expect(tok_t_colon);  // ":"
//         // id_type();
//         parameter_next();  // <parameter_next>
//     }
//     // Else, epsilon production, do nothing
//     return 0;
// }

// // Grammar: <statement> → <assignment> | <declaration> | <fn_call_statement> | ...
// int statement() {
//     // This function would have logic to handle each possible statement.
//     if (current_token->type == tok_t_sym) {
//         // Depending on the context, it could be an assignment or a function call.
//         assignment();
//     } else if (current_token->type == tok_t_var || current_token->type == tok_t_const) {
//         declaration();
//     }
//     // Add other conditions for fn_call_statement, if_statement, while_statement, etc.
//     return 0;
// }

// // Grammar: <assignment> → <ID> <equals_to> ";" <statement_next>
// int assignment() {
//     expect(tok_t_sym);  // <ID>
//     // equals_to();
//     expect(tok_t_semicolon);  // ";"
//     statement_next();  // <statement_next>
//     return 0;
// }

// // Grammar: <declaration> → <prefix> <ID> : <ID_type> <equals_to> ";" <statement_next>
// int declaration() {
//     // Handle 'var' or 'const' prefix
//     if (current_token->type == tok_t_var || current_token->type == tok_t_const) {
//         next_token();  // Consume 'var' or 'const'
//     }
//     expect(tok_t_sym);  // <ID>
//     expect(tok_t_colon);  // ":"
//     // id_type();
//     // equals_to();
//     expect(tok_t_semicolon);  // ";"
//     statement_next();  // <statement_next>
//     return 0;
// }

// // Grammar: <statement_next> → ɛ | <statement>
// int statement_next() {
//     // If the next token can start a statement, call statement().
//     if (current_token->type == tok_t_sym || current_token->type == tok_t_var || current_token->type == tok_t_const) {
//         statement();
//     }
//     // Else, epsilon production, do nothing.
//     return 0;
// }

void parse() {
    printf("No nazdar!\n");

    // Initialize the scanner with the source code file.
    scanner = scn_init("test/scanner/test2.zig");
    if (scanner == NULL) {
        fprintf(stderr, "Failed to initialize scanner.\n");
        return;
    }

    // Get the first token.
    next_token();

    // Start parsing from the <program> non-terminal.
    program();

    
    // Free the scanner.
    scn_free(scanner);
}