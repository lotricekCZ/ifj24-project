/** IFJ2024
 * xondre16; Jozef Ondrejička
 */

// TO-DO:
// 1) built-in funkcie
// 2) semantika
// 3) Iplement second parser

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"
#include "../scanner/scanner.h"
#include "../utils/token.h"
#include "../utils/token_types.h"

// Stack structure
#define MAX 100

/* 
 * Global variables
 */
Token_ptr current_token = NULL;
Token_ptr stored_token = NULL;
Scanner_ptr scanner = NULL;
token_type fn_ret_type;
bool ifj_flag = false;
bool first_parse_done = false;
int statement_index = 1;

/*
 * Structure for stack
 */
typedef struct {
    char items[MAX];
    int top;
} Stack;
/*
 * Precedence table
 */
char precedence_table[17][17] = {
//     +    -    *    /    ==   !=   <    >    <=   >=  and   or   !    (    )    id   $
    { '>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '>', '<', '>' }, // +
    { '>', '>', '<', '<', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '>', '<', '>' }, // -
    { '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '>', '<', '>' }, // *
    { '>', '>', '>', '>', '<', '<', '<', '<', '<', '<', '>', '>', '<', '<', '>', '<', '>' }, // /
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '>' }, // ==
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '>' }, // !=
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '>' }, // <
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '>' }, // >
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '>' }, // <=
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '<', '<', '>', '<', '>' }, // >=
    { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '<', '<', '>', '<', '>' }, // and
    { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '>', '<', '>' }, // or
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '<', '>', '<', '>' }, // !
    { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '=', '<', ' ' }, // (
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>', '>', '>' }, // )
    { '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', '>', ' ', '>' }, // id
    { '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', '<', ' ', '<', ' ' }  // $
};
/* 
 * Function to get the precedence index of the operator
 */
int get_precedence_index(token_type type) {
    switch (type) {
        case tok_t_plus: return 0;
        case tok_t_minus: return 1;
        case tok_t_times: return 2;
        case tok_t_divide: return 3;
        case tok_t_eq: return 4;
        case tok_t_neq: return 5;
        case tok_t_lt: return 6;
        case tok_t_gt: return 7;
        case tok_t_leq: return 8;
        case tok_t_geq: return 9;
        case tok_t_and: return 10;
        case tok_t_or: return 11;
        case tok_t_not: return 12;
        case tok_t_lpa: return 13;
        case tok_t_rpa: return 14;
        case tok_t_sym: return 15;
        case tok_t_eof: return 16;
        default: return -1;
    }
}
/* 
 * Function to get the precedence of the operator
 */
int getPrecedence(token_type stackTop, token_type input) {
    return precedence_table[get_precedence_index(stackTop)][get_precedence_index(input)];
}
/* 
 * Function for empty stack
 */
int isEmpty(Stack *stack) {
    return stack->top == -1;
}
/* 
 * Function to push operator to stack
 */
void push(Stack *stack, token_type value) {
    if (stack->top == MAX - 1) {
        printf("Stack overflow\n");
        return;
    }
    stack->items[++stack->top] = value;
}
/* 
 * Function to pop operator from stack
 */
token_type pop(Stack *stack) {
    if (isEmpty(stack)) {
        printf("Stack underflow\n");
        return tok_t_eof;
    }
    return stack->items[stack->top--];
}
/* 
 * Function to peek the top of the stack
 */
token_type peek(Stack *stack) {
    if (isEmpty(stack)) {
        return tok_t_eof;
    }
    return stack->items[stack->top];
}
/* 
 * Function to print postfix and stack
 */
void print_postfix_and_stack(char *postfix, int postfix_index, Stack *stack) {
    printf("Postfix: ");
    for (int i = 0; i < postfix_index; i++) {
        printf("%c ", postfix[i]);
    }
    printf("\nStack: ");
    for (int i = 0; i <= stack->top; i++) {
        printf("%s ", tok_type_to_str(stack->items[i]));
    }
    printf("\n");
}
/* 
 * Function to print postfix expression
 */
void print_postfix(token_type *postfix, int postfix_index) {
    printf("Postfix expression: ");
    for (int i = 0; i < postfix_index; i++) {
        printf("%s ", tok_type_to_str(postfix[i]));
    }
    printf("\n");
}
/* 
 * Function to parse the expression
 */
