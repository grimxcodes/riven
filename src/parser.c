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

    } else {

        printf("Unexpected token: %s\n", current_token.value);
        exit(1);
    }
}

static void parse_stamp() {

    eat(TOKEN_STAMP);
    eat(TOKEN_LPAREN);

    char* text = strdup(current_token.value);

    eat(TOKEN_STRING);

    eat(TOKEN_RPAREN);

    runtime_stamp(text);
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

        else {

            printf("Unknown statement\n");
            exit(1);
        }
    }

    eat(TOKEN_RBRACE);
}
