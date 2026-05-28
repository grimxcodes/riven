#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"

static const char* src;
static int pos = 0;

void init_lexer(const char* source) {

    src = source;
    pos = 0;

}

static char current() {

    return src[pos];

}

static void advance() {

    pos++;

}

static void skip_whitespace() {

    while (isspace(current())) {

        advance();

    }

}

static Token make_token(TokenType type, const char* value) {

    Token token;

    token.type = type;
    token.value = strdup(value);

    return token;

}

static Token identifier() {

    char buffer[256];

    int i = 0;

    while (isalnum(current()) || current() == '_') {

        buffer[i++] = current();

        advance();

    }

    buffer[i] = '\0';

    if (strcmp(buffer, "riven") == 0)
        return make_token(TOKEN_RIVEN, buffer);

    if (strcmp(buffer, "core") == 0)
        return make_token(TOKEN_CORE, buffer);

    if (strcmp(buffer, "stamp") == 0)
        return make_token(TOKEN_STAMP, buffer);

    if (strcmp(buffer, "if") == 0)
        return make_token(TOKEN_IF, buffer);

    if (strcmp(buffer, "flow") == 0)
        return make_token(TOKEN_FLOW, buffer);

    if (strcmp(buffer, "craft") == 0)
        return make_token(TOKEN_CRAFT, buffer);

    return make_token(TOKEN_IDENTIFIER, buffer);

}

static Token string() {

    char buffer[1024];

    int i = 0;

    advance();

    while (current() != '"' && current() != '\0') {

        buffer[i++] = current();

        advance();

    }

    buffer[i] = '\0';

    advance();

    return make_token(TOKEN_STRING, buffer);

}

static Token number() {

    char buffer[256];

    int i = 0;

    while (isdigit(current())) {

        buffer[i++] = current();

        advance();

    }

    buffer[i] = '\0';

    return make_token(TOKEN_NUMBER, buffer);

}

Token get_next_token() {

    skip_whitespace();

    if (current() == '\0') {

        return make_token(TOKEN_EOF, "EOF");

    }

    if (isalpha(current())) {

        return identifier();

    }

    if (isdigit(current())) {

        return number();

    }

    if (current() == '"') {

        return string();

    }

    if (current() == '=') {

        advance();

        return make_token(TOKEN_ASSIGN, "=");

    }

    if (current() == '{') {

        advance();

        return make_token(TOKEN_LBRACE, "{");

    }

    if (current() == '}') {

        advance();

        return make_token(TOKEN_RBRACE, "}");

    }

    if (current() == '(') {

        advance();

        return make_token(TOKEN_LPAREN, "(");

    }

    if (current() == ')') {

        advance();

        return make_token(TOKEN_RPAREN, ")");

    }

    advance();

    return make_token(TOKEN_EOF, "UNKNOWN");

}
