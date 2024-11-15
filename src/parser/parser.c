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
#include <stdarg.h>
#include "parser.h"
#include "../scanner/scanner.h"
#include "../utils/token.h"
#include "../utils/token_types.h"
#include "../utils/errors.h"
#include "../utils/codegen.h"
#include "../utils/stack.h"


#define OK if (error != err_none) return

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
err_codes error = err_none;

Stack stack_codegen;
int counter_codegen = 0;
char string_buffer[100];

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
void print_postfix_and_stack(char *postfix, int postfix_index, Stack *stack) {
    printf("Postfix: ");
    for (int i = 0; i < postfix_index; i++) {
        printf("%c ", postfix[i]);
    }
    printf("\nStack: ");
    for (int i = 0; i <= stack->top; i++) {
        printf("%s ", tok_type_to_str(stack->items[i]->type));
    }
    printf("\n");
}
/* 
 * Function to print postfix expression
 */
void print_postfix(Token_ptr *postfix, int postfix_index) {
    printf("Postfix expression: ");
    for (int i = 0; i < postfix_index; i++) {
        printf("%s ", tok_type_to_str(postfix[i]->type));
    }
    printf("\n");
}
/* 
 * Function to parse the expression
 */
void parse_expression(Token_ptr stored_token) {
    printf(format[_comment], "<expression>");

    Stack stack;
    init(&stack);

    Token_ptr postfix[MAX];
    int postfix_index = 0;

    bool first_iteration = true;
    int paren_count = 0;  // Na sledovanie počtu otvorených zátvoriek

    while (current_token->type != tok_t_semicolon) {
        Token_ptr token_to_use = first_iteration ? stored_token : current_token;

        // TO-DO: Implement functions for precedence parsing

        // Ak je to pravá zátvorka
        if (token_to_use->type == tok_t_rpa) {
            paren_count--;
            // Ak máme 0 zátvoriek, skončíme spracovanie
            if (paren_count <= 0) {
                break;  // Končíme spracovanie podmienky
            }
            // Inak pokračujeme v spracovaní výrazu
            while (!isEmpty(&stack) && peek(&stack)->type != tok_t_lpa) {
                postfix[postfix_index++] = pop(&stack);
            }
            pop(&stack); // Discard the left parenthesis
        } 
        // Ak je to ľavá zátvorka, zvýšime počet zátvoriek 
        else if (token_to_use->type == tok_t_lpa) { 
            paren_count++;
            push(&stack, token_to_use);
        } 
        // Operand handling (čísla, symboly, premenné)
        else if (token_to_use->type == tok_t_int || token_to_use->type == tok_t_flt || token_to_use->type == tok_t_sym) {
            postfix[postfix_index++] = token_to_use;
        } 
        // Operator handling
        else {
            while (!isEmpty(&stack) && getPrecedence(peek(&stack)->type, token_to_use->type) != '<') {
                postfix[postfix_index++] = pop(&stack);
            }
            push(&stack, token_to_use);
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
    printf(format[_comment], "</expression>");
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
void next_token_initial() {
    if (current_token != NULL) {
        tok_free(current_token);
    }
    current_token = scn_scan(scanner);
    if (current_token->type == tok_t_error) {
        tok_free(current_token);
        current_token = NULL;
        error = err_lexic;
    }
}

void next_token() {
    current_token = scn_scan(scanner);
}
/* 
 * Function to check if the current token is of the expected type
 */
void expect_type(token_type type) {
    if (current_token->type != type) {
        fprintf(stderr, "Syntax error1: Expected %s, got %s\n", tok_type_to_str(type), tok_type_to_str(current_token->type));
        error = err_syntax;
    }
}
/*
* Function to check if the current token is of the expected type
*/
void expect_types(int count, ...) {
    va_list args;
    va_start(args, count);
    bool found = false;
    token_type expected_types[count];
    // První průchod - kontrola shody a uložení typů
    for (int i = 0; i < count; i++) {
        token_type type = va_arg(args, token_type);
        expected_types[i] = type;
        
        if (current_token->type == type) {
            found = true;
            break;
        }
    }
    
    va_end(args);
    
    // Pokud nebyla nalezena shoda, vytvoříme chybovou hlášku
    if (!found) {
        fprintf(stderr, "Syntax error: Expected one of: ");
        // Výpis všech očekávaných typů
        for (int i = 0; i < count; i++) {
            fprintf(stderr, "%s", tok_type_to_str(expected_types[i]));
            if (i < count - 1) {
                fprintf(stderr, ", ");
            }
        }
        fprintf(stderr, ", but got %s\n", tok_type_to_str(current_token->type));
        error = err_syntax;
    }
}
/* 
 * Function to check if the current token has the expected attribute
 */
void expect_attribute(const char* attr) {
    printf("Expect_attribute: %s\n", attr);
    if (current_token->attribute == NULL) {
        fprintf(stderr, "Syntax error2: Expected %s, but current token has no attribute\n", attr);
        error = err_syntax;
    }
    if (strcmp(current_token->attribute, attr) != 0) {
        fprintf(stderr, "Syntax error3: Expected %s, got %s\n", attr, current_token->attribute);
        error = err_syntax;
    }
}

/************** Grammar functions ************** 
 *  Function to parse the <program> non-terminal
 */
void program() {
    printf(format[_comment], "<program>");

    next_token();
    prolog(); OK;

    printf(format[_jump], "$main");

    next_token();
    function(); OK;

    printf(format[_label], "&$main");

    printf(format[_comment], "</program>");
}
/* 
 *  Grammar: <prolog> → "const" <ID> "=" "@import" "(" <STR> ")" ";"
 */
void prolog() {
    printf(format[_comment], "<prolog>");

    // "const"
    expect_type(tok_t_const); OK;  

    // "ID"
    next_token();
    expect_type(tok_t_sym); OK; 

    // "="
    next_token();
    expect_type(tok_t_ass); OK;

    // "@import"
    next_token();
    expect_type(tok_t_import); OK; 

    // "("
    next_token();
    expect_type(tok_t_lpa); OK;

    // "STRING"
    next_token();
    expect_type(tok_t_str); OK;

    // ")"
    next_token();
    expect_type(tok_t_rpa); OK;

    // ";"
    next_token();
    expect_type(tok_t_semicolon); OK;

    printf(".ifjcode24\n");

    printf(format[_comment], "</prolog>");
}
/* 
 *  Grammar: <function> → "pub" "fn" <ID> "(" <parameter> ")" <return_type> "{" <statement> "}" <function_next>
 */
void function() {
    printf(format[_comment], "<function>");

    if (first_parse_done){
        expect_type(tok_t_pub); OK; // "pub"

        next_token();
    }

    expect_type(tok_t_fn); OK; // "fn"

    next_token();
    expect_type(tok_t_sym); OK; // "ID"

    sprintf(string_buffer, "$%s", current_token->attribute);
    printf(format[_label], string_buffer);
    printf(format[_pushframe]);
    push(&stack_codegen, current_token);

    // Parsing first time: Add function to TS
    if (!first_parse_done)
    {
        // TS add function
    }

    next_token(); // "("
    expect_type(tok_t_lpa); OK;  

    next_token();
    parameter(); OK;
    counter_codegen = 0;
    
    expect_type(tok_t_rpa); OK; // ")"
    
    next_token();
    return_type(); OK;

    next_token();
    expect_type(tok_t_lcbr); OK; // "{"

    // Parsing second time: Parse function body
    if (first_parse_done) {
        next_token();
        body(); OK;
    } else {
        // Handle nested braces
        int brace_count = 1; // We already encountered one '{'

        while (brace_count > 0) {
            next_token();

            if (current_token->type == tok_t_lcbr) {
                brace_count++;
            } else if (current_token->type == tok_t_rcbr) {
                brace_count--;
            }
        }
    }

    // "}"
    expect_type(tok_t_rcbr); OK;

    printf(format[_popframe]);
    if (pop(&stack_codegen)->attribute == "main") {
        printf(format[_jump], "&$main");
    } else {
        printf(format[_return]);
    }

    // Next function or EOF
    if (first_parse_done) {
        next_token();
        function_next(); OK;
    }

    printf(format[_comment], "</function>");
}
/*
 * Grammar: <function_next> → EOF | <function>
 */
void function_next() {
    printf(format[_comment], "<function_next>");

    if (current_token->type != tok_t_eof) {
        function(); OK;
    } else {
        expect_type(tok_t_eof); OK;
    }
    printf(format[_comment], "</function_next>");
}
/*
 * Grammar: <parameter> → ɛ | <ID> : <ID_type> <parameter_next>
 */
void parameter() {
    printf(format[_comment], "<parameter>");

    if (current_token->type == tok_t_sym) {
        printf(format[_defvar], "LF@%s", current_token->attribute);
        printf("MOVE LF@%s LF@%%%i\n", current_token->attribute, counter_codegen++);

        next_token();
        expect_type(tok_t_colon); OK; // ":"

        next_token();
        if (current_token->type == tok_t_i32 || current_token->type == tok_t_f64 || current_token->type == tok_t_u8 || current_token->type == tok_t_bool || current_token->type == tok_t_i32_opt || current_token->type == tok_t_f64_opt || current_token->type == tok_t_u8_opt) {
            next_token();
            parameter_next(); OK;
        } else {
            fprintf(stderr, "Syntax error: Expected type, got %s\n", tok_type_to_str(current_token->type));
            error = err_syntax;
        }
    }
    printf(format[_comment], "</parameter>");
}
/*
 * Grammar: <parameter_next> → ɛ | "," <ID> : <ID_type> <parameter_next>
 */
void parameter_next() {
    printf(format[_comment], "<parameter_next>");

    if (current_token->type == tok_t_com) {
        next_token();
        parameter(); OK;
    }
    printf(format[_comment], "</parameter_next>");
}
/*
 * Grammar: <body> → ɛ | <statement> <body>
 */
void body() {
    printf(format[_comment], "<body>");
    /* Toto je správně, ale zatím to necháme takto
    while (current_token->type != tok_t_rcbr) {
        statement(); OK;
    }*/
    if (current_token->type != tok_t_rcbr) {
        statement(); OK;
        body(); OK;
    }

    printf(format[_comment], "</body>");
}
/*
 * Grammar: <statement> → <id_statement> | <declaration> | <if_statement> | <while_statement> | <for_statement> | <return_statement> | <break_statement> | <continue_statement>
 */
void statement() {
    printf(format[_comment], "<statement>");

    switch (current_token->type) {
    case tok_t_unused: //toto se v podstatě nemusí generovat ani kontrolovat
        next_token();
        expect_type(tok_t_ass); OK;

        next_token();
        value(); OK;

        expect_type(tok_t_semicolon); OK;

        next_token();
        break;

    case tok_t_sym:
        push(&stack_codegen, current_token);

        next_token();
        id_statement(); OK;

        expect_type(tok_t_semicolon); OK;

        next_token();
        break;

    case tok_t_const:
    case tok_t_var:
        printf(format[_comment], "<prefix>");
        printf(format[_comment], "</prefix>");

        next_token();
        expect_type(tok_t_sym); OK; // ID

        sprintf(string_buffer, "LF@%s", current_token->attribute);
        printf(format[_defvar], string_buffer);
        
        next_token();
        definition(); OK;
        
        expect_type(tok_t_ass); OK;

        next_token();
        value(); OK;

        expect_type(tok_t_semicolon); OK;

        next_token();
        break;

    case tok_t_if:
        next_token(); // (
        expect_type(tok_t_lpa); OK;

        next_token();
        value(); OK;

        expect_type(tok_t_rpa); OK;

        next_token();
        not_null_value(); OK;

        then(); OK;

        else_then(); OK;
        break;

    case tok_t_while:
        next_token();
        expect_type(tok_t_lpa); OK;

        next_token();
        value(); OK;

        expect_type(tok_t_rpa); OK;

        next_token();
        not_null_value(); OK;

        then(); OK;

        else_then(); OK;
        break;

    case tok_t_for:
        next_token();
        expect_type(tok_t_lpa); OK;

        next_token();
        expect_type(tok_t_sym); OK;

        next_token();
        id_continue(); OK;

        expect_type(tok_t_rpa); OK;
       
        next_token();
        expect_type(tok_t_alias); OK;
 
        next_token();
        expect_type(tok_t_sym); OK;

        next_token();
        expect_type(tok_t_alias); OK;

        next_token();
        then(); OK;
        break;

    case tok_t_return:
        next_token();
        return_value(); OK;

        expect_type(tok_t_semicolon); OK;

        next_token();
        break;
    case tok_t_break:
        next_token();
        expect_type(tok_t_semicolon); OK;

        next_token();
        break;
    case tok_t_continue:
        next_token();
        expect_type(tok_t_semicolon); OK;

        next_token();
        break;

    default:
        fprintf(stderr, "Syntax error: Expected statement, got %s\n", tok_type_to_str(current_token->type));
        error = err_syntax;
    }

    printf(format[_comment], "</statement>");
}
void id_statement() {
    printf(format[_comment], "<id_statement>");

    if (current_token->type == tok_t_ass) {
        sprintf(string_buffer, "MOVE LF@%s", pop(&stack_codegen)->attribute);

        next_token();
        value(); OK;
    } else {
        call(); OK;

        next_token();
    }

    printf(format[_comment], "</id_statement>");
}
/*
 * Grammar: <value> → <expression> | <literal> | <ID>
 */
void value() {
    printf(format[_comment], "<value>");

    // Store the current token into a single variable
    if (stored_token == NULL) {
        stored_token = malloc(sizeof(Token));
        if (stored_token == NULL) {
            fprintf(stderr, "Memory allocation error");
            error = err_internal;
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

    next_token();

    // test for expression 
    if (current_token->type != tok_t_lpa && current_token->type != tok_t_rpa && current_token->type != tok_t_semicolon && current_token->type != tok_t_dot && current_token->type != tok_t_com) {
        parse_expression(stored_token); OK;
    } else {
        switch (stored_token->type) {
            case tok_t_null:
                printf("%s nil@nil\n", string_buffer);
                break;
            case tok_t_int:
                printf("%s int@%i\n", string_buffer, (int)atoi(stored_token->attribute));
                break;
            case tok_t_flt:
                printf("%s float@%a\n", string_buffer, (float)atof(stored_token->attribute));
                break;
            case tok_t_bool:
                printf("%s bool@%s\n", string_buffer, stored_token->attribute);
                break;
                // ;
                // TS
                break;

            case tok_t_as:
                expect_type(tok_t_lpa); OK;

                next_token();
                expect_type(tok_t_i32); OK;

                next_token();
                expect_type(tok_t_com); OK;

                next_token();
                expect_type(tok_t_sym); OK;

                next_token();
                expect_type(tok_t_rpa); OK;



                next_token();
                break;

            case tok_t_sym:
                id_continue(); OK;
                /*if (strcmp(stored_token->attribute, "true") == 0 || strcmp(stored_token->attribute, "false") == 0) {
                    print_token(); OK; // ;
                } else { // ID
                    // TS
                }*/
                break;
            default:
                fprintf(stderr, "Syntax error: Expected value, got %s\n", tok_type_to_str(current_token->type));
                error = err_syntax;
        }
    }

    printf(format[_comment], "</value>");
}
/*
 * Grammar: <not_null_value> → <ID> <id_continue> | <STRING> | <value>
 */
void not_null_value() {
    printf(format[_comment], "<not_null_value>");

    if (current_token->type == tok_t_alias) {
        next_token();
        expect_type(tok_t_sym); OK; // ID

        next_token();
        expect_type(tok_t_alias); OK; // |

        next_token();
    }

    printf(format[_comment], "</not_null_value>");
}
void then() {
    printf(format[_comment], "<then>");

    if (current_token->type == tok_t_lcbr) {
        next_token();
        body(); OK;

        expect_type(tok_t_rcbr); OK;

        next_token();
    } else {
        expect_types(10, tok_t_sym, tok_t_unused, tok_t_const, tok_t_var, tok_t_if, tok_t_while, tok_t_for, tok_t_return, tok_t_break, tok_t_continue); OK;
        statement(); OK;
    }

    printf(format[_comment], "</then>");
}
/*
 * Grammar: <else_then> → ɛ | "else" <then>
 */
void else_then() {
    printf(format[_comment], "<else_then>");

    if (current_token->type == tok_t_else) {
        next_token();
        then(); OK;
    }
    printf(format[_comment], "</else_then>");
}
/*
 * Grammar: <id_continue> → "." <ID> <call> | "(" <call_params> ")"
 */
void id_continue() {
    printf(format[_comment], "<id_continue>");

    if (current_token->type == tok_t_dot || current_token->type == tok_t_lpa) {
        call(); OK;

        next_token();
    }

    printf(format[_comment], "</id_continue>");
}
/*
 * Grammar: <return_value> → ɛ | <value>
 */
void return_value() {
    printf(format[_comment], "<return_value>");

    if (current_token->type != tok_t_semicolon) {
        value(); OK;
    }

    printf(format[_comment], "</return_value>");
}
/*
 * Grammar: <call> → "." <ID> <call_params> | "(" <call_params> ")"
 */
void call() {
    printf(format[_comment], "<call>");

    printf(format[_createframe]);

    if (current_token->type == tok_t_dot) { //a.a()
        next_token();
        expect_type(tok_t_sym); OK;

        next_token();
        expect_type(tok_t_lpa); OK;

        next_token();
        call_params(); OK;

        expect_type(tok_t_rpa); OK;
    } else { //a()
        expect_type(tok_t_lpa); OK;

        next_token();
        call_params(); OK;

        expect_type(tok_t_rpa); OK;

        sprintf(string_buffer, "$%s", pop(&stack_codegen)->attribute);
        printf(format[_call], string_buffer);
    }

    printf(format[_comment], "</call>");
}
/*
 * Grammar: <call_params> → ɛ | <call_value> <call_params_next>
 */
void call_params() {
    printf(format[_comment], "<call_params>");

    if (current_token->type != tok_t_rpa) {
        call_value(); OK;

        call_params_next(); OK;

        counter_codegen = 0;
    }

    printf(format[_comment], "</call_params>");
}
/*
 * Grammar: <call_value> → <value> | <ID> <call_params> | <STRING> <call_params>
 */
void call_value() {
    printf(format[_comment], "<call_value>");

    sprintf(string_buffer, "TF@%%%i", counter_codegen);
    printf(format[_defvar], string_buffer);
    sprintf(string_buffer, "MOVE TF@%%%i", counter_codegen++);
    if (current_token->type == tok_t_str) {
        printf("%s string@", string_buffer);
        printi_string(current_token);
        printf("\n");

        next_token();
    } else {
        value(); OK;
    }

    printf(format[_comment], "</call_value>");
}
/*
 * Grammar: <call_params_next> → ɛ | "," <call_params>
 */
void call_params_next() {
    printf(format[_comment], "<call_params_next>");

    if (current_token->type == tok_t_com) {
        next_token();
        call_params(); OK;
    }
    printf(format[_comment], "</call_params_next>");
}

void return_type() {
    printf(format[_comment], "<return_type>");

    if (current_token->type != tok_t_void) {
        type(); OK;
        
        printf(format[_defvar], "LF@%retval");
    }

    printf(format[_comment], "</return_type>");
}
void type() {
    printf(format[_comment], "<type>");

    expect_types(7, tok_t_i32, tok_t_f64, tok_t_u8, tok_t_bool, tok_t_i32_opt, tok_t_f64_opt, tok_t_u8_opt); OK;

    printf(format[_comment], "</type>");
}

void definition() {
    printf(format[_comment], "<definition>");

    if (current_token->type == tok_t_colon) {
        next_token();
        type(); OK;

        next_token();
    }

    printf(format[_comment], "</definition>");
}

/*
 * Function to parse code for the first time
 *//*
void parse_fn_first() {

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
}*/


/*
 * Function to parse code for the first time
 */
void parse_fn_first() {
    // Get the first token.
    next_token_initial(); OK;
    // Iterate through program
    while (current_token->type != tok_t_eof) {
        if (current_token->type == tok_t_fn) {
            next_token_initial(); OK;
            expect_type(tok_t_sym); OK;

            next_token_initial(); OK;
            expect_type(tok_t_lpa); OK; // (

            next_token_initial(); OK;
            if (current_token->type != tok_t_rpa) { //)   ???
                expect_type(tok_t_sym); OK;

                next_token_initial(); OK;
                expect_type(tok_t_colon); OK;

                next_token_initial(); OK;
                expect_types(7, tok_t_i32, tok_t_f64, tok_t_u8, tok_t_bool, tok_t_i32_opt, tok_t_f64_opt, tok_t_u8_opt); OK;

                next_token_initial(); OK;
            }
            
            while (current_token->type != tok_t_rpa) { // ) ???
                next_token_initial(); OK;
                expect_type(tok_t_com); OK;

                next_token_initial(); OK;
                expect_type(tok_t_sym); OK;

                next_token_initial(); OK;
                expect_type(tok_t_colon); OK;

                next_token_initial(); OK;
                expect_types(7, tok_t_i32, tok_t_f64, tok_t_u8, tok_t_bool, tok_t_i32_opt, tok_t_f64_opt, tok_t_u8_opt); OK;

                next_token_initial(); OK;
            }

            next_token_initial(); OK;
            expect_types(4, tok_t_i32, tok_t_f64, tok_t_u8, tok_t_void); OK;
        }
        next_token_initial(); OK;
    }
    // set next token for second parse
    tok_free(current_token);
}


/******** End of grammar functions ******** 
 * Function to parse the source code
 */
void parse() {
    // Parse the source code for the first time.
    parse_fn_first();

    if (error == err_none) {
        // Set the first_parse_done flag to true.
        first_parse_done = true;

        // Initialize the stack for code generation
        init(&stack_codegen);

        // Start parsing second time from the <program> non-terminal.
        program();
    }

    // Free the stored_token
    if (stored_token != NULL) {
        if (stored_token->attribute != NULL) {
            free(stored_token->attribute);
        }
        free(stored_token);
    }
}