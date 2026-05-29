#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"

static char* source = NULL;

static int position = 0;

void init_lexer(char* text) {

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

static void skip_spaces() {

    while (

        current_char() == ' ' ||
        current_char() == '\n' ||
        current_char() == '\t' ||
        current_char() == '\r'

    ) {

        advance();

    }

}

static void skip_comment() {

    if (

        current_char() == '~' &&
        peek_char() == '~'

    ) {

        while (

            current_char() != '\n' &&
            current_char() != '\0'

        ) {

            advance();

        }

    }

    else if (

        current_char() == '<' &&
        peek_char() == '<'

    ) {

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
            current_char() != '\0'
        ) {

            advance();
            advance();

        }

    }

}

static Token make_token(
    TokenType type,
    const char* value
) {

    Token token;

    token.type = type;

    strcpy(
        token.value,
        value
    );

    return token;

}

static Token number_token() {

    char buffer[1000];

    int i = 0;

    while (
        isdigit(current_char())
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

static Token string_token() {

    char buffer[1000];

    int i = 0;

    advance();

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

static Token identifier_token() {

    char buffer[1000];

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

    if (
        strcmp(buffer, "riven") == 0
    ) {

        return make_token(
            TOKEN_RIVEN,
            buffer
        );

    }

    if (
        strcmp(buffer, "core") == 0
    ) {

        return make_token(
            TOKEN_CORE,
            buffer
        );

    }

    if (
        strcmp(buffer, "stamp") == 0
    ) {

        return make_token(
            TOKEN_STAMP,
            buffer
        );

    }

    if (
        strcmp(buffer, "grab") == 0
    ) {

        return make_token(
            TOKEN_GRAB,
            buffer
        );

    }

    if (
        strcmp(buffer, "consistof") == 0
    ) {

        return make_token(
            TOKEN_CONSISTOF,
            buffer
        );

    }

    if (
        strcmp(buffer, "if") == 0
    ) {

        return make_token(
            TOKEN_IF,
            buffer
        );

    }

    if (
        strcmp(buffer, "altif") == 0
    ) {

        return make_token(
            TOKEN_ALTIF,
            buffer
        );

    }

    if (
        strcmp(buffer, "else") == 0
    ) {

        return make_token(
            TOKEN_ELSE,
            buffer
        );

    }

    if (
        strcmp(buffer, "flow") == 0
    ) {

        return make_token(
            TOKEN_FLOW,
            buffer
        );

    }

    if (
        strcmp(buffer, "during") == 0
    ) {

        return make_token(
            TOKEN_DURING,
            buffer
        );

    }

    if (
        strcmp(buffer, "craft") == 0
    ) {

        return make_token(
            TOKEN_CRAFT,
            buffer
        );

    }

    if (
        strcmp(buffer, "returns") == 0
    ) {

        return make_token(
            TOKEN_RETURNS,
            buffer
        );

    }

    if (
        strcmp(buffer, "frame") == 0
    ) {

        return make_token(
            TOKEN_FRAME,
            buffer
        );

    }

    if (
        strcmp(buffer, "boot") == 0
    ) {

        return make_token(
            TOKEN_BOOT,
            buffer
        );

    }

    if (
        strcmp(buffer, "open") == 0
    ) {

        return make_token(
            TOKEN_OPEN,
            buffer
        );

    }

    if (
        strcmp(buffer, "hidden") == 0
    ) {

        return make_token(
            TOKEN_HIDDEN,
            buffer
        );

    }

    if (
        strcmp(buffer, "spawn") == 0
    ) {

        return make_token(
            TOKEN_SPAWN,
            buffer
        );

    }

    if (
        strcmp(buffer, "rec") == 0
    ) {

        return make_token(
            TOKEN_REC,
            buffer
        );

    }

    if (
        strcmp(buffer, "rise") == 0
    ) {

        return make_token(
            TOKEN_RISE,
            buffer
        );

    }

    if (
        strcmp(buffer, "drop") == 0
    ) {

        return make_token(
            TOKEN_DROP,
            buffer
        );

    }

    if (
        strcmp(buffer, "firm") == 0
    ) {

        return make_token(
            TOKEN_FIRM,
            buffer
        );

    }

    if (
        strcmp(buffer, "correct") == 0
    ) {

        return make_token(
            TOKEN_CORRECT,
            buffer
        );

    }

    if (
        strcmp(buffer, "incorrect") == 0
    ) {

        return make_token(
            TOKEN_INCORRECT,
            buffer
        );

    }

    if (
        strcmp(buffer, "emp") == 0
    ) {

        return make_token(
            TOKEN_EMP,
            buffer
        );

    }

    if (
        strcmp(buffer, "and") == 0
    ) {

        return make_token(
            TOKEN_AND,
            buffer
        );

    }

    if (
        strcmp(buffer, "or") == 0
    ) {

        return make_token(
            TOKEN_OR,
            buffer
        );

    }

    if (
        strcmp(buffer, "not") == 0
    ) {

        return make_token(
            TOKEN_NOT,
            buffer
        );

    }

    return make_token(
        TOKEN_IDENTIFIER,
        buffer
    );

}

Token get_next_token() {

    while (
        current_char() != '\0'
    ) {

        skip_spaces();

        if (

            (current_char() == '~' &&
            peek_char() == '~')

            ||

            (current_char() == '<' &&
            peek_char() == '<')

        ) {

            skip_comment();

            continue;

        }

        if (
            isdigit(current_char())
        ) {

            return number_token();

        }

        if (
            current_char() == '"'
        ) {

            return string_token();

        }

        if (
            isalpha(current_char()) ||
            current_char() == '_'
        ) {

            return identifier_token();

        }

        if (
            current_char() == '+' &&
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
            current_char() == '-' &&
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
            current_char() == '&' &&
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
            current_char() == '|' &&
            peek_char() == '|'
        ) {

            advance();
            advance();

            return make_token(
                TOKEN_OR_SYMBOL,
                "||"
            );

        }

        if (
            current_char() == '=' &&
            peek_char() == '='
        ) {

            advance();
            advance();

            return make_token(
                TOKEN_EQUAL_EQUAL,
                "=="
            );

        }

        switch (
            current_char()
        ) {

            case '+':

                advance();

                return make_token(
                    TOKEN_PLUS,
                    "+"
                );

            case '-':

                advance();

                return make_token(
                    TOKEN_MINUS,
                    "-"
                );

            case '=':

                advance();

                return make_token(
                    TOKEN_ASSIGN,
                    "="
                );

            case '<':

                advance();

                return make_token(
                    TOKEN_LESS,
                    "<"
                );

            case '>':

                advance();

                return make_token(
                    TOKEN_GREATER,
                    ">"
                );

            case '.':

                advance();

                return make_token(
                    TOKEN_DOT,
                    "."
                );

            case '(':

                advance();

                return make_token(
                    TOKEN_LPAREN,
                    "("
                );

            case ')':

                advance();

                return make_token(
                    TOKEN_RPAREN,
                    ")"
                );

            case '{':

                advance();

                return make_token(
                    TOKEN_LBRACE,
                    "{"
                );

            case '}':

                advance();

                return make_token(
                    TOKEN_RBRACE,
                    "}"
                );

            case '[':

                advance();

                return make_token(
                    TOKEN_LBRACKET,
                    "["
                );

            case ']':

                advance();

                return make_token(
                    TOKEN_RBRACKET,
                    "]"
                );

            case ',':

                advance();

                return make_token(
                    TOKEN_COMMA,
                    ","
                );

        }

        printf(
            "Unknown character: %c\n",
            current_char()
        );

        exit(1);

    }

    return make_token(
        TOKEN_EOF,
        "EOF"
    );

}
