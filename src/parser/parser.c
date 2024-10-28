/** IFJ2024
 * xondre16; Jozef Ondrejička
 */

// TO-DO:
// 1) skontrolovat vsetky fprintf texty a return kody
// 2) doimplementovat gramatiku

#include <stdio.h>
#include "parser.h"
#include <string.h>
#include "../scanner/scanner.h"
#include "../utils/token.h"
#include "../utils/token_types.h"

/* 
 * Global variables
 */
Token_ptr current_token = NULL;
Scanner_ptr scanner = NULL;
token_type fn_ret_type;
/* 
 * Function to print the type and attribute of the current token
 */
void print_token() {
    printf("->%s : %s\n", tok_type_to_str(current_token->type), current_token->attribute ? current_token->attribute : "(null)");
}
/* 
 * Function to get the next token from the scanner
 */
void next_token() {
    if (current_token != NULL) {
        tok_free(current_token);
    }
    current_token = scn_scan(scanner);
}
/* 
 * Function to check if the current token is of the expected type
 */
void expect_type(token_type type) {
    printf("Expect_type: %s\n", tok_type_to_str(type));
    if (current_token->type != type) {
        fprintf(stderr, "Syntax error: Expected %s, got %s\n", tok_type_to_str(type), tok_type_to_str(current_token->type));
        exit(2); // Replace with better error handling if needed
    }
}
/* 
 * Function to check if the current token has the expected attribute
 */
void expect_attribute(const char* attr) {
    printf("Expect_attribute: %s\n", attr);
    if (current_token->attribute == NULL) {
        fprintf(stderr, "Syntax error: Expected %s, but current token has no attribute\n", attr);
        exit(2); // Replace with better error handling if needed
    }
    if (strcmp(current_token->attribute, attr) != 0) {
        fprintf(stderr, "Syntax error: Expected %s, got %s\n", attr, current_token->attribute);
        exit(2); // Replace with better error handling if needed
    }
}


/************** Grammar functions ************** 
 *  Function to parse the <program> non-terminal
 */
int program() {
    printf("<program>\n");

    prolog();
    
    function_declaration();
    
    return 0;
}
/* 
 *  Grammar: <prolog> → "const" <ID> "=" "@import" "(" <STR> ")" ";"
 */
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

    return 0;
}
/* 
 *  Grammar: <function_declaration> → "pub" "fn" <ID> "(" <parameter> ")" <return_type> "{" <statement> "}" <function_declaration_next>
 */
int function_declaration() {
    printf("<function_declaration>\n");
    // Funkce main nemá definován žádný parametr ani návratovou hodnotu, jinak chyba 4.

    next_token();
    print_token(); // "pub"
    expect_type(tok_t_pub);  

    next_token();
    print_token(); // "fn"
    expect_type(tok_t_fn);

    next_token();
    print_token(); // <ID>
    expect_type(tok_t_sym); 
    // vlozenie do TS

    next_token();
    print_token(); // "("
    expect_type(tok_t_lpa);  

    next_token();
    print_token();

    parameter();


    // ")"
    expect_type(tok_t_rpa);
    next_token();
    
    // For ONE function ONE return type, maybe ADD more
    switch (current_token->type)
    {
    case tok_t_i32:
        fn_ret_type = tok_t_i32;
        printf("fn_ret_type: %s\n", tok_type_to_str(fn_ret_type));
        break;
    case tok_t_f64:
        fn_ret_type = tok_t_f64;
        printf("fn_ret_type: %s\n", tok_type_to_str(fn_ret_type));
        break;
    case tok_t_u8:
        fn_ret_type = tok_t_u8;
        printf("fn_ret_type: %s\n", tok_type_to_str(fn_ret_type));
        break;
    case tok_t_void:
        fn_ret_type = tok_t_void;
        printf("fn_ret_type: %s\n", tok_type_to_str(fn_ret_type));
        break;
    default:
        fprintf(stderr, "Syntax error: Missing or wrong return type\n");
        exit(2); // Replace with better error handling if needed
        break;
    }

    next_token();
    print_token();
    expect_type(tok_t_lcbr); // "{"

    printf("-------------------------\n");
    next_token();
    print_token();

    // <statement>
    statement();

    // "}"
    expect_type(tok_t_rcbr);
    next_token();

    // <function_declaration_next>
    function_declaration_next();  

    return 0;
}
/*
 * Grammar: <function_declaration_next> → ε | <function_declaration>
 */
int function_declaration_next() {
    printf("<function_declaration_next>\n");
    if (current_token->type == tok_t_pub) {
        function_declaration();
    }
    // Else, epsilon production, do nothing
    print_token();
    expect_type(tok_t_eof);
    return 0;
}
/*
 * Grammar: <parameter> → ɛ | <ID> : <ID_type> <parameter_next>
 */
int parameter() {
    printf("<parameter>\n");
    bool params = false;
    if (current_token->type == tok_t_sym) {
        next_token();
        print_token();
        expect_type(tok_t_colon);  // ":"

        next_token();
        print_token();
        if (current_token->type == tok_t_i32 || current_token->type == tok_t_f64 || current_token->type == tok_t_u8) {
            next_token();
            print_token();
            // param counter
            params = true;
        } else {
            fprintf(stderr, "Syntax error: Expected type, got %s\n", tok_type_to_str(current_token->type));
            exit(2); // Replace with better error handling if needed
        }
    }

    if (current_token->type == tok_t_com && params) {
        next_token();
        print_token();

        // <parameter_next>
        parameter_next();
        params = false;
    }

    // Else, epsilon production, do nothing
    // Fucntion without parameters
    return 0;
}
/*
 * Grammar: <parameter_next> → ɛ | "," <ID> : <ID_type> <parameter_next>
 */
int parameter_next() {
    printf("<parameter_next>\n");
    if (current_token->type == tok_t_sym) {
        parameter();
    }

    // Else, epsilon production, do nothing
    return 0;
}
/*
 * Grammar: <statement> → <assignment> | <declaration> | <fn_call_statement> | ...
 */
int statement() {
    printf("<statement>\n");
    switch (current_token->type)
    {
    case tok_t_sym: // <assignment> || <fn_call_statement>
        printf("tok_t_sym\n");
        next_token();
        print_token();

        if (current_token->type == tok_t_ass)
        {
            assignment();
            printf("Assignment done\n");
        } else if (current_token->type == tok_t_lpa || current_token->type == tok_t_dot)
        {
            printf("<function call>\n");
            // fn_call_statement();
        } else 
        {
            fprintf(stderr, "Syntax error: Expected type, got %s\n", tok_type_to_str(current_token->type));
            exit(2); // Replace with better error handling if needed
        }
        next_token();
        print_token();

        statement_next();
        break;
    case tok_t_const: // <declaration> const
        printf("tok_t_const\n");
        declaration();
        printf("Declaration done\n");

        next_token();
        print_token();

        statement_next();
        break;
    case tok_t_var: // <declaration> var
        printf("tok_t_var\n");
        declaration();
        printf("Declaration done\n");

        next_token();
        print_token();
        
        statement_next();
        break;
    case tok_t_if: // <if_statement>
        // if_statement();
        printf("If statement done\n");
        
        next_token();
        print_token();
        
        statement_next();
        break;
    case tok_t_while: // <while_statement>
        // while_statement();
        printf("While statement done\n");
        
        next_token();
        print_token();
        
        statement_next();
        break;
    case tok_t_for: // <for_statement>
        // for_statement();
        printf("For statement done\n");
        
        next_token();
        print_token();
        
        statement_next();
        break;
    case tok_t_return: // <return_statement>
        // return_statement();
        printf("Return statement done\n");
        
        next_token();
        print_token();
        
        statement_next();
        break;
    case tok_t_break: // <break_statement>
        // break_statement();
        printf("Break statement done\n");
        
        next_token();
        print_token();
        
        statement_next();
        break;
    case tok_t_continue: // <continue_statement>
        // continue_statement();
        printf("Continue statement done\n");
        
        next_token();
        print_token();
        
        statement_next();
        break;
    default: // Syntax error
        fprintf(stderr, "Syntax error: Expected statement, got %s\n", tok_type_to_str(current_token->type));
        exit(2); 
        break;
    }

    // Add other conditions for fn_call_statement, if_statement, while_statement, etc.
    printf("Statements all done\n");
    return 0;
}
/*
 * Grammar: <statement_next> → ɛ | <statement>
 */
int statement_next() {
    printf("<statement_next>\n");

    // If the next token can start a statement, call statement().
    if (current_token->type != tok_t_rcbr) {
        statement();
    } // Else, epsilon production, do nothing.
    
    return 0;
}
/*
 * Grammar: <assignment> → <ID> <equals_to> ";" <statement_next>
 */
int assignment() {
    printf("<assignment>\n");
    // to-do: dokoncit gramatiku
    // assign premennej, expression, funkcie(s returnom), ...



    return 0;
}
/*
 * Grammar: <declaration> → <prefix> <ID> : <ID_type> <equals_to> ";" <statement_next>
 */
int declaration() {
    // Handle 'var' or 'const' prefix
    printf("<declaration>\n");
    if (current_token->type == tok_t_var) {
        next_token();  
        print_token();
        expect_type(tok_t_sym);  // <ID>

        next_token();  
        print_token();

        if (current_token->type == tok_t_ass){
            printf("Ass exp\n");
            // expression
        } else if (current_token->type == tok_t_colon)
        {
            printf(": type\n");
            next_token();  // Consume 'type' 
            print_token();
            if (current_token->type == tok_t_i32 || current_token->type == tok_t_f64 || current_token->type == tok_t_u8) {
                next_token();  
                print_token();
                if (current_token->type == tok_t_ass)
                {
                    printf(": type = \n");
                } else if (current_token->type == tok_t_semicolon)
                {
                    printf(": type;\n");
                } else
                {
                    fprintf(stderr, "Syntax error: Expected type, got %s\n", tok_type_to_str(current_token->type));
                    exit(2); // Replace with better error handling if needed
                }
            } else {
                fprintf(stderr, "Syntax error: Expected type, got %s\n", tok_type_to_str(current_token->type));
                exit(2); // Replace with better error handling if needed
            }
        }
    } else if (current_token->type == tok_t_const) {
        next_token();  
        print_token();
        expect_type(tok_t_sym);  // <ID>

        next_token();  
        print_token();

        if (current_token->type == tok_t_ass){
            printf("Ass exp\n");
            // expression
        } else if (current_token->type == tok_t_colon)
        {
            printf(": type\n");
            next_token();  // Consume 'type' 
            print_token();
            if (current_token->type == tok_t_i32 || current_token->type == tok_t_f64 || current_token->type == tok_t_u8) {
                next_token();  
                print_token();
                if (current_token->type == tok_t_ass)
                {
                    printf(": type = \n");
                    // expression
                } else if (current_token->type == tok_t_semicolon)
                {
                    printf(": type;\n");
                    // TS
                } else
                {
                    fprintf(stderr, "Syntax error: Expected type, got %s\n", tok_type_to_str(current_token->type));
                    exit(2); // Replace with better error handling if needed
                }
            } else {
                fprintf(stderr, "Syntax error: Expected type, got %s\n", tok_type_to_str(current_token->type));
                exit(2); // Replace with better error handling if needed
            }
        }
    } else {
        fprintf(stderr, "Syntax error: Expected 'var' or 'const', got %s\n", tok_type_to_str(current_token->type));
        exit(2); // Replace with better error handling if needed
    }

    // statement_next();  // <statement_next>
    printf("Meow meow\n");
    return 0;
}

/******** End of grammar functions ******** 
 * Function to parse the source code
 */
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