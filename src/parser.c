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

        int result = get_number_value();

        while (
            current_token.type == TOKEN_PLUS ||
            current_token.type == TOKEN_MINUS
        ) {

            if (current_token.type == TOKEN_PLUS) {

                eat(TOKEN_PLUS);

                result += get_number_value();

            }

            else if (current_token.type == TOKEN_MINUS) {

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

static void parse_if() {

    eat(TOKEN_IF);

    int condition = parse_condition();

    eat(TOKEN_LBRACE);

    if (condition) {

        while (current_token.type != TOKEN_RBRACE) {

            parse_statement();

        }

    }

    else {

        int braces = 1;

        while (braces > 0) {

            current_token = get_next_token();

            if (current_token.type == TOKEN_LBRACE)
                braces++;

            else if (current_token.type == TOKEN_RBRACE)
                braces--;

        }

    }

    eat(TOKEN_RBRACE);

    if (current_token.type == TOKEN_ELSE) {

        eat(TOKEN_ELSE);

        eat(TOKEN_LBRACE);

        if (!condition) {

            while (current_token.type != TOKEN_RBRACE) {

                parse_statement();

            }

        }

        else {

            int braces = 1;

            while (braces > 0) {

                current_token = get_next_token();

                if (current_token.type == TOKEN_LBRACE)
                    braces++;

                else if (current_token.type == TOKEN_RBRACE)
                    braces--;

            }

        }

        eat(TOKEN_RBRACE);

    }

}

static void parse_flow() {

    eat(TOKEN_FLOW);

    char* var_name = strdup(current_token.value);

    eat(TOKEN_IDENTIFIER);

    TokenType op = current_token.type;

    if (op == TOKEN_LESS)
        eat(TOKEN_LESS);

    else if (op == TOKEN_GREATER)
        eat(TOKEN_GREATER);

    else if (op == TOKEN_EQUAL_EQUAL)
        eat(TOKEN_EQUAL_EQUAL);

    int compare_value = get_number_value();

    eat(TOKEN_LBRACE);

    Token loop_tokens[1000];

    int loop_count = 0;

    while (current_token.type != TOKEN_RBRACE) {

        loop_tokens[loop_count++] = current_token;

        current_token = get_next_token();

    }

    eat(TOKEN_RBRACE);

    while (1) {

        int left = atoi(runtime_get_variable(var_name));

        int condition = 0;

        if (op == TOKEN_LESS)
            condition = left < compare_value;

        else if (op == TOKEN_GREATER)
            condition = left > compare_value;

        else if (op == TOKEN_EQUAL_EQUAL)
            condition = left == compare_value;

        if (!condition)
            break;

        int saved_index = 0;

        Token backup = current_token;

        current_token = loop_tokens[saved_index++];

        while (saved_index <= loop_count) {

            if (current_token.type == TOKEN_STAMP) {

                parse_stamp();

            }

            else if (current_token.type == TOKEN_IDENTIFIER) {

                parse_variable();

            }

            if (saved_index >= loop_count)
                break;

            current_token = loop_tokens[saved_index++];

        }

        current_token = backup;

    }

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
