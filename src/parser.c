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

static void parse_variable() {

    char* name = strdup(current_token.value);

    eat(TOKEN_IDENTIFIER);

    eat(TOKEN_ASSIGN);

    char* value = strdup(current_token.value);

    eat(TOKEN_STRING);

    runtime_set_variable(name, value);

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

    eat(TOKEN_RPAREN);

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

        else {

            printf("Unknown statement\n");

            exit(1);

        }

    }

    eat(TOKEN_RBRACE);

}
