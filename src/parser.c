#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "runtime.h"

static Token tokens[5000];

static int token_count = 0;

static int current = 0;

static Token current_token() {

    return tokens[current];

}

static void advance() {

    current++;

}

static void eat(TokenType type) {

    if (current_token().type == type) {

        advance();

    }

    else {

        printf("Unexpected token: %s\n", current_token().value);

        exit(1);

    }

}

static int get_number_value() {

    if (current_token().type == TOKEN_NUMBER) {

        int value = atoi(current_token().value);

        eat(TOKEN_NUMBER);

        return value;

    }

    else if (current_token().type == TOKEN_IDENTIFIER) {

        int value = atoi(runtime_get_variable(current_token().value));

        eat(TOKEN_IDENTIFIER);

        return value;

    }

    printf("Expected number\n");

    exit(1);

}

static void parse_statement();

static void parse_block() {

    eat(TOKEN_LBRACE);

    while (current_token().type != TOKEN_RBRACE) {

        parse_statement();

    }

    eat(TOKEN_RBRACE);

}

static void parse_stamp() {

    eat(TOKEN_STAMP);

    eat(TOKEN_LPAREN);

    if (current_token().type == TOKEN_STRING) {

        runtime_stamp(current_token().value);

        eat(TOKEN_STRING);

    }

    else if (current_token().type == TOKEN_IDENTIFIER) {

        runtime_stamp(
            runtime_get_variable(current_token().value)
        );

        eat(TOKEN_IDENTIFIER);

    }

    else if (current_token().type == TOKEN_NUMBER) {

        runtime_stamp(current_token().value);

        eat(TOKEN_NUMBER);

    }

    eat(TOKEN_RPAREN);

}

static void parse_variable() {

    char name[100];

    strcpy(name, current_token().value);

    eat(TOKEN_IDENTIFIER);

    eat(TOKEN_ASSIGN);

    if (current_token().type == TOKEN_STRING) {

        runtime_set_variable(
            name,
            current_token().value
        );

        eat(TOKEN_STRING);

    }

    else {

        int result = get_number_value();

        while (
            current_token().type == TOKEN_PLUS ||
            current_token().type == TOKEN_MINUS
        ) {

            if (current_token().type == TOKEN_PLUS) {

                eat(TOKEN_PLUS);

                result += get_number_value();

            }

            else {

                eat(TOKEN_MINUS);

                result -= get_number_value();

            }

        }

        char buffer[100];

        sprintf(buffer, "%d", result);

        runtime_set_variable(name, buffer);

    }

}

static int parse_condition() {

    int left = get_number_value();

    if (current_token().type == TOKEN_LESS) {

        eat(TOKEN_LESS);

        return left < get_number_value();

    }

    else if (current_token().type == TOKEN_GREATER) {

        eat(TOKEN_GREATER);

        return left > get_number_value();

    }

    else if (current_token().type == TOKEN_EQUAL_EQUAL) {

        eat(TOKEN_EQUAL_EQUAL);

        return left == get_number_value();

    }

    return 0;

}

static void parse_if() {

    eat(TOKEN_IF);

    int condition = parse_condition();

    if (condition) {

        parse_block();

        if (current_token().type == TOKEN_ELSE) {

            eat(TOKEN_ELSE);

            eat(TOKEN_LBRACE);

            int braces = 1;

            while (braces > 0) {

                if (current_token().type == TOKEN_LBRACE)
                    braces++;

                else if (current_token().type == TOKEN_RBRACE)
                    braces--;

                advance();

            }

        }

    }

    else {

        eat(TOKEN_LBRACE);

        int braces = 1;

        while (braces > 0) {

            if (current_token().type == TOKEN_LBRACE)
                braces++;

            else if (current_token().type == TOKEN_RBRACE)
                braces--;

            advance();

        }

        if (current_token().type == TOKEN_ELSE) {

            eat(TOKEN_ELSE);

            parse_block();

        }

    }

}

static void execute_saved_block(
    int block_start,
    int block_end
) {

    int saved = current;

    current = block_start;

    while (current < block_end) {

        parse_statement();

    }

    current = saved;

}

static void parse_flow() {

    eat(TOKEN_FLOW);

    char loop_var[100];

    strcpy(loop_var, current_token().value);

    eat(TOKEN_IDENTIFIER);

    TokenType op = current_token().type;

    advance();

    int compare_value = get_number_value();

    eat(TOKEN_LBRACE);

    int block_start = current;

    int braces = 1;

    while (braces > 0) {

        if (current_token().type == TOKEN_LBRACE)
            braces++;

        else if (current_token().type == TOKEN_RBRACE)
            braces--;

        advance();

    }

    int block_end = current - 1;

    while (1) {

        int left = atoi(
            runtime_get_variable(loop_var)
        );

        int condition = 0;

        if (op == TOKEN_LESS)
            condition = left < compare_value;

        else if (op == TOKEN_GREATER)
            condition = left > compare_value;

        else if (op == TOKEN_EQUAL_EQUAL)
            condition = left == compare_value;

        if (!condition)
            break;

        execute_saved_block(
            block_start,
            block_end
        );

    }

}

static void parse_statement() {

    if (current_token().type == TOKEN_STAMP) {

        parse_stamp();

    }

    else if (current_token().type == TOKEN_IDENTIFIER) {

        parse_variable();

    }

    else if (current_token().type == TOKEN_IF) {

        parse_if();

    }

    else if (current_token().type == TOKEN_FLOW) {

        parse_flow();

    }

    else {

        printf(
            "Unknown statement: %s\n",
            current_token().value
        );

        exit(1);

    }

}

void parse_program() {

    Token token;

    do {

        token = get_next_token();

        tokens[token_count++] = token;

    }

    while (token.type != TOKEN_EOF);

    current = 0;

    eat(TOKEN_RIVEN);

    eat(TOKEN_CORE);

    parse_block();

}
