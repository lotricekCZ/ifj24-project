/** IFJ2024
 * xondre16; Jozef Ondrejička
 */

// TO-DO:
// 1) expression (bottom-up) - výrazy by mali byť spracované pomocou precedenčnej syntaktickej analýzy
// 2) built-in funkcie
// 3) assignment -> declaration

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"
#include "../scanner/scanner.h"
#include "../utils/token.h"
#include "../utils/token_types.h"

/* 
 * Global variables
 */
Token_ptr current_token = NULL;
Scanner_ptr scanner = NULL;
token_type fn_ret_type;
bool ifj_flag = false;
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
        exit(2); // Syntax error
    }
}
/* 
 * Function to check if the current token has the expected attribute
 */
void expect_attribute(const char* attr) {
    printf("Expect_attribute: %s\n", attr);
    if (current_token->attribute == NULL) {
        fprintf(stderr, "Syntax error: Expected %s, but current token has no attribute\n", attr);
        exit(2); // Syntax error
    }
    if (strcmp(current_token->attribute, attr) != 0) {
        fprintf(stderr, "Syntax error: Expected %s, got %s\n", attr, current_token->attribute);
        exit(2); // Syntax error
    }
}


/************** Grammar functions ************** 
 *  Function to parse the <program> non-terminal
 */
int program() {
    printf("-------------------------\n");
    printf("<program>\n");

    prolog();
    printf("-------------------------\n");
    
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
    printf("-------------------------\n");
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
    switch (current_token->type) {
    case tok_t_i32:
    case tok_t_f64:
    case tok_t_u8:
    case tok_t_void:
        fn_ret_type = current_token->type;
        printf("fn_ret_type: %s\n", tok_type_to_str(fn_ret_type));
        break;
    default:
        fprintf(stderr, "Syntax error: Missing or wrong return type\n");
        exit(2); // Syntax error
    }

    next_token();
    print_token();
    expect_type(tok_t_lcbr); // "{"

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
    printf("-------------------------\n");
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
    printf("-------------------------\n");
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
            exit(2); // Syntax error
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
    printf("-------------------------\n");
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
    printf("-------------------------\n");
    printf("<statement>\n");
    switch (current_token->type)
    {
    case tok_t_sym: // <assignment> || <fn_call_statement>
        printf("tok_t_sym\n");
        ifj_flag = false;
        if (strcmp(current_token->attribute, "ifj") == 0){
            ifj_flag = true;
        }
        next_token();
        print_token();

        if (current_token->type == tok_t_ass)
        {
            assignment();
            printf("<assignment> DONE\n");
        } else if (current_token->type == tok_t_lpa || current_token->type == tok_t_dot)
        {
            fn_call_statement(ifj_flag);
            printf("<fn_call_statement> DONE\n");
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
        printf("<declaration> DONE\n");

        next_token();
        print_token();

        statement_next();
        break;
    case tok_t_var: // <declaration> var
        printf("tok_t_var\n");
        declaration();
        printf("<declaration> DONE\n");

        next_token();
        print_token();
        
        statement_next();
        break;
    case tok_t_if: // <if_statement>
        if_statement();
        printf("<if_statement> DONE\n");
        
        next_token();
        print_token();
        
        statement_next();
        break;
    case tok_t_while: // <while_statement>
        while_statement();
        printf("<while_statement> DONE\n");
        
        next_token();
        print_token();
        
        statement_next();
        break;
    case tok_t_for: // <for_statement>
        for_statement();
        printf("<for_statement> DONE\n");
        
        next_token();
        print_token();
        
        statement_next();
        break;
    case tok_t_return: // <return_statement>
        return_statement();
        printf("<return_statement> DONE\n");
        
        // Do not call statement_next() after return
        break;
    case tok_t_break: // <break_statement>
        break_statement();
        printf("<break_statement> DONE\n");
        
        // Do not call statement_next() after break
        break;
    case tok_t_continue: // <continue_statement>
        continue_statement();
        printf("<continue_statement> DONE\n");
        
        // Do not call statement_next() after continue
        break;
    default: // Syntax error
        fprintf(stderr, "Syntax error: Expected statement, got %s\n", tok_type_to_str(current_token->type));
        exit(2); 
        break;
    }

    // Add other conditions for fn_call_statement, if_statement, while_statement, etc.
    printf("All statements DONE\n");
    return 0;
}
/*
 * Grammar: <statement_next> → ɛ | <statement>
 */
int statement_next() {
    printf("-------------------------\n");
    printf("<statement_next>\n");

    // If the next token can start a statement, call statement().
    if (current_token->type != tok_t_rcbr && current_token->type != tok_t_eof) {
        statement();
    } // Else, epsilon production, do nothing.
    
    return 0;
}
/*
 * Grammar: <assignment> → <ID> <equals_to> ";" <statement_next>
 */
int assignment() {
    printf("-------------------------\n");
    printf("<assignment>\n");
    // assign premennej, expression, funkcie(s returnom), ...

    // test na to, ci je uz deklarovana premenna
    // ak hej, tak asi rovno <declaration>


    return 0;
}
/*
 * Grammar: <declaration> → <prefix> <ID> : <ID_type> <equals_to> ";" <statement_next>
 */
int declaration() {
    // Handle 'var' or 'const' prefix
    printf("-------------------------\n");
    printf("<declaration>\n");
    if (current_token->type == tok_t_var) { // <prefix> var
        next_token();  
        print_token();
        expect_type(tok_t_sym);  // <ID>

        next_token();  
        print_token();

        if (current_token->type == tok_t_ass){
            printf("Ass exp\n");
            // expression

            // zde implementuj

        } else if (current_token->type == tok_t_colon)
        {
            printf(": type\n");
            next_token();  
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
                    printf(": type; \n");
                    // TS
                } else
                {
                    fprintf(stderr, "Syntax error: Expected type, got %s\n", tok_type_to_str(current_token->type));
                    exit(2); // Syntax error
                }
            } else {
                fprintf(stderr, "Syntax error: Expected type, got %s\n", tok_type_to_str(current_token->type));
                exit(2); // Syntax error
            }
        }
    } else if (current_token->type == tok_t_const) { // <prefix> const
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
            next_token();  
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
                    printf(": type; \n");
                    // TS
                } else
                {
                    fprintf(stderr, "Syntax error: Expected type, got %s\n", tok_type_to_str(current_token->type));
                    exit(2); // Syntax error
                }
            } else {
                fprintf(stderr, "Syntax error: Expected type, got %s\n", tok_type_to_str(current_token->type));
                exit(2); // Syntax error
            }
        }
    } else {
        fprintf(stderr, "Syntax error: Expected 'var' or 'const', got %s\n", tok_type_to_str(current_token->type));
        exit(2); // Syntax error
    }

    return 0;
}
/*
 * Grammar: <fn_call_statement> → <ID> "(" <fn_call_params> ")" ";"
 */
int fn_call_statement(bool ifj_flag) {
    printf("-------------------------\n");
    printf("<fn_call_statement>\n");
    
    if (ifj_flag) {
        expect_type(tok_t_dot);
        next_token();
        print_token();

        // TO-DO: Each function differently
        if (strcmp(current_token->attribute, "readstr") == 0) {
            printf("readstr\n");
            // implement readstr
        } else if (strcmp(current_token->attribute, "readi32") == 0) {
            printf("readi32\n");
            // implement readi32
        } else if (strcmp(current_token->attribute, "readf64") == 0) {
            printf("readf64\n");
            // implement readf64
        } else if (strcmp(current_token->attribute, "write") == 0) {
            printf("write\n");
            // implement write
        } else if (strcmp(current_token->attribute, "i2f") == 0) {
            printf("i2f\n");
            // implement i2f
        } else if (strcmp(current_token->attribute, "f2i") == 0) {
            printf("f2i\n");
            // implement f2i
        } else if (strcmp(current_token->attribute, "string") == 0) {
            printf("string\n");
            // implement string
        } else if (strcmp(current_token->attribute, "length") == 0) {
            printf("length\n");
            // implement length
        } else if (strcmp(current_token->attribute, "concat") == 0) {
            printf("concat\n");
            // implement concat
        } else if (strcmp(current_token->attribute, "substring") == 0) {
            printf("substring\n");
            // implement substring
        } else if (strcmp(current_token->attribute, "strcmp") == 0) {
            printf("strcmp\n");
            // implement strcmp
        } else if (strcmp(current_token->attribute, "ord") == 0) {
            printf("ord\n");
            // implement ord
        } else if (strcmp(current_token->attribute, "chr") == 0) {
            printf("chr\n");
            // implement chr
        } else {
            fprintf(stderr, "Syntax error: Wrong built-in function syntax\n");
            exit(2); // Replace with better error handling if needed
        }
    } else {
        printf("Non-ifj. function\n");
    }

    // provizorne prechadzanie... fix potom
    next_token();
    print_token();

    next_token();
    print_token();

    next_token();
    print_token();

    next_token();
    print_token();

    return 0;
}
/*
 * Grammar: <if_statement> → "if" <expression> "{" <statement> "}" <else_statement>
 *          | "if" <nullable_expression> "|" <non_null_id> "|" "{" <statement> "}" <else_statement>
 */
