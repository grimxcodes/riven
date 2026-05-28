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

static void parse_if() {

    eat(TOKEN_IF);

    int condition = parse_condition();

    eat(TOKEN_LBRACE);

    if (condition) {

        while (current_token.type != TOKEN_RBRACE) {

            if (current_token.type == TOKEN_STAMP) {

                parse_stamp();

            }

            else if (current_token.type == TOKEN_IDENTIFIER) {

                parse_variable();

            }

        }

    }

    else {

        int brace_count = 1;

        while (brace_count > 0) {

            current_token = get_next_token();

            if (current_token.type == TOKEN_LBRACE)
                brace_count++;

            if (current_token.type == TOKEN_RBRACE)
                brace_count--;

        }

    }

    eat(TOKEN_RBRACE);

    if (current_token.type == TOKEN_ELSE) {

        eat(TOKEN_ELSE);

        eat(TOKEN_LBRACE);

        if (!condition) {

            while (current_token.type != TOKEN_RBRACE) {

                if (current_token.type == TOKEN_STAMP) {

                    parse_stamp();

                }

                else if (current_token.type == TOKEN_IDENTIFIER) {

                    parse_variable();

                }

            }

        }

        else {

            while (current_token.type != TOKEN_RBRACE) {

                current_token = get_next_token();

            }

        }

        eat(TOKEN_RBRACE);

    }

}

void parse_program() {

    current_token = get_next_token();

    eat(TOKEN_RIVEN);

    eat(TOKEN_CORE);

    eat(TOKEN_LBRACE);

    while (current_token.type != TOKEN_RBRACE) {

        if (current_token.type == TOKEN_STAMP) {

            parse_stamp();

        }

        else if (current_token.type == TOKEN_IDENTIFIER) {

            parse_variable();

        }

        else if (current_token.type == TOKEN_IF) {

            parse_if();

        }

        else {

            printf("Unknown statement\n");

            exit(1);

        }

    }

    eat(TOKEN_RBRACE);

}
