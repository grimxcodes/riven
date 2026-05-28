#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "runtime.h"

static Token current_token;

static void eat(TokenType type) {

    if (current_token.type == type) {

        current_token = get_next_token();

    }

    else {

        printf("Unexpected token: %s\n", current_token.value);

        exit(1);

    }

}

static int get_number_value() {

    int value = 0;

    if (current_token.type == TOKEN_NUMBER) {

        value = atoi(current_token.value);

        eat(TOKEN_NUMBER);

    }

    else if (current_token.type == TOKEN_IDENTIFIER) {

        value = atoi(runtime_get_variable(current_token.value));

        eat(TOKEN_IDENTIFIER);

    }

    else {

        printf("Expected number\n");

        exit(1);

    }

    return value;

}

static void parse_statement();

static void parse_block() {

    eat(TOKEN_LBRACE);

    while (current_token.type != TOKEN_RBRACE) {

        parse_statement();

    }

    eat(TOKEN_RBRACE);

}

static void parse_stamp() {

    eat(TOKEN_STAMP);

    eat(TOKEN_LPAREN);

    if (current_token.type == TOKEN_STRING) {

        char* text = strdup(current_token.value);

        eat(TOKEN_STRING);

        runtime_stamp(text);

    }

    else if (current_token.type == TOKEN_IDENTIFIER) {

        char* name = strdup(current_token.value);

        eat(TOKEN_IDENTIFIER);

        runtime_stamp(runtime_get_variable(name));

    }

    else if (current_token.type == TOKEN_NUMBER) {

        char* number = strdup(current_token.value);

        eat(TOKEN_NUMBER);

        runtime_stamp(number);

    }

    eat(TOKEN_RPAREN);

}

static void parse_variable() {

    char* name = strdup(current_token.value);

    eat(TOKEN_IDENTIFIER);

    eat(TOKEN_ASSIGN);

    if (current_token.type == TOKEN_STRING) {

        char* value = strdup(current_token.value);

        eat(TOKEN_STRING);

        runtime_set_variable(name, value);

    }

    else {

        int left = get_number_value();

        while (
            current_token.type == TOKEN_PLUS ||
            current_token.type == TOKEN_MINUS
        ) {

            if (current_token.type == TOKEN_PLUS) {

                eat(TOKEN_PLUS);

                int right = get_number_value();

                left = left + right;

            }

            else if (current_token.type == TOKEN_MINUS) {

                eat(TOKEN_MINUS);

                int right = get_number_value();

                left = left - right;

            }

        }

        char result[100];

        sprintf(result, "%d", left);

        runtime_set_variable(name, result);

    }

}

static int parse_condition() {

    int left = get_number_value();

    if (current_token.type == TOKEN_GREATER) {

        eat(TOKEN_GREATER);

        int right = get_number_value();

        return left > right;

    }

    else if (current_token.type == TOKEN_LESS) {

        eat(TOKEN_LESS);

        int right = get_number_value();

        return left < right;

    }

    else if (current_token.type == TOKEN_EQUAL_EQUAL) {

        eat(TOKEN_EQUAL_EQUAL);

        int right = get_number_value();

        return left == right;

    }

    return 0;

}

static void skip_block() {

    int braces = 1;

    while (braces > 0) {

        current_token = get_next_token();

        if (current_token.type == TOKEN_LBRACE)
            braces++;

        else if (current_token.type == TOKEN_RBRACE)
            braces--;

    }

}

static void parse_if() {

    eat(TOKEN_IF);

    int condition = parse_condition();

    if (condition) {

        parse_block();

    }

    else {

        eat(TOKEN_LBRACE);

        skip_block();

    }

    if (current_token.type == TOKEN_ELSE) {

        eat(TOKEN_ELSE);

        if (!condition) {

            parse_block();

        }

        else {

            eat(TOKEN_LBRACE);

            skip_block();

        }

    }

}

static void parse_flow() {

    eat(TOKEN_FLOW);

    char* condition_var = strdup(current_token.value);

    eat(TOKEN_IDENTIFIER);

    TokenType condition_type = current_token.type;

    if (condition_type == TOKEN_LESS) {

        eat(TOKEN_LESS);

    }

    else if (condition_type == TOKEN_GREATER) {

        eat(TOKEN_GREATER);

    }

    else if (condition_type == TOKEN_EQUAL_EQUAL) {

        eat(TOKEN_EQUAL_EQUAL);

    }

    int compare_value = get_number_value();

    eat(TOKEN_LBRACE);

    long block_start = ftell(stdin);

    Token saved_token = current_token;

    while (1) {

        int left = atoi(runtime_get_variable(condition_var));

        int result = 0;

        if (condition_type == TOKEN_LESS) {

            result = left < compare_value;

        }

        else if (condition_type == TOKEN_GREATER) {

            result = left > compare_value;

        }

        else if (condition_type == TOKEN_EQUAL_EQUAL) {

            result = left == compare_value;

        }

        if (!result)
            break;

        current_token = saved_token;

        while (current_token.type != TOKEN_RBRACE) {

            parse_statement();

        }

    }

    eat(TOKEN_RBRACE);

}

static void parse_statement() {

    if (current_token.type == TOKEN_STAMP) {

        parse_stamp();

    }

    else if (current_token.type == TOKEN_IDENTIFIER) {

        parse_variable();

    }

    else if (current_token.type == TOKEN_IF) {

        parse_if();

    }

    else if (current_token.type == TOKEN_FLOW) {

        parse_flow();

    }

    else {

        printf("Unknown statement\n");

        exit(1);

    }

}

void parse_program() {

    current_token = get_next_token();

    eat(TOKEN_RIVEN);

    eat(TOKEN_CORE);

    parse_block();

}