int if_statement() {
    printf("-------------------------\n");
    printf("<if_statement>\n");

    next_token();
    print_token();
    expect_type(tok_t_lpa); // "("

    next_token();
    print_token();

    // Handle expression or nullable_expression
    // expression();

    next_token();
    print_token();
    expect_type(tok_t_rpa); // ")"

    next_token();
    print_token();


    if (current_token->type == tok_t_alias) { // "|"
        // Handle nullable_expression
        next_token();
        print_token();
        expect_type(tok_t_sym); // non_null_id

        next_token();
        print_token();
        expect_type(tok_t_alias); // "|"

        next_token();
        print_token();
    }

    expect_type(tok_t_lcbr); // "{"

    next_token();
    print_token();

    // <statement>
    statement();

    // "}"
    expect_type(tok_t_rcbr);
    next_token();

    // <else_statement>
    if (current_token->type == tok_t_else) {
        next_token();
        print_token();
        expect_type(tok_t_lcbr); // "{"

        next_token();
        print_token();

        // <statement>
        statement();

        // "}"
        expect_type(tok_t_rcbr);
        next_token();
    }

    return 0;
}
/*
 * Grammar: <while_statement> → "while" <expression> "{" <statement> "}"
 *          | "while" <nullable_expression> "|" <non_null_id> "|" "{" <statement> "}"
 */
int while_statement() {
    printf("-------------------------\n");
    printf("<while_statement>\n");

    next_token();
    print_token();
    expect_type(tok_t_lpa); // "("

    next_token();
    print_token();

    // Handle expression or nullable_expression
    // expression();

    next_token();
    print_token();
    expect_type(tok_t_rpa); // ")"

    next_token();
    print_token();

    if (current_token->type == tok_t_alias) { // "|"
        // Handle nullable_expression
        next_token();
        print_token();
        expect_type(tok_t_sym); // non_null_id

        next_token();
        print_token();
        expect_type(tok_t_alias); // "|"

        next_token();
        print_token();
    }

    expect_type(tok_t_lcbr); // "{"

    next_token();
    print_token();

    // <statement>
    statement();

    // "}"
    expect_type(tok_t_rcbr);
    next_token();

    return 0;
}
/*
 * Grammar: <for_statement> → "for" <expression> "|" <id> "|" "{" <statement> "}"
 */
int for_statement() {
    printf("-------------------------\n");
    printf("<for_statement>\n");

    next_token();
    print_token();
    expect_type(tok_t_lpa); // "("

    next_token();
    print_token();

    // Handle expression
    // expression();

    next_token();
    print_token();
    expect_type(tok_t_rpa); // ")"

    next_token();
    print_token();
    expect_type(tok_t_alias); // "|"

    next_token();
    print_token();
    expect_type(tok_t_sym); // id

    next_token();
    print_token();
    expect_type(tok_t_alias); // "|"

    next_token();
    print_token();
    expect_type(tok_t_lcbr); // "{"

    next_token();
    print_token();

    // <statement>
    statement();

    // "}"
    expect_type(tok_t_rcbr);
    next_token();

    return 0;
}
/*
 * Grammar: <return_statement> → "return" <expression> ";" | "return" ";"
 */
int return_statement() {
    printf("-------------------------\n");
    printf("<return_statement>\n");

    next_token();
    print_token();

    if (current_token->type != tok_t_semicolon) {
        // Handle expression
        // expression();

        // Check if the expression type matches the function return type
        if (current_token->type != fn_ret_type) {
            fprintf(stderr, "Semantic error: Return type mismatch\n");
            exit(4); // Semantic error - wrong return type
        }

        next_token();
        print_token();
    } else if (fn_ret_type != tok_t_void) {
        fprintf(stderr, "Semantic error: Missing return expression in non-void function\n");
        exit(6); // Semantic error - missing return expression
    }

    expect_type(tok_t_semicolon); // ";"
    next_token();

    return 0;
}
/*
 * Grammar: <break_statement> → "break" ";" | "break" <label> ";"
 */
int break_statement() {
    printf("-------------------------\n");
    printf("<break_statement>\n");

    next_token();
    print_token();

    if (current_token->type == tok_t_sym) {
        // Handle labeled break
        char* label = current_token->attribute;
        next_token();
        print_token();
        expect_type(tok_t_semicolon); // ";"
        printf("Break with label: %s\n", label);
        // Implement logic to handle labeled break
    } else {
        // Handle unlabeled break
        expect_type(tok_t_semicolon); // ";"
        printf("Break without label\n");
        // Implement logic to handle unlabeled break
    }

    next_token();
    return 0;
}
/*
 * Grammar: <continue_statement> → "continue" ";" | "continue" <label> ";"
 */
int continue_statement() {
    printf("-------------------------\n");
    printf("<continue_statement>\n");

    next_token();
    print_token();

    if (current_token->type == tok_t_sym) {
        // Handle labeled continue
        char* label = current_token->attribute;
        next_token();
        print_token();
        expect_type(tok_t_semicolon); // ";"
        printf("Continue with label: %s\n", label);
        // Implement logic to handle labeled continue
    } else {
        // Handle unlabeled continue
        expect_type(tok_t_semicolon); // ";"
        printf("Continue without label\n");
        // Implement logic to handle unlabeled continue
    }

    next_token();
    return 0;
}

/******** End of grammar functions ******** 
 * Function to parse the source code
 */
void parse() {
    printf("No nazdar!\n");

    // Initialize the scanner with the source code file.
    scanner = scn_init("test/scanner/constructions.zig");
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