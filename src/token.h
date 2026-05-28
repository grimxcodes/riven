#ifndef TOKEN_H
#define TOKEN_H

typedef enum {

    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,

    TOKEN_RIVEN,
    TOKEN_CORE,

    TOKEN_STAMP,
    TOKEN_INPUT,
    TOKEN_IMPORT,

    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_FLOW,

    TOKEN_CRAFT,
    TOKEN_RETURN,

    TOKEN_PLUS,
    TOKEN_MINUS,

    TOKEN_ASSIGN,
    TOKEN_EQUAL_EQUAL,

    TOKEN_LESS,
    TOKEN_GREATER,

    TOKEN_LPAREN,
    TOKEN_RPAREN,

    TOKEN_LBRACE,
    TOKEN_RBRACE,

    TOKEN_LBRACKET,
    TOKEN_RBRACKET,

    TOKEN_COMMA,

    TOKEN_EOF

} TokenType;

typedef struct {

    TokenType type;

    char value[1000];

} Token;

#endif
