#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"
#include "token.h"

static const char* source;

static int position = 0;

void init_lexer(const char* text) {

    source = text;

    position = 0;

}

static char current_char() {

    return source[position];

}

static char peek_char() {

    return source[position + 1];

}

static void advance() {

    if (
        current_char() != '\0'
    ) {

        position++;

    }

}

static void skip_whitespace() {

    while (

        current_char() == ' '  ||
        current_char() == '\n' ||
        current_char() == '\t' ||
        current_char() == '\r'

    ) {

        advance();

    }

}

static void skip_single_comment() {

    advance();
    advance();

    while (

        current_char() != '\n' &&
        current_char() != '\0'

    ) {

        advance();

    }

}

static void skip_multi_comment() {

    advance();
    advance();

    while (

        !(

            current_char() == '>' &&
            peek_char() == '>'

        ) &&

        current_char() != '\0'

    ) {

        advance();

    }

    if (
        current_char() == '>'
    ) {

        advance();
        advance();

    }

}

static Token make_token(
    TokenType type,
    const char* value
) {

    Token token;

    token.type = type;

    strcpy(token.value, value);

    return token;

}

static Token string_token() {

    advance();

    char buffer[1000];

    int i = 0;

    while (

        current_char() != '"' &&
        current_char() != '\0'

    ) {

        buffer[i++] =
            current_char();

        advance();

    }

    buffer[i] = '\0';

    advance();

    return make_token(
        TOKEN_STRING,
        buffer
    );

}

static Token number_token() {

    char buffer[100];

    int i = 0;

    while (

        isdigit(current_char()) ||
        current_char() == '.'

    ) {

        buffer[i++] =
            current_char();

        advance();

    }

    buffer[i] = '\0';

    return make_token(
        TOKEN_NUMBER,
        buffer
    );

}

static Token identifier_token() {

    char buffer[100];

    int i = 0;

    while (

        isalnum(current_char()) ||
        current_char() == '_'

    ) {

        buffer[i++] =
            current_char();

        advance();

    }

    buffer[i] = '\0';

    if (strcmp(buffer, "riven") == 0)
        return make_token(TOKEN_RIVEN, buffer);

    if (strcmp(buffer, "core") == 0)
        return make_token(TOKEN_CORE, buffer);

    if (strcmp(buffer, "stamp") == 0)
        return make_token(TOKEN_STAMP, buffer);

    if (strcmp(buffer, "grab") == 0)
        return make_token(TOKEN_GRAB, buffer);

    if (strcmp(buffer, "consistof") == 0)
        return make_token(TOKEN_CONSISTOF, buffer);

    if (strcmp(buffer, "if") == 0)
        return make_token(TOKEN_IF, buffer);

    if (strcmp(buffer, "altif") == 0)
        return make_token(TOKEN_ALTIF, buffer);

    if (strcmp(buffer, "else") == 0)
        return make_token(TOKEN_ELSE, buffer);

    if (strcmp(buffer, "flow") == 0)
        return make_token(TOKEN_FLOW, buffer);

    if (strcmp(buffer, "during") == 0)
        return make_token(TOKEN_DURING, buffer);

    if (strcmp(buffer, "craft") == 0)
        return make_token(TOKEN_CRAFT, buffer);

    if (strcmp(buffer, "returns") == 0)
        return make_token(TOKEN_RETURNS, buffer);

    if (strcmp(buffer, "and") == 0)
        return make_token(TOKEN_AND, buffer);

    if (strcmp(buffer, "or") == 0)
        return make_token(TOKEN_OR, buffer);

    if (strcmp(buffer, "not") == 0)
        return make_token(TOKEN_NOT, buffer);

    return make_token(
        TOKEN_IDENTIFIER,
        buffer
    );

}

Token get_next_token() {

    while (1) {

        skip_whitespace();

        if (

            current_char() == '~' &&
            peek_char() == '~'

        ) {

            skip_single_comment();

            continue;

        }

        if (

            current_char() == '<' &&
            peek_char() == '<'

        ) {

            skip_multi_comment();

            continue;

        }

        break;

    }

    char c = current_char();

    if (c == '\0') {

        return make_token(
            TOKEN_EOF,
            "EOF"
        );

    }

    if (

        isdigit(c)

    ) {

        return number_token();

    }

    if (

        isalpha(c) ||
        c == '_'

    ) {

        return identifier_token();

    }

    if (c == '"') {

        return string_token();

    }

    if (

        c == '+' &&
        peek_char() == '>'

    ) {

        advance();
        advance();

        return make_token(
            TOKEN_INCREMENT,
            "+>"
        );

    }

    if (

        c == '-' &&
        peek_char() == '<'

    ) {

        advance();
        advance();

        return make_token(
            TOKEN_DECREMENT,
            "-<"
        );

    }

    if (

        c == '&' &&
        peek_char() == '&'

    ) {

        advance();
        advance();

        return make_token(
            TOKEN_AND_SYMBOL,
            "&&"
        );

    }

    if (

        c == '|' &&
        peek_char() == '|'

    ) {

        advance();
        advance();

        return make_token(
            TOKEN_OR_SYMBOL,
            "||"
        );

    }

    if (c == '!') {

        advance();

        return make_token(
            TOKEN_NOT_SYMBOL,
            "!"
        );

    }

    if (c == '=') {

        advance();

        if (
            current_char() == '='
        ) {

            advance();

            return make_token(
                TOKEN_EQUAL_EQUAL,
                "=="
            );

        }

        return make_token(
            TOKEN_ASSIGN,
            "="
        );

    }

    if (c == '+') {

        advance();

        return make_token(
            TOKEN_PLUS,
            "+"
        );

    }

    if (c == '-') {

        advance();

        return make_token(
            TOKEN_MINUS,
            "-"
        );

    }

    if (c == '<') {

        advance();

        return make_token(
            TOKEN_LESS,
            "<"
        );

    }

    if (c == '>') {

        advance();

        return make_token(
            TOKEN_GREATER,
            ">"
        );

    }

    if (c == '{') {

        advance();

        return make_token(
            TOKEN_LBRACE,
            "{"
        );

    }

    if (c == '}') {

        advance();

        return make_token(
            TOKEN_RBRACE,
            "}"
        );

    }

    if (c == '(') {

        advance();

        return make_token(
            TOKEN_LPAREN,
            "("
        );

    }

    if (c == ')') {

        advance();

        return make_token(
            TOKEN_RPAREN,
            ")"
        );

    }

    if (c == '[') {

        advance();

        return make_token(
            TOKEN_LBRACKET,
            "["
        );

    }

    if (c == ']') {

        advance();

        return make_token(
            TOKEN_RBRACKET,
            "]"
        );

    }

    if (c == ',') {

        advance();

        return make_token(
            TOKEN_COMMA,
            ","
        );

    }

    advance();

    return get_next_token();

}
