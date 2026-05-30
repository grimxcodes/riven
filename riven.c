/*
 * RIVEN LANGUAGE INTERPRETER v1.0 (C Implementation)
 * Fully implements spec: OOP, async, loops, conditions, builtins, etc.
 * Compile: clang -o riven riven.c -lm -lpthread
 * Run: ./riven main.rv
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <pthread.h>
#include <setjmp.h>
#include <stdarg.h>

// ------------------------------ LEXER ------------------------------------
typedef enum {
    TOK_IDENT, TOK_NUMBER, TOK_STRING, TOK_KEYWORD,
    TOK_LBRACE, TOK_RBRACE, TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACK, TOK_RBRACK, TOK_ASSIGN, TOK_PLUS, TOK_MINUS,
    TOK_STAR, TOK_SLASH, TOK_EQ, TOK_NE, TOK_LT, TOK_GT,
    TOK_AND, TOK_OR, TOK_NOT, TOK_INC, TOK_DEC, TOK_DOT,
    TOK_COMMA, TOK_SEMI, TOK_EOF
} TokenType;

typedef struct {
    TokenType type;
    char *value;
    int line;
} Token;

static char *source;
static int pos;
static int line_no = 1;

void init_lexer(char *src) { source = src; pos = 0; line_no = 1; }

static char peek() { return source[pos]; }
static char advance() { char c = source[pos++]; if (c == '\n') line_no++; return c; }

static int is_keyword(char *s) {
    const char *keywords[] = {"craft","returns","if","altif","else","during","flow",
                              "frame","boot","spawn","spark","sync","resc","attack",
                              "bind","ref","ptr","raw","open","hidden","consistof",
                              "firm","and","or","not","rise","drop","correct","incorrect","emp",
                              "riven","core","stamp","grab","int","txt","dnum","coll","rec","fetch",
                              NULL};
    for (int i=0; keywords[i]; i++) if (strcmp(s, keywords[i])==0) return 1;
    return 0;
}

Token next_token() {
    while (isspace(peek())) advance();
    if (peek() == '\0') return (Token){TOK_EOF, "", line_no};
    if (isalpha(peek()) || peek() == '_') {
        int start = pos;
        while (isalnum(peek()) || peek() == '_') advance();
        int len = pos-start;
        char *buf = malloc(len+1);
        strncpy(buf, source+start, len);
        buf[len] = '\0';
        TokenType t = is_keyword(buf) ? TOK_KEYWORD : TOK_IDENT;
        return (Token){t, buf, line_no};
    }
    if (isdigit(peek())) {
        int start = pos;
        while (isdigit(peek()) || peek() == '.') advance();
        int len = pos-start;
        char *buf = malloc(len+1);
        strncpy(buf, source+start, len);
        buf[len] = '\0';
        return (Token){TOK_NUMBER, buf, line_no};
    }
    if (peek() == '"') {
        advance();
        int start = pos;
        while (peek() != '"' && peek() != '\0') advance();
        int len = pos-start;
        char *buf = malloc(len+1);
        strncpy(buf, source+start, len);
        buf[len] = '\0';
        advance(); // closing "
        return (Token){TOK_STRING, buf, line_no};
    }
    char c = advance();
    if (c == '{') return (Token){TOK_LBRACE, "{", line_no};
    if (c == '}') return (Token){TOK_RBRACE, "}", line_no};
    if (c == '(') return (Token){TOK_LPAREN, "(", line_no};
    if (c == ')') return (Token){TOK_RPAREN, ")", line_no};
    if (c == '[') return (Token){TOK_LBRACK, "[", line_no};
    if (c == ']') return (Token){TOK_RBRACK, "]", line_no};
    if (c == '=') {
        if (peek() == '=') { advance(); return (Token){TOK_EQ, "==", line_no}; }
        return (Token){TOK_ASSIGN, "=", line_no};
    }
    if (c == '+') {
        if (peek() == '>') { advance(); return (Token){TOK_INC, "+>", line_no}; }
        if (peek() == '=') { advance(); return (Token){TOK_ASSIGN, "+=", line_no}; }
        return (Token){TOK_PLUS, "+", line_no};
    }
    if (c == '-') {
        if (peek() == '<') { advance(); return (Token){TOK_DEC, "-<", line_no}; }
        if (peek() == '=') { advance(); return (Token){TOK_ASSIGN, "-=", line_no}; }
        return (Token){TOK_MINUS, "-", line_no};
    }
    if (c == '*') return (Token){TOK_STAR, "*", line_no};
    if (c == '/') return (Token){TOK_SLASH, "/", line_no};
    if (c == '<') return (Token){TOK_LT, "<", line_no};
    if (c == '>') return (Token){TOK_GT, ">", line_no};
    if (c == '!') {
        if (peek() == '=') { advance(); return (Token){TOK_NE, "!=", line_no}; }
        return (Token){TOK_NOT, "!", line_no};
    }
    if (c == '&') { if (peek() == '&') { advance(); return (Token){TOK_AND, "&&", line_no}; } }
    if (c == '|') { if (peek() == '|') { advance(); return (Token){TOK_OR, "||", line_no}; } }
    if (c == '.') return (Token){TOK_DOT, ".", line_no};
    if (c == ',') return (Token){TOK_COMMA, ",", line_no};
    if (c == ';') return (Token){TOK_SEMI, ";", line_no};
    return (Token){TOK_EOF, "", line_no};
}

// ------------------------------ AST NODES ---------------------------------
typedef enum {
    NODE_PROGRAM, NODE_CORE, NODE_VAR_DECL, NODE_CONST_DECL,
    NODE_FUNC_DECL, NODE_CLASS_DECL, NODE_IF, NODE_WHILE, NODE_FLOW,
    NODE_RETURN, NODE_EXPR_STMT, NODE_BINARY, NODE_UNARY,
    NODE_CALL, NODE_METHOD_CALL, NODE_VAR, NODE_PROPERTY,
    NODE_NUMBER, NODE_STRING, NODE_BOOL, NODE_NULL, NODE_LIST, NODE_RECORD,
    NODE_INC_DEC, NODE_INCLUDE, NODE_RAW, NODE_RESC, NODE_ATTACK,
    NODE_SPARK, NODE_SYNC, NODE_BIND, NODE_SPAWN, NODE_FETCH,
    NODE_INDEX, NODE_ASSIGN, NODE_PROP_ASSIGN
} NodeType;

typedef struct ASTNode {
    NodeType type;
    struct ASTNode *next; // for block lists
    union {
        struct { struct ASTNode **stmts; int count; } block;
        struct { char *name; struct ASTNode *value; int constant; } var_decl;
        struct { char *name; char **params; int paramc; struct ASTNode *body; } func_decl;
        struct { char *name; struct ASTNode **methods; int methodc; struct ASTNode *constructor; } class_decl;
        struct { struct ASTNode *cond; struct ASTNode *then; struct ASTNode **elifs; int elifc; struct ASTNode *els; } if_stmt;
        struct { struct ASTNode *cond; struct ASTNode *body; } while_stmt;
        struct { struct ASTNode *count; struct ASTNode *body; } flow_stmt;
        struct { struct ASTNode *value; } return_stmt;
        struct { struct ASTNode *expr; } expr_stmt;
        struct { struct ASTNode *left; char *op; struct ASTNode *right; } binary;
        struct { char *op; struct ASTNode *expr; } unary;
        struct { char *name; struct ASTNode **args; int argc; } call;
        struct { struct ASTNode *obj; char *method; struct ASTNode **args; int argc; } method_call;
        struct { char *name; } var;
        struct { struct ASTNode *obj; char *prop; } property;
        struct { double num; } number;
        struct { char *str; } string;
        struct { int val; } boolean;
        struct { struct ASTNode **elems; int count; } list;
        struct { char **keys; struct ASTNode **vals; int count; } record;
        struct { struct ASTNode *var; char *op; } inc_dec; // op: "+>" "-<" "rise" "drop"
        struct { char *path; } include;
        struct { struct ASTNode *body; } raw;
        struct { struct ASTNode *body; } resc;
        struct { struct ASTNode *msg; } attack;
        struct { struct ASTNode *call; } spark;
        struct { struct ASTNode *ref; } bind;
        struct { char *class_name; } spawn;
        struct { struct ASTNode *url; } fetch;
        struct { struct ASTNode *coll; struct ASTNode *idx; } index;
        struct { struct ASTNode *left; struct ASTNode *right; } assign;
        struct { struct ASTNode *obj; char *prop; struct ASTNode *value; } prop_assign;
    };
} ASTNode;

// ------------------------------ PARSER ------------------------------------
typedef struct {
    Token current;
    jmp_buf err;
} ParserState;

static ParserState ps;

void advance_token() { ps.current = next_token(); }

void parse_error(char *msg) {
    fprintf(stderr, "Parse error at line %d: %s\n", ps.current.line, msg);
    longjmp(ps.err, 1);
}

void expect(TokenType t, char *msg) {
    if (ps.current.type != t) parse_error(msg);
    advance_token();
}

ASTNode *parse_expression();
ASTNode *parse_statement();
ASTNode *parse_block();

ASTNode *new_node(NodeType type) {
    ASTNode *n = calloc(1, sizeof(ASTNode));
    n->type = type;
    return n;
}

ASTNode *parse_primary() {
    if (ps.current.type == TOK_NUMBER) {
        ASTNode *n = new_node(NODE_NUMBER);
        n->number.num = atof(ps.current.value);
        advance_token();
        return n;
    }
    if (ps.current.type == TOK_STRING) {
        ASTNode *n = new_node(NODE_STRING);
        n->string.str = strdup(ps.current.value);
        advance_token();
        return n;
    }
    if (ps.current.type == TOK_KEYWORD) {
        if (strcmp(ps.current.value, "correct")==0) {
            advance_token();
            ASTNode *n = new_node(NODE_BOOL);
            n->boolean.val = 1;
            return n;
        }
        if (strcmp(ps.current.value, "incorrect")==0) {
            advance_token();
            ASTNode *n = new_node(NODE_BOOL);
            n->boolean.val = 0;
            return n;
        }
        if (strcmp(ps.current.value, "emp")==0) {
            advance_token();
            return new_node(NODE_NULL);
        }
    }
    if (ps.current.type == TOK_IDENT) {
        char *name = strdup(ps.current.value);
        advance_token();
        // function call?
        if (ps.current.type == TOK_LPAREN) {
            advance_token();
            ASTNode **args = NULL;
            int argc = 0;
            if (ps.current.type != TOK_RPAREN) {
                do {
                    args = realloc(args, sizeof(ASTNode*)*(argc+1));
                    args[argc++] = parse_expression();
                } while (ps.current.type == TOK_COMMA && advance_token(), ps.current.type != TOK_RPAREN);
            }
            expect(TOK_RPAREN, "Expected ')'");
            ASTNode *n = new_node(NODE_CALL);
            n->call.name = name;
            n->call.args = args;
            n->call.argc = argc;
            return n;
        }
        // property access chain
        ASTNode *obj = new_node(NODE_VAR);
        obj->var.name = name;
        while (ps.current.type == TOK_DOT) {
            advance_token();
            if (ps.current.type != TOK_IDENT) parse_error("Expected property name");
            char *prop = strdup(ps.current.value);
            advance_token();
            if (ps.current.type == TOK_LPAREN) { // method call
                advance_token();
                ASTNode **args = NULL;
                int argc = 0;
                if (ps.current.type != TOK_RPAREN) {
                    do {
                        args = realloc(args, sizeof(ASTNode*)*(argc+1));
                        args[argc++] = parse_expression();
                    } while (ps.current.type == TOK_COMMA && advance_token(), ps.current.type != TOK_RPAREN);
                }
                expect(TOK_RPAREN, "Expected ')'");
                ASTNode *m = new_node(NODE_METHOD_CALL);
                m->method_call.obj = obj;
                m->method_call.method = prop;
                m->method_call.args = args;
                m->method_call.argc = argc;
                obj = m;
            } else {
                ASTNode *p = new_node(NODE_PROPERTY);
                p->property.obj = obj;
                p->property.prop = prop;
                obj = p;
            }
        }
        return obj;
    }
    if (ps.current.type == TOK_LPAREN) {
        advance_token();
        ASTNode *e = parse_expression();
        expect(TOK_RPAREN, "Expected ')'");
        return e;
    }
    if (ps.current.type == TOK_LBRACK) {
        advance_token();
        ASTNode **elems = NULL;
        int cnt = 0;
        if (ps.current.type != TOK_RBRACK) {
            do {
                elems = realloc(elems, sizeof(ASTNode*)*(cnt+1));
                elems[cnt++] = parse_expression();
            } while (ps.current.type == TOK_COMMA && advance_token(), ps.current.type != TOK_RBRACK);
        }
        expect(TOK_RBRACK, "Expected ']'");
        ASTNode *n = new_node(NODE_LIST);
        n->list.elems = elems;
        n->list.count = cnt;
        return n;
    }
    if (ps.current.type == TOK_LBRACE) {
        advance_token();
        char **keys = NULL;
        ASTNode **vals = NULL;
        int cnt = 0;
        if (ps.current.type != TOK_RBRACE) {
            do {
                if (ps.current.type != TOK_IDENT) parse_error("Expected field name");
                char *key = strdup(ps.current.value);
                advance_token();
                expect(TOK_ASSIGN, "Expected '='");
                ASTNode *val = parse_expression();
                keys = realloc(keys, sizeof(char*)*(cnt+1));
                vals = realloc(vals, sizeof(ASTNode*)*(cnt+1));
                keys[cnt] = key;
                vals[cnt] = val;
                cnt++;
            } while (ps.current.type == TOK_COMMA && advance_token(), ps.current.type != TOK_RBRACE);
        }
        expect(TOK_RBRACE, "Expected '}'");
        ASTNode *n = new_node(NODE_RECORD);
        n->record.keys = keys;
        n->record.vals = vals;
        n->record.count = cnt;
        return n;
    }
    if (ps.current.type == TOK_KEYWORD && strcmp(ps.current.value, "spawn")==0) {
        advance_token();
        if (ps.current.type != TOK_IDENT) parse_error("Expected class name");
        char *cls = strdup(ps.current.value);
        advance_token();
        ASTNode *n = new_node(NODE_SPAWN);
        n->spawn.class_name = cls;
        return n;
    }
    if (ps.current.type == TOK_KEYWORD && strcmp(ps.current.value, "fetch")==0) {
        advance_token();
        expect(TOK_LPAREN, "Expected '('");
        ASTNode *url = parse_expression();
        expect(TOK_RPAREN, "Expected ')'");
        ASTNode *n = new_node(NODE_FETCH);
        n->fetch.url = url;
        return n;
    }
    parse_error("Unexpected token in primary");
    return NULL;
}

int get_precedence(TokenType t, char **op) {
    if (t == TOK_AND || (t == TOK_KEYWORD && strcmp(ps.current.value,"and")==0)) { *op = "&&"; return 3; }
    if (t == TOK_OR || (t == TOK_KEYWORD && strcmp(ps.current.value,"or")==0)) { *op = "||"; return 2; }
    if (t == TOK_EQ || t == TOK_NE) { *op = (t==TOK_EQ)?"==":"!="; return 4; }
    if (t == TOK_LT || t == TOK_GT) { *op = (t==TOK_LT)?"<":">"; return 5; }
    if (t == TOK_PLUS || t == TOK_MINUS) { *op = (t==TOK_PLUS)?"+":"-"; return 6; }
    if (t == TOK_STAR || t == TOK_SLASH) { *op = (t==TOK_STAR)?"*":"/"; return 7; }
    return 0;
}

ASTNode *parse_binary(int min_prec) {
    ASTNode *left = parse_primary();
    while (1) {
        char *op = NULL;
        int prec = get_precedence(ps.current.type, &op);
        if (prec == 0 || prec < min_prec) break;
        TokenType t = ps.current.type;
        advance_token();
        if (t == TOK_KEYWORD && (strcmp(op,"and")==0 || strcmp(op,"or")==0)) {
            // keyword handled
        }
        ASTNode *right = parse_binary(prec+1);
        ASTNode *node = new_node(NODE_BINARY);
        node->binary.left = left;
        node->binary.op = strdup(op);
        node->binary.right = right;
        left = node;
    }
    return left;
}

ASTNode *parse_expression() { return parse_binary(0); }

ASTNode *parse_statement() {
    if (ps.current.type == TOK_KEYWORD) {
        if (strcmp(ps.current.value, "riven")==0) {
            advance_token();
            if (ps.current.type != TOK_KEYWORD || strcmp(ps.current.value,"core")!=0) parse_error("Expected 'core'");
            advance_token();
            expect(TOK_LBRACE, "Expected '{'");
            ASTNode *core = new_node(NODE_CORE);
            core->block.stmts = NULL;
            core->block.count = 0;
            while (ps.current.type != TOK_RBRACE && ps.current.type != TOK_EOF) {
                core->block.stmts = realloc(core->block.stmts, sizeof(ASTNode*)*(core->block.count+1));
                core->block.stmts[core->block.count++] = parse_statement();
            }
            expect(TOK_RBRACE, "Expected '}'");
            return core;
        }
        if (strcmp(ps.current.value, "craft")==0) {
            advance_token();
            if (ps.current.type != TOK_IDENT) parse_error("Expected function name");
            char *name = strdup(ps.current.value);
            advance_token();
            expect(TOK_LPAREN, "Expected '('");
            char **params = NULL;
            int paramc = 0;
            if (ps.current.type != TOK_RPAREN) {
                do {
                    if (ps.current.type != TOK_IDENT) parse_error("Expected parameter name");
                    params = realloc(params, sizeof(char*)*(paramc+1));
                    params[paramc++] = strdup(ps.current.value);
                    advance_token();
                } while (ps.current.type == TOK_COMMA && advance_token(), ps.current.type != TOK_RPAREN);
            }
            expect(TOK_RPAREN, "Expected ')'");
            expect(TOK_LBRACE, "Expected '{'");
            ASTNode *body = parse_block();
            ASTNode *func = new_node(NODE_FUNC_DECL);
            func->func_decl.name = name;
            func->func_decl.params = params;
            func->func_decl.paramc = paramc;
            func->func_decl.body = body;
            return func;
        }
        if (strcmp(ps.current.value, "frame")==0) {
            advance_token();
            if (ps.current.type != TOK_IDENT) parse_error("Expected class name");
            char *name = strdup(ps.current.value);
            advance_token();
            expect(TOK_LBRACE, "Expected '{'");
            ASTNode **methods = NULL;
            int methodc = 0;
            ASTNode *constructor = NULL;
            while (ps.current.type != TOK_RBRACE && ps.current.type != TOK_EOF) {
                if (ps.current.type == TOK_KEYWORD) {
                    if (strcmp(ps.current.value, "boot")==0) {
                        advance_token();
                        expect(TOK_LPAREN, "Expected '('");
                        expect(TOK_RPAREN, "Expected ')'");
                        expect(TOK_LBRACE, "Expected '{'");
                        constructor = parse_block();
                        expect(TOK_RBRACE, "Expected '}'");
                        continue;
                    }
                    if (strcmp(ps.current.value, "open")==0 || strcmp(ps.current.value,"hidden")==0) {
                        advance_token(); // skip modifier
                    }
                }
                if (ps.current.type == TOK_IDENT || (ps.current.type == TOK_KEYWORD && strcmp(ps.current.value,"craft")==0)) {
                    // method definition
                    if (ps.current.type != TOK_KEYWORD || strcmp(ps.current.value,"craft")!=0) parse_error("Expected 'craft' for method");
                    advance_token();
                    if (ps.current.type != TOK_IDENT) parse_error("Expected method name");
                    char *mname = strdup(ps.current.value);
                    advance_token();
                    expect(TOK_LPAREN, "Expected '('");
                    char **mparams = NULL;
                    int mparamc = 0;
                    if (ps.current.type != TOK_RPAREN) {
                        do {
                            if (ps.current.type != TOK_IDENT) parse_error("Expected parameter name");
                            mparams = realloc(mparams, sizeof(char*)*(mparamc+1));
                            mparams[mparamc++] = strdup(ps.current.value);
                            advance_token();
                        } while (ps.current.type == TOK_COMMA && advance_token(), ps.current.type != TOK_RPAREN);
                    }
                    expect(TOK_RPAREN, "Expected ')'");
                    expect(TOK_LBRACE, "Expected '{'");
                    ASTNode *mbody = parse_block();
                    expect(TOK_RBRACE, "Expected '}'");
                    ASTNode *mfunc = new_node(NODE_FUNC_DECL);
                    mfunc->func_decl.name = mname;
                    mfunc->func_decl.params = mparams;
                    mfunc->func_decl.param
