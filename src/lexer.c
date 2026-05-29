#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "lexer.h"

static char* source;
static int index_pos = 0;

void init_lexer(char* text) {

    source = text;

    index_pos = 0;

}

static char current_char() {

    return source[index_pos];

}

static char peek_char() {

    return source[index_pos + 1];

}

static void advance() {

    if (
        current_char() != '\0'
    ) {

        index_pos++;

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

static Token read_number() {

    char buffer[256];

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

static Token read_string() {

    char buffer[1024];

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

static Token read_identifier() {

    char buffer[256];

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

    #define KEYWORD(word, token) \
        if (strcmp(buffer, word) == 0) \
            return make_token(token, buffer);

    KEYWORD("if", TOKEN_IF)
    KEYWORD("altif", TOKEN_ALTIF)
    KEYWORD("else", TOKEN_ELSE)

    KEYWORD("flow", TOKEN_FLOW)
    KEYWORD("during", TOKEN_DURING)

    KEYWORD("craft", TOKEN_CRAFT)
    KEYWORD("returns", TOKEN_RETURNS)

    KEYWORD("riven", TOKEN_RIVEN)
    KEYWORD("core", TOKEN_CORE)

    KEYWORD("stamp", TOKEN_STAMP)
    KEYWORD("grab", TOKEN_GRAB)

    KEYWORD("correct", TOKEN_CORRECT)
    KEYWORD("incorrect", TOKEN_INCORRECT)

    KEYWORD("emp", TOKEN_EMP)

    KEYWORD("firm", TOKEN_FIRM)

    KEYWORD("rise", TOKEN_RISE)
    KEYWORD("drop", TOKEN_DROP)

    KEYWORD("frame", TOKEN_FRAME)
    KEYWORD("boot", TOKEN_BOOT)

    KEYWORD("open", TOKEN_OPEN)
    KEYWORD("hidden", TOKEN_HIDDEN)

    KEYWORD("spawn", TOKEN_SPAWN)

    KEYWORD("consistof", TOKEN_CONSISTOF)

    KEYWORD("and", TOKEN_AND)
    KEYWORD("or", TOKEN_OR)
    KEYWORD("not", TOKEN_NOT)

    KEYWORD("rec", TOKEN_REC)

    KEYWORD("ptr", TOKEN_PTR)
    KEYWORD("ref", TOKEN_REF)

    KEYWORD("raw", TOKEN_RAW)

    KEYWORD("fetch", TOKEN_FETCH)

    KEYWORD("spark", TOKEN_SPARK)
    KEYWORD("sync", TOKEN_SYNC)

    KEYWORD("file", TOKEN_FILE)

    KEYWORD("int", TOKEN_INT)
    KEYWORD("txt", TOKEN_TXT)
    KEYWORD("dnum", TOKEN_DNUM)

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
            current_char() == '\0'
        ) {

            break;

        }

        if (
            isdigit(current_char())
        ) {

            return read_number();

        }

        if (

            isalpha(current_char()) ||

            current_char() == '_'

        ) {

            return read_identifier();

        }

        if (
            current_char() == '"'
        ) {

            return read_string();

        }

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

            continue;

        }

        if (

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

            continue;

        }

        if (
            current_char() == '+'
        ) {

            if (
                peek_char() == '>'
            ) {

                advance();
                advance();

                return make_token(
                    TOKEN_INCREMENT,
                    "+>"
                );

            }

            advance();

            return make_token(
                TOKEN_PLUS,
                "+"
            );

        }

        if (
            current_char() == '-'
        ) {

            if (
                peek_char() == '<'
            ) {

                advance();
                advance();

                return make_token(
                    TOKEN_DECREMENT,
                    "-<"
                );

            }

            advance();

            return make_token(
                TOKEN_MINUS,
                "-"
            );

        }

        if (
            current_char() == '*'
        ) {

            advance();

            return make_token(
                TOKEN_MULTIPLY,
                "*"
            );

        }

        if (
            current_char() == '/'
        ) {

            advance();

            return make_token(
                TOKEN_DIVIDE,
                "/"
            );

        }

        if (
            current_char() == '='
        ) {

            if (
                peek_char() == '='
            ) {

                advance();
                advance();

                return make_token(
                    TOKEN_EQUAL_EQUAL,
                    "=="
                );

            }

            advance();

            return make_token(
                TOKEN_ASSIGN,
                "="
            );

        }

        if (
            current_char() == '!'
        ) {

            if (
                peek_char() == '='
            ) {

                advance();
                advance();

                return make_token(
                    TOKEN_NOT_EQUAL,
                    "!="
                );

            }

        }

        if (
            current_char() == '<'
        ) {

            advance();

            return make_token(
                TOKEN_LESS,
                "<"
            );

        }

        if (
            current_char() == '>'
        ) {

            advance();

            return make_token(
                TOKEN_GREATER,
                ">"
            );

        }

        if (
            current_char() == '&'
        ) {

            if (
                peek_char() == '&'
            ) {

                advance();
                advance();

                return make_token(
                    TOKEN_AND_SYMBOL,
                    "&&"
                );

            }

        }

        if (
            current_char() == '|'
        ) {

            if (
                peek_char() == '|'
            ) {

                advance();
                advance();

                return make_token(
                    TOKEN_OR_SYMBOL,
                    "||"
                );

            }

        }

        if (
            current_char() == '('
        ) {

            advance();

            return make_token(
                TOKEN_LPAREN,
                "("
            );

        }

        if (
            current_char() == ')'
        ) {

            advance();

            return make_token(
                TOKEN_RPAREN,
                ")"
            );

        }

        if (
            current_char() == '{'
        ) {

            advance();

            return make_token(
                TOKEN_LBRACE,
                "{"
            );

        }

        if (
            current_char() == '}'
        ) {

            advance();

            return make_token(
                TOKEN_RBRACE,
                "}"
            );

        }

        if (
            current_char() == '['
        ) {

            advance();

            return make_token(
                TOKEN_LBRACKET,
                "["
            );

        }

        if (
            current_char() == ']'
        ) {

            advance();

            return make_token(
                TOKEN_RBRACKET,
                "]"
            );

        }

        if (
            current_char() == ','
        ) {

            advance();

            return make_token(
                TOKEN_COMMA,
                ","
            );

        }

        if (
            current_char() == '.'
        ) {

            advance();

            return make_token(
                TOKEN_DOT,
                "."
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