void parse_expression(Token_ptr stored_token) {
    Stack stack;
    stack.top = -1;

    token_type postfix[MAX];
    int postfix_index = 0;

    printf("-------------------------\n");
    printf("Expression parsing:\n");

    bool first_iteration = true;
    int paren_count = 0;  // Na sledovanie počtu otvorených zátvoriek

    while (current_token->type != tok_t_semicolon) {
        token_type token_type_to_use = first_iteration ? stored_token->type : current_token->type;

        // TO-DO: Implement functions for precedence parsing

        // Ak je to pravá zátvorka
        if (token_type_to_use == tok_t_rpa) {
            paren_count--;
            // Ak máme 0 zátvoriek, skončíme spracovanie
            if (paren_count <= 0) {
                break;  // Končíme spracovanie podmienky
            }
            // Inak pokračujeme v spracovaní výrazu
            while (!isEmpty(&stack) && peek(&stack) != tok_t_lpa) {
                postfix[postfix_index++] = pop(&stack);
            }
            pop(&stack); // Discard the left parenthesis
        } 
        // Ak je to ľavá zátvorka, zvýšime počet zátvoriek 
        else if (token_type_to_use == tok_t_lpa) { 
            paren_count++;
            push(&stack, tok_t_lpa);
        } 
        // Operand handling (čísla, symboly, premenné)
        else if (token_type_to_use == tok_t_int || token_type_to_use == tok_t_flt || token_type_to_use == tok_t_sym) {
            postfix[postfix_index++] = token_type_to_use;
        } 
        // Operator handling
        else {
            while (!isEmpty(&stack) && getPrecedence(peek(&stack), token_type_to_use) != '<') {
                postfix[postfix_index++] = pop(&stack);
            }
            push(&stack, token_type_to_use);
        }

        if (first_iteration) {
            first_iteration = false;
        } else {
            next_token();
        }
    }

    // Pop remaining operators
    while (!isEmpty(&stack)) {
        postfix[postfix_index++] = pop(&stack);
    }
    print_postfix(postfix, postfix_index);
}
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
        fprintf(stderr, "Syntax error1: Expected %s, got %s\n", tok_type_to_str(type), tok_type_to_str(current_token->type));
        exit(2); // Syntax error
    }
}
/* 
 * Function to check if the current token has the expected attribute
 */
void expect_attribute(const char* attr) {
    printf("Expect_attribute: %s\n", attr);
    if (current_token->attribute == NULL) {
        fprintf(stderr, "Syntax error2: Expected %s, but current token has no attribute\n", attr);
        exit(2); // Syntax error
    }
    if (strcmp(current_token->attribute, attr) != 0) {
        fprintf(stderr, "Syntax error3: Expected %s, got %s\n", attr, current_token->attribute);
        exit(2); // Syntax error
    }
}

/************** Grammar functions ************** 
 *  Function to parse the <program> non-terminal
 */
int program() {
    printf("-------------------------\n");
    printf("<program>\n");
    printf("-------------------------\n");

    prolog();
    function();
    
    return 0;
}
/* 
 *  Grammar: <prolog> → "const" <ID> "=" "@import" "(" <STR> ")" ";"
 */
int prolog() {
    printf("-------------------------\n");
    printf("<prolog>\n");
    printf("-------------------------\n");

    // "const"
    print_token();
    expect_type(tok_t_const);  

    // "ID"
    next_token();
    print_token();
    expect_type(tok_t_sym); 

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

    // "STRING"
    next_token();
    print_token();
    expect_type(tok_t_str);

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
 *  Grammar: <function> → "pub" "fn" <ID> "(" <parameter> ")" <return_type> "{" <statement> "}" <function_next>
 */
int function() {
    printf("-------------------------\n");
    printf("<function>\n");
    printf("-------------------------\n");

    if (first_parse_done){
        next_token();
        print_token(); // "pub"
        expect_type(tok_t_pub);  
    }

    next_token();
    print_token(); // "fn"
    expect_type(tok_t_fn);

    next_token();
    print_token(); // "ID"
    expect_type(tok_t_sym); 

    // Parsing first time: Add function to TS
    if (!first_parse_done)
    {
        // TS add function
    }


    next_token();
    print_token(); // "("
    expect_type(tok_t_lpa);  

    next_token();
    print_token();

    parameter();
    
    // ")"
    expect_type(tok_t_rpa);
    next_token();
    
    // Return type
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

    // Parsing second time: Parse function body
    if (first_parse_done) {
        body();
    } else {
        // Handle nested braces
        int brace_count = 1; // We already encountered one '{'

        while (brace_count > 0) {
            next_token();
            print_token();

            if (current_token->type == tok_t_lcbr) {
                brace_count++;
            } else if (current_token->type == tok_t_rcbr) {
                brace_count--;
            }
        }
    }

    // "}"
    expect_type(tok_t_rcbr);

    // Next function or EOF
    if (first_parse_done) {
        next_token();
        function_next();
    }

    printf("-------------------------\n");
    printf("<function> DONE\n");
    printf("-------------------------\n");
    return 0;
}
/*
 * Grammar: <function_next> → EOF | <function>
 */
int function_next() {
    printf("-------------------------\n");
    printf("<function_next>\n");
    printf("-------------------------\n");
    print_token();  
    if (current_token->type != tok_t_eof) {
        function();
    } else {
        expect_type(tok_t_eof);
    }

    printf("-------------------------\n");
    printf("<function_next> DONE\n");
    printf("-------------------------\n");

    return 0;
}
/*
 * Grammar: <parameter> → ɛ | <ID> : <ID_type> <parameter_next>
 */
int parameter() {
    printf("-------------------------\n");
    printf("<parameter>\n");
    printf("-------------------------\n");

    print_token();
    if (current_token->type == tok_t_sym) {
        next_token();
        print_token();
        expect_type(tok_t_colon);  // ":"

        next_token();
        print_token();
        if (current_token->type == tok_t_i32 || current_token->type == tok_t_f64 || current_token->type == tok_t_u8 || current_token->type == tok_t_bool || current_token->type == tok_t_i32_opt || current_token->type == tok_t_f64_opt || current_token->type == tok_t_u8_opt) {
            next_token();
            print_token();
            parameter_next();
        } else {
            fprintf(stderr, "Syntax error: Expected type, got %s\n", tok_type_to_str(current_token->type));
            exit(2); // Syntax error
        }
    }
    printf("-------------------------\n");
    printf("<parameter> DONE\n");
    printf("-------------------------\n");
    return 0;
}
/*
 * Grammar: <parameter_next> → ɛ | "," <ID> : <ID_type> <parameter_next>
 */
int parameter_next() {
    printf("-------------------------\n");
    printf("<parameter_next>\n");
    printf("-------------------------\n");

    print_token();
    if (current_token->type == tok_t_com) {
        next_token();
        print_token();
        parameter();
    }
    printf("<parameter_next> DONE\n");
    return 0;
}
/*
 * Grammar: <body> → ɛ | <statement> <body>
 */
int body() {
    printf("-------------------------\n");
    printf("<body>\n");
    printf("-------------------------\n");

    while (current_token->type != tok_t_rcbr) {
        statement();
    }
    printf("<body> DONE\n");
    return 0;
}
/*
 * Grammar: <statement> → <id_statement> | <declaration> | <if_statement> | <while_statement> | <for_statement> | <return_statement> | <break_statement> | <continue_statement>
 */
int statement() {
    printf("-------------------------\n");
    printf("<statement(%d)>\n", statement_index++);
    printf("-------------------------\n");

    print_token();
    switch (current_token->type) {
    case tok_t_sym:
        printf("tok_t_sym\n");
        next_token();
        print_token();
        id_statement();
        expect_type(tok_t_semicolon);
        next_token();
        break;
    case tok_t_const:
    case tok_t_var:
        printf("tok_t_const || tok_t_var \n");
        next_token();
        print_token(); // ID

        expect_type(tok_t_sym);
        next_token();
        print_token(); // : | =

        // <definition>
        if (current_token->type == tok_t_colon) {
            next_token();
            print_token(); // <type>

            if (current_token->type == tok_t_i32 || current_token->type == tok_t_f64 || current_token->type == tok_t_u8 || current_token->type == tok_t_bool || current_token->type == tok_t_i32_opt || current_token->type == tok_t_f64_opt || current_token->type == tok_t_u8_opt) {
                next_token();
                print_token(); // =
                // TS
            } else {
                fprintf(stderr, "Syntax error\n");
                exit(2);
            }
        } // end of <definition>

        expect_type(tok_t_ass);
        next_token();

        // <value> 
        value();

        expect_type(tok_t_semicolon);
        next_token();
        print_token();
        break;
    case tok_t_if:
        printf("tok_t_if\n");
        next_token();
        print_token(); // (

        expect_type(tok_t_lpa);
        next_token();
        print_token(); 

        value();

        print_token();
        printf("Meowwww\n");
        expect_type(tok_t_rpa);
        next_token();
        print_token();

        not_null_value();
        then();
        else_then();

        break;
    case tok_t_while:
        printf("tok_t_while\n");
        next_token();
        print_token();
        expect_type(tok_t_lpa);
        next_token();
        print_token();
        value();
        expect_type(tok_t_rpa);
        next_token();
        print_token();
        not_null_value();
        then();
        else_then();
        break;
    case tok_t_for:
        printf("tok_t_for\n");
        next_token();
        print_token();
        expect_type(tok_t_lpa);
        next_token();
        print_token();
        expect_type(tok_t_sym);
        next_token();
        print_token();
        id_continue();
        expect_type(tok_t_rpa);
        next_token();
        print_token();
        expect_type(tok_t_alias);
        next_token();
        print_token();
        expect_type(tok_t_sym);
        next_token();
        print_token();
        expect_type(tok_t_alias);
        next_token();
        print_token();
        then();
        break;
    case tok_t_return:
        printf("tok_t_return\n");
        next_token();
        print_token();
        return_value();
        expect_type(tok_t_semicolon);
        next_token();
        break;
    case tok_t_break:
        printf("tok_t_break\n");
        next_token();
        print_token();
        expect_type(tok_t_semicolon);
        next_token();
        break;
    case tok_t_continue:
        printf("tok_t_continue\n");
        next_token();
        print_token();
        expect_type(tok_t_semicolon);
        next_token();
        break;
    default:
        fprintf(stderr, "Syntax error: Expected statement, got %s\n", tok_type_to_str(current_token->type));
        exit(2);
    }

    printf("-------------------------\n");
    printf("<statement> DONE\n");
    printf("-------------------------\n");

    return 0;
}
int id_statement() {
    printf("-------------------------\n");
    printf("<id_statement>\n");
    printf("-------------------------\n");

    if (current_token->type == tok_t_ass) {
        next_token();
        print_token();
        value();
    } else {
        call();
    }

    printf("-------------------------\n");
    printf("<id_statement> DONE\n");
    printf("-------------------------\n");

    return 0;
}
/*
 * Grammar: <value> → <expression> | <literal> | <ID>
 */
int value() {
    printf("-------------------------\n");
    printf("<value>\n");
    printf("-------------------------\n");

    print_token();

    // Store the current token into a single variable
    if (stored_token == NULL) {
        stored_token = malloc(sizeof(Token));
        if (stored_token == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            exit(1);
        }
    } else {
        // Free the previous attribute if it exists
        if (stored_token->attribute != NULL) {
            free(stored_token->attribute);
        }
    }

    // Copy current_token into stored_token
    stored_token->type = current_token->type;
    if (current_token->attribute != NULL) {
        stored_token->attribute = strdup(current_token->attribute); // Duplicate the attribute string
    } else {
        stored_token->attribute = NULL;
    }

    // Print stored_token type and attribute
    printf("Stored token type: %s\n", tok_type_to_str(stored_token->type));
    printf("Stored token attribute: %s\n", stored_token->attribute ? stored_token->attribute : "(null)");

    next_token();
    print_token();

    // test for expression 
    if (current_token->type != tok_t_rpa && current_token->type != tok_t_semicolon){
        // parse_expression
        printf("Expressiiooon bitch\n");
        parse_expression(stored_token);
    } else {
        printf("No expression\n");
        printf("Stored token type: %s\n", tok_type_to_str(stored_token->type));
        printf("Stored token attribute: %s\n", stored_token->attribute ? stored_token->attribute : "(null)");
        switch (stored_token->type) {
            case tok_t_null:
            case tok_t_int:
            case tok_t_flt:
            case tok_t_bool:
                print_token(); // ;
                // TS
                break;
            case tok_t_as:
                print_token();
                expect_type(tok_t_lpa);
                next_token();
                print_token();
                expect_type(tok_t_i32);
                next_token();
                print_token();
                expect_type(tok_t_com);
                next_token();
                print_token();
                expect_type(tok_t_sym);
                next_token();
                print_token();
                expect_type(tok_t_rpa);
                next_token();
                print_token();
                break;
            case tok_t_sym:
                printf("Meow\n");
                if (strcmp(stored_token->attribute, "true") == 0 || strcmp(stored_token->attribute, "false") == 0) {
                    print_token(); // ;
                } else { // ID
                    // TS
                }
                break;
            default:
                fprintf(stderr, "Syntax error: Expected value, got %s\n", tok_type_to_str(current_token->type));
                exit(2);
        }
    }

    printf("-------------------------\n");
    printf("<value> DONE\n");
    printf("-------------------------\n");

    return 0;
}
/*
 * Grammar: <not_null_value> → <ID> <id_continue> | <STRING> | <value>
 */
int not_null_value() {
    printf("-------------------------\n");
    printf("<not_null_value>\n");
    printf("-------------------------\n");

    if (current_token->type == tok_t_alias) {
        next_token();
        print_token(); // ID

        expect_type(tok_t_sym);
        next_token();
        print_token(); // |

        expect_type(tok_t_alias);
        next_token();
        print_token(); 
    }
    printf("-------------------------\n");
    printf("<not_null_value> DONE\n");
    printf("-------------------------\n");

    return 0;
}
int then() {
    printf("-------------------------\n");
    printf("<then>\n");
    printf("-------------------------\n");

    if (current_token->type == tok_t_lcbr) {
        next_token();
        print_token();
        body();
        expect_type(tok_t_rcbr);
        next_token();
        print_token();
    } else {
        statement();
    }
    printf("-------------------------\n");
    printf("<then> DONE\n");
    printf("-------------------------\n");

    return 0;
}
/*
 * Grammar: <else_then> → ɛ | "else" <then>
 */
int else_then() {
    printf("-------------------------\n");
    printf("<else_then>\n");
    printf("-------------------------\n");

    if (current_token->type == tok_t_else) {
        next_token();
        print_token();
        then();
    }

    return 0;
}
/*
 * Grammar: <id_continue> → "." <ID> <call> | "(" <call_params> ")"
 */
int id_continue() {
    printf("-------------------------\n");
    printf("<id_continue>\n");
    printf("-------------------------\n");
    print_token();

    if (current_token->type == tok_t_dot || current_token->type == tok_t_lpa) {
        call();
    }

    return 0;
}
/*
 * Grammar: <return_value> → ɛ | <value>
 */
int return_value() {
    printf("-------------------------\n");
    printf("<return_value>\n");

    if (current_token->type != tok_t_semicolon) {
        value();
    }

    return 0;
}
/*
 * Grammar: <call> → "." <ID> <call_params> | "(" <call_params> ")"
 */
void call() {
    printf("-------------------------\n");
    printf("<call>\n");

    if (current_token->type == tok_t_dot) {
        next_token();
        print_token();
        expect_type(tok_t_sym);
        next_token();
        print_token();
    }

    expect_type(tok_t_lpa);
    next_token();
    print_token();
    call_params();
    expect_type(tok_t_rpa);
    next_token();
    print_token();
}
/*
 * Grammar: <call_params> → ɛ | <call_value> <call_params_next>
 */
void call_params() {
    printf("-------------------------\n");
    printf("<call_params>\n");

    if (current_token->type != tok_t_rpa) {
        call_value();
        call_params_next();
    }
}
/*
 * Grammar: <call_value> → <value> | <ID> <call_params> | <STRING> <call_params>
 */
void call_value() {
    printf("-------------------------\n");
    printf("<call_value>\n");

    if (current_token->type == tok_t_str) {
        next_token();
        print_token();
    } else {
        value();
    }
}
/*
 * Grammar: <call_params_next> → ɛ | "," <call_params>
 */
void call_params_next() {
    printf("-------------------------\n");
    printf("<call_params_next>\n");

    if (current_token->type == tok_t_com) {
        next_token();
        print_token();
        call_params();
    }
}
/*
 * Function to parse code for the first time
 */
int parse_fn_first() {

    printf("Nazdar sefe!\n");

    // Iterate through program
    while (current_token->type != tok_t_eof) {
        if (current_token->type == tok_t_pub) {
            // in <function> add to TS
            function();
        }
        next_token();
        print_token();
    }

    printf("First parse done!\n");

    // set next token for second parse
    next_token();

    return 0;
}
/******** End of grammar functions ******** 
 * Function to parse the source code
 */
void parse() {
    printf("No nazdar!\n");

    // Get the first token.
    next_token();

    // Parse the source code for the first time.
    parse_fn_first();

    // Set the first_parse_done flag to true.
    first_parse_done = true;

    // Start parsing second time from the <program> non-terminal.
    program();

    // Free the stored_token
    if (stored_token != NULL) {
        if (stored_token->attribute != NULL) {
            free(stored_token->attribute);
        }
        free(stored_token);
    }
}