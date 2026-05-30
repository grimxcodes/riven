// riven.h
#ifndef RIVEN_H
#define RIVEN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <setjmp.h>

// ---------- Lexer ----------
typedef enum {
    TOK_IDENT, TOK_NUMBER, TOK_STRING, TOK_KEYWORD,
    TOK_LBRACE, TOK_RBRACE, TOK_LPAREN, TOK_RPAREN,
    TOK_ASSIGN, TOK_SEMI, TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH,
    TOK_EQ, TOK_NE, TOK_LT, TOK_GT, TOK_AND, TOK_OR, TOK_NOT,
    TOK_COMMA, TOK_DOT, TOK_INC, TOK_DEC,
    TOK_EOF
} TokenType;

typedef struct {
    TokenType type;
    char* value;
    int line;
} Token;

// ---------- AST Nodes ----------
typedef enum { NODE_VAR, NODE_NUM, NODE_STR, NODE_BINOP, NODE_CALL, NODE_IF, NODE_WHILE, NODE_BLOCK } NodeType;

typedef struct ASTNode {
    NodeType type;
    union {
        struct { char* name; } var;
        struct { double num; } num;
        struct { char* str; } str;
        struct { struct ASTNode* left; char op; struct ASTNode* right; } binop;
        struct { char* func; struct ASTNode** args; int argc; } call;
        struct { struct ASTNode* cond; struct ASTNode* then; struct ASTNode* els; } ifstmt;
        struct { struct ASTNode* cond; struct ASTNode* body; } whilestmt;
        struct { struct ASTNode** stmts; int count; } block;
    };
} ASTNode;

// ---------- Interpreter ----------
typedef struct {
    char* name;
    double num_val;
    char* str_val;
    int type; // 0=num,1=str,2=undef
} Value;

typedef struct Variable {
    char* name;
    Value val;
    struct Variable* next;
} Variable;

Variable* global_vars;
jmp_buf error_jmp;

// Function prototypes
void init_lexer(const char* src);
Token next_token();
ASTNode* parse_program();
Value eval(ASTNode* node);
void free_ast(ASTNode* node);

#endif
