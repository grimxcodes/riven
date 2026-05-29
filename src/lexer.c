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
        strcmp(buffer, "firm") == 0
    ) {

        return make_token(
            TOKEN_FIRM,
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
        strcmp(buffer, "consistof") == 0
    ) {

        return make_token(
            TOKEN_CONSISTOF,
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

    if (
        strcmp(buffer, "rec") == 0
    ) {

        return make_token(
            TOKEN_REC,
            buffer
        );

    }

    if (
        strcmp(buffer, "ptr") == 0
    ) {

        return make_token(
            TOKEN_PTR,
            buffer
        );

    }

    if (
        strcmp(buffer, "ref") == 0
    ) {

        return make_token(
            TOKEN_REF,
            buffer
        );

    }

    if (
        strcmp(buffer, "raw") == 0
    ) {

        return make_token(
            TOKEN_RAW,
            buffer
        );

    }

    if (
        strcmp(buffer, "fetch") == 0
    ) {

        return make_token(
            TOKEN_FETCH,
            buffer
        );

    }

    if (
        strcmp(buffer, "spark") == 0
    ) {

        return make_token(
            TOKEN_SPARK,
            buffer
        );

    }

    if (
        strcmp(buffer, "sync") == 0
    ) {

        return make_token(
            TOKEN_SYNC,
            buffer
        );

    }

    if (
        strcmp(buffer, "file") == 0
    ) {

        return make_token(
            TOKEN_FILE,
            buffer
        );

    }

    if (
        strcmp(buffer, "int") == 0
    ) {

        return make_token(
            TOKEN_INT,
            buffer
        );

    }

    if (
        strcmp(buffer, "txt") == 0
    ) {

        return make_token(
            TOKEN_TXT,
            buffer
        );

    }

    if (
        strcmp(buffer, "dnum") == 0
    ) {

        return make_token(
            TOKEN_DNUM,
            buffer
        );

    }

    return make_token(
        TOKEN_IDENTIFIER,
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

Token get_next_token() {

    while (
        current_char() != '\0'
    ) {

        skip_spaces();

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

            source[index_pos + 1] == '~'

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

            source[index_pos + 1] == '<'

        ) {

            advance();
            advance();

            while (

                !(

                    current_char() == '>' &&

                    source[index_pos + 1] == '>'

                ) &&

                current_char() != '\0'

            ) {

                advance();

            }

            advance();
            advance();

            continue;

        }

        if (
            current_char() == '+'
        ) {

            if (
                source[index_pos + 1] == '>'
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
                source[index_pos + 1] == '<'
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
                source[index_pos + 1] == '='
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
                source[index_pos + 1] == '='
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
                source[index_pos + 1] == '&'
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
                source[index_pos + 1] == '|'
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
