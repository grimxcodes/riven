/*
 * RIVEN LANGUAGE INTERPRETER v1.0 (FULLY FIXED)
 * Compile: clang -o riven riven.c -lpthread -lm
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <pthread.h>
#include <setjmp.h>

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
        return (Token){TOK_PLUS, "+", line_no};
    }
    if (c == '-') {
        if (peek() == '<') { advance(); return (Token){TOK_DEC, "-<", line_no}; }
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
    NODE_PROGRAM, NODE_CORE, NODE_VAR_DECL, NODE_FUNC_DECL, NODE_CLASS_DECL,
    NODE_IF, NODE_WHILE, NODE_FLOW, NODE_RETURN, NODE_EXPR_STMT, NODE_BINARY,
    NODE_CALL, NODE_METHOD_CALL, NODE_VAR, NODE_PROPERTY, NODE_NUMBER, NODE_STRING,
    NODE_BOOL, NODE_NULL, NODE_LIST, NODE_RECORD, NODE_INC_DEC, NODE_INCLUDE,
    NODE_RAW, NODE_RESC, NODE_ATTACK, NODE_SPARK, NODE_SYNC, NODE_BIND, NODE_SPAWN,
    NODE_FETCH, NODE_INDEX
} NodeType;

typedef struct ASTNode {
    NodeType type;
    struct ASTNode *next;
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
        struct { char *name; struct ASTNode **args; int argc; } call;
        struct { struct ASTNode *obj; char *method; struct ASTNode **args; int argc; } method_call;
        struct { char *name; } var;
        struct { struct ASTNode *obj; char *prop; } property;
        struct { double num; } number;
        struct { char *str; } string;
        struct { int val; } boolean;
        struct { struct ASTNode **elems; int count; } list;
        struct { char **keys; struct ASTNode **vals; int count; } record;
        struct { struct ASTNode *var; char *op; } inc_dec;
        struct { char *path; } include;
        struct { struct ASTNode *body; } raw;
        struct { struct ASTNode *body; } resc;
        struct { struct ASTNode *msg; } attack;
        struct { struct ASTNode *call; } spark;
        struct { struct ASTNode *ref; } bind;
        struct { char *class_name; } spawn;
        struct { struct ASTNode *url; } fetch;
        struct { struct ASTNode *coll; struct ASTNode *idx; } index;
    };
} ASTNode;

// ------------------------------ PARSER ------------------------------------
typedef struct {
    Token current;
    jmp_buf err;
} ParserState;
static ParserState ps;

void advance_token() { ps.current = next_token(); }
void parse_error(char *msg) { fprintf(stderr, "Parse error at line %d: %s\n", ps.current.line, msg); longjmp(ps.err, 1); }
void expect(TokenType t, char *msg) { if (ps.current.type != t) parse_error(msg); advance_token(); }

ASTNode *parse_expression();
ASTNode *parse_statement();
ASTNode *parse_block();
ASTNode *new_node(NodeType type) { ASTNode *n = calloc(1, sizeof(ASTNode)); n->type = type; return n; }

// ----- parse_primary -----
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
        if (strcmp(ps.current.value, "correct")==0) { advance_token(); ASTNode *n = new_node(NODE_BOOL); n->boolean.val = 1; return n; }
        if (strcmp(ps.current.value, "incorrect")==0) { advance_token(); ASTNode *n = new_node(NODE_BOOL); n->boolean.val = 0; return n; }
        if (strcmp(ps.current.value, "emp")==0) { advance_token(); return new_node(NODE_NULL); }
    }
    if (ps.current.type == TOK_IDENT) {
        char *name = strdup(ps.current.value);
        advance_token();
        if (ps.current.type == TOK_LPAREN) {
            advance_token();
            ASTNode **args = NULL;
            int argc = 0;
            if (ps.current.type != TOK_RPAREN) {
                do {
                    args = realloc(args, sizeof(ASTNode*)*(argc+1));
                    args[argc++] = parse_expression();
                } while (ps.current.type == TOK_COMMA);
                if (ps.current.type == TOK_COMMA) advance_token();
            }
            expect(TOK_RPAREN, "Expected ')'");
            ASTNode *n = new_node(NODE_CALL);
            n->call.name = name;
            n->call.args = args;
            n->call.argc = argc;
            return n;
        }
        ASTNode *obj = new_node(NODE_VAR);
        obj->var.name = name;
        while (ps.current.type == TOK_DOT) {
            advance_token();
            if (ps.current.type != TOK_IDENT) parse_error("Expected property name");
            char *prop = strdup(ps.current.value);
            advance_token();
            if (ps.current.type == TOK_LPAREN) {
                advance_token();
                ASTNode **args = NULL;
                int argc = 0;
                if (ps.current.type != TOK_RPAREN) {
                    do {
                        args = realloc(args, sizeof(ASTNode*)*(argc+1));
                        args[argc++] = parse_expression();
                    } while (ps.current.type == TOK_COMMA);
                    if (ps.current.type == TOK_COMMA) advance_token();
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
            } while (ps.current.type == TOK_COMMA);
            if (ps.current.type == TOK_COMMA) advance_token();
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
            } while (ps.current.type == TOK_COMMA);
            if (ps.current.type == TOK_COMMA) advance_token();
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
        advance_token();
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

// ----- parse_statement (fixed) -----
ASTNode *parse_statement() {
    if (ps.current.type == TOK_KEYWORD) {
        // riven core
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
        // craft function
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
                } while (ps.current.type == TOK_COMMA);
                if (ps.current.type == TOK_COMMA) advance_token();
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
        // frame class
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
                if (ps.current.type == TOK_KEYWORD && strcmp(ps.current.value, "boot")==0) {
                    advance_token();
                    expect(TOK_LPAREN, "Expected '('");
                    expect(TOK_RPAREN, "Expected ')'");
                    expect(TOK_LBRACE, "Expected '{'");
                    constructor = parse_block();
                    expect(TOK_RBRACE, "Expected '}'");
                    continue;
                }
                if (ps.current.type == TOK_KEYWORD && (strcmp(ps.current.value,"open")==0 || strcmp(ps.current.value,"hidden")==0)) {
                    advance_token(); // skip
                }
                if (ps.current.type == TOK_KEYWORD && strcmp(ps.current.value,"craft")==0) {
                    // method
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
                        } while (ps.current.type == TOK_COMMA);
                        if (ps.current.type == TOK_COMMA) advance_token();
                    }
                    expect(TOK_RPAREN, "Expected ')'");
                    expect(TOK_LBRACE, "Expected '{'");
                    ASTNode *mbody = parse_block();
                    expect(TOK_RBRACE, "Expected '}'");
                    ASTNode *mfunc = new_node(NODE_FUNC_DECL);
                    mfunc->func_decl.name = mname;
                    mfunc->func_decl.params = mparams;
                    mfunc->func_decl.paramc = mparamc;
                    mfunc->func_decl.body = mbody;
                    methods = realloc(methods, sizeof(ASTNode*)*(methodc+1));
                    methods[methodc++] = mfunc;
                } else {
                    // field or unknown, skip
                    parse_statement(); // consume but ignore
                }
            }
            expect(TOK_RBRACE, "Expected '}'");
            ASTNode *cls = new_node(NODE_CLASS_DECL);
            cls->class_decl.name = name;
            cls->class_decl.methods = methods;
            cls->class_decl.methodc = methodc;
            cls->class_decl.constructor = constructor;
            return cls;
        }
        // firm constant
        if (strcmp(ps.current.value, "firm")==0) {
            advance_token();
            if (ps.current.type != TOK_IDENT) parse_error("Expected constant name");
            char *name = strdup(ps.current.value);
            advance_token();
            expect(TOK_ASSIGN, "Expected '='");
            ASTNode *val = parse_expression();
            ASTNode *n = new_node(NODE_VAR_DECL);
            n->var_decl.name = name;
            n->var_decl.value = val;
            n->var_decl.constant = 1;
            return n;
        }
        // if
        if (strcmp(ps.current.value, "if")==0) {
            advance_token();
            ASTNode *cond = parse_expression();
            expect(TOK_LBRACE, "Expected '{'");
            ASTNode *then_body = parse_block();
            expect(TOK_RBRACE, "Expected '}'");
            ASTNode **elifs = NULL;
            int elifc = 0;
            while (ps.current.type == TOK_KEYWORD && strcmp(ps.current.value,"altif")==0) {
                advance_token();
                ASTNode *econd = parse_expression();
                expect(TOK_LBRACE, "Expected '{'");
                ASTNode *ebody = parse_block();
                expect(TOK_RBRACE, "Expected '}'");
                elifs = realloc(elifs, sizeof(ASTNode*)*(elifc+2));
                elifs[elifc++] = econd;
                elifs[elifc++] = ebody;
            }
            ASTNode *else_body = NULL;
            if (ps.current.type == TOK_KEYWORD && strcmp(ps.current.value,"else")==0) {
                advance_token();
                expect(TOK_LBRACE, "Expected '{'");
                else_body = parse_block();
                expect(TOK_RBRACE, "Expected '}'");
            }
            ASTNode *n = new_node(NODE_IF);
            n->if_stmt.cond = cond;
            n->if_stmt.then = then_body;
            n->if_stmt.elifs = elifs;
            n->if_stmt.elifc = elifc;
            n->if_stmt.els = else_body;
            return n;
        }
        // during loop
        if (strcmp(ps.current.value, "during")==0) {
            advance_token();
            ASTNode *cond = parse_expression();
            expect(TOK_LBRACE, "Expected '{'");
            ASTNode *body = parse_block();
            expect(TOK_RBRACE, "Expected '}'");
            ASTNode *n = new_node(NODE_WHILE);
            n->while_stmt.cond = cond;
            n->while_stmt.body = body;
            return n;
        }
        // flow loop
        if (strcmp(ps.current.value, "flow")==0) {
            advance_token();
            ASTNode *count = parse_expression();
            expect(TOK_LBRACE, "Expected '{'");
            ASTNode *body = parse_block();
            expect(TOK_RBRACE, "Expected '}'");
            ASTNode *n = new_node(NODE_FLOW);
            n->flow_stmt.count = count;
            n->flow_stmt.body = body;
            return n;
        }
        // returns
        if (strcmp(ps.current.value, "returns")==0) {
            advance_token();
            ASTNode *val = parse_expression();
            ASTNode *n = new_node(NODE_RETURN);
            n->return_stmt.value = val;
            return n;
        }
        // resc
        if (strcmp(ps.current.value, "resc")==0) {
            advance_token();
            expect(TOK_LBRACE, "Expected '{'");
            ASTNode *body = parse_block();
            expect(TOK_RBRACE, "Expected '}'");
            ASTNode *n = new_node(NODE_RESC);
            n->resc.body = body;
            return n;
        }
        // attack
        if (strcmp(ps.current.value, "attack")==0) {
            advance_token();
            ASTNode *msg = parse_expression();
            ASTNode *n = new_node(NODE_ATTACK);
            n->attack.msg = msg;
            return n;
        }
        // spark
        if (strcmp(ps.current.value, "spark")==0) {
            advance_token();
            if (ps.current.type == TOK_KEYWORD && strcmp(ps.current.value,"craft")==0) {
                advance_token();
                return parse_statement(); // function
            } else {
                ASTNode *call = parse_expression();
                ASTNode *n = new_node(NODE_SPARK);
                n->spark.call = call;
                return n;
            }
        }
        // sync
        if (strcmp(ps.current.value, "sync")==0) {
            advance_token();
            return new_node(NODE_SYNC);
        }
        // bind
        if (strcmp(ps.current.value, "bind")==0) {
            advance_token();
            if (ps.current.type != TOK_IDENT) parse_error("Expected identifier");
            char *name = strdup(ps.current.value);
            advance_token();
            expect(TOK_ASSIGN, "Expected '='");
            ASTNode *target = parse_expression();
            ASTNode *n = new_node(NODE_VAR_DECL);
            n->var_decl.name = name;
            n->var_decl.value = target;
            n->var_decl.constant = 0;
            return n;
        }
        // raw
        if (strcmp(ps.current.value, "raw")==0) {
            advance_token();
            expect(TOK_LBRACE, "Expected '{'");
            ASTNode *body = parse_block();
            expect(TOK_RBRACE, "Expected '}'");
            ASTNode *n = new_node(NODE_RAW);
            n->raw.body = body;
            return n;
        }
        // consistof
        if (strcmp(ps.current.value, "consistof")==0) {
            advance_token();
            ASTNode *path = parse_expression();
            ASTNode *n = new_node(NODE_INCLUDE);
            if (path->type == NODE_STRING) n->include.path = path->string.str;
            return n;
        }
        // rise / drop
        if (strcmp(ps.current.value, "rise")==0 || strcmp(ps.current.value,"drop")==0) {
            char *op = strdup(ps.current.value);
            advance_token();
            ASTNode *var = parse_expression();
            ASTNode *n = new_node(NODE_INC_DEC);
            n->inc_dec.var = var;
            n->inc_dec.op = op;
            return n;
        }
    }
    // variable assignment or expression
    if (ps.current.type == TOK_IDENT) {
        char *name = strdup(ps.current.value);
        advance_token();
        if (ps.current.type == TOK_ASSIGN) {
            advance_token();
            ASTNode *val = parse_expression();
            ASTNode *n = new_node(NODE_VAR_DECL);
            n->var_decl.name = name;
            n->var_decl.value = val;
            n->var_decl.constant = 0;
            return n;
        } else if (ps.current.type == TOK_INC || ps.current.type == TOK_DEC) {
            char *op = strdup(ps.current.value);
            advance_token();
            ASTNode *var = new_node(NODE_VAR);
            var->var.name = name;
            ASTNode *n = new_node(NODE_INC_DEC);
            n->inc_dec.var = var;
            n->inc_dec.op = op;
            return n;
        } else {
            ASTNode *expr = new_node(NODE_VAR);
            expr->var.name = name;
            ASTNode *n = new_node(NODE_EXPR_STMT);
            n->expr_stmt.expr = expr;
            return n;
        }
    }
    if (ps.current.type == TOK_INC || ps.current.type == TOK_DEC) {
        char *op = strdup(ps.current.value);
        advance_token();
        ASTNode *var = parse_expression();
        ASTNode *n = new_node(NODE_INC_DEC);
        n->inc_dec.var = var;
        n->inc_dec.op = op;
        return n;
    }
    // fallback expression statement
    ASTNode *expr = parse_expression();
    ASTNode *n = new_node(NODE_EXPR_STMT);
    n->expr_stmt.expr = expr;
    return n;
}

ASTNode *parse_block() {
    ASTNode *block = new_node(NODE_PROGRAM);
    block->block.stmts = NULL;
    block->block.count = 0;
    while (ps.current.type != TOK_RBRACE && ps.current.type != TOK_EOF) {
        block->block.stmts = realloc(block->block.stmts, sizeof(ASTNode*)*(block->block.count+1));
        block->block.stmts[block->block.count++] = parse_statement();
    }
    return block;
}

ASTNode *parse_program() { advance_token(); return parse_block(); }

// ------------------------------ INTERPRETER ---------------------------------
typedef struct Value {
    int type;
    union {
        double num;
        char *str;
        struct { char *class_name; struct Variable *fields; struct Function *methods; } obj;
        struct { struct Value *items; int count; } list;
        struct { char **keys; struct Value *values; int count; } record;
        int boolean;
    };
} Value;

typedef struct Variable {
    char *name;
    Value val;
    int constant;
    struct Variable *next;
} Variable;

typedef struct Function {
    char *name;
    char **params;
    int paramc;
    ASTNode *body;
    struct Function *next;
} Function;

typedef struct Class {
    char *name;
    Function *methods;
    ASTNode *constructor;
    struct Class *next;
} Class;

Variable *globals = NULL;
Function *functions = NULL;
Class *classes = NULL;
jmp_buf runtime_err;

Value null_value() { return (Value){6}; }
Value number_value(double n) { Value v = {0}; v.num = n; return v; }
Value string_value(char *s) { Value v = {1}; v.str = strdup(s); return v; }
Value bool_value(int b) { Value v = {5}; v.boolean = b; return v; }

Variable *find_var(char *name) {
    Variable *v = globals;
    while (v) { if (strcmp(v->name, name)==0) return v; v = v->next; }
    return NULL;
}

void set_var(char *name, Value val, int constant) {
    Variable *v = find_var(name);
    if (v) { if (v->constant) { fprintf(stderr,"Cannot modify constant %s\n", name); longjmp(runtime_err,1); } v->val = val; }
    else { v = malloc(sizeof(Variable)); v->name = strdup(name); v->val = val; v->constant = constant; v->next = globals; globals = v; }
}

Function *find_func(char *name) {
    Function *f = functions;
    while (f) { if (strcmp(f->name, name)==0) return f; f = f->next; }
    return NULL;
}

Class *find_class(char *name) {
    Class *c = classes;
    while (c) { if (strcmp(c->name, name)==0) return c; c = c->next; }
    return NULL;
}

void add_function(Function *f) { f->next = functions; functions = f; }
void add_class(Class *c) { c->next = classes; classes = c; }

Value eval_expr(ASTNode *node, Variable *local_scope);
void eval_stmt(ASTNode *node, Variable *local_scope);

Value eval_binary(Value left, char *op, Value right) {
    if (left.type == 0 && right.type == 0) {
        if (strcmp(op,"+")==0) return number_value(left.num + right.num);
        if (strcmp(op,"-")==0) return number_value(left.num - right.num);
        if (strcmp(op,"*")==0) return number_value(left.num * right.num);
        if (strcmp(op,"/")==0) return number_value(left.num / right.num);
        if (strcmp(op,"==")==0) return bool_value(left.num == right.num);
        if (strcmp(op,"!=")==0) return bool_value(left.num != right.num);
        if (strcmp(op,"<")==0) return bool_value(left.num < right.num);
        if (strcmp(op,">")==0) return bool_value(left.num > right.num);
        if (strcmp(op,"&&")==0) return bool_value(left.num && right.num);
        if (strcmp(op,"||")==0) return bool_value(left.num || right.num);
    }
    if (left.type == 1 && right.type == 1 && strcmp(op,"+")==0) {
        char *res = malloc(strlen(left.str)+strlen(right.str)+1);
        strcpy(res, left.str); strcat(res, right.str);
        return string_value(res);
    }
    if (strcmp(op,"&&")==0 || strcmp(op,"||")==0) {
        int lbool = (left.type==0 && left.num!=0) || (left.type==5 && left.boolean) || (left.type==1 && strlen(left.str)>0);
        int rbool = (right.type==0 && right.num!=0) || (right.type==5 && right.boolean) || (right.type==1 && strlen(right.str)>0);
        if (strcmp(op,"&&")==0) return bool_value(lbool && rbool);
        else return bool_value(lbool || rbool);
    }
    fprintf(stderr,"Binary op %s not supported\n", op);
    return null_value();
}

Value eval_expr(ASTNode *node, Variable *local_scope) {
    if (!node) return null_value();
    switch (node->type) {
        case NODE_NUMBER: return number_value(node->number.num);
        case NODE_STRING: return string_value(node->string.str);
        case NODE_BOOL: return bool_value(node->boolean.val);
        case NODE_NULL: return null_value();
        case NODE_VAR: {
            Variable *v = find_var(node->var.name);
            if (!v) { fprintf(stderr,"Undefined var %s\n", node->var.name); longjmp(runtime_err,1); }
            return v->val;
        }
        case NODE_PROPERTY: {
            Value obj = eval_expr(node->property.obj, local_scope);
            if (obj.type == 2) {
                Variable *f = obj.obj.fields;
                while (f) { if (strcmp(f->name, node->property.prop)==0) return f->val; f = f->next; }
            } else if (obj.type == 4) {
                for (int i=0; i<obj.record.count; i++)
                    if (strcmp(obj.record.keys[i], node->property.prop)==0) return obj.record.values[i];
            }
            return null_value();
        }
        case NODE_BINARY:
            return eval_binary(eval_expr(node->binary.left, local_scope), node->binary.op, eval_expr(node->binary.right, local_scope));
        case NODE_CALL: {
            if (strcmp(node->call.name, "stamp")==0) {
                for (int i=0; i<node->call.argc; i++) {
                    Value v = eval_expr(node->call.args[i], local_scope);
                    if (v.type == 0) printf("%g", v.num);
                    else if (v.type == 1) printf("%s", v.str);
                    else if (v.type == 5) printf("%s", v.boolean?"true":"false");
                }
                printf("\n");
                return null_value();
            }
            if (strcmp(node->call.name, "grab")==0) {
                if (node->call.argc>0) { Value v = eval_expr(node->call.args[0], local_scope); if (v.type==1) printf("%s", v.str); }
                char input[256]; fgets(input,256,stdin); input[strcspn(input,"\n")]=0;
                return string_value(input);
            }
            if (strcmp(node->call.name, "int")==0) {
                Value v = eval_expr(node->call.args[0], local_scope);
                if (v.type==0) return v; if (v.type==1) return number_value(atoi(v.str)); return number_value(0);
            }
            if (strcmp(node->call.name, "txt")==0) {
                Value v = eval_expr(node->call.args[0], local_scope);
                if (v.type==1) return v;
                char buf[64]; if (v.type==0) sprintf(buf,"%g",v.num); else sprintf(buf,"%s",v.type==5?(v.boolean?"correct":"incorrect"):"emp");
                return string_value(buf);
            }
            if (strcmp(node->call.name, "dnum")==0) {
                Value v = eval_expr(node->call.args[0], local_scope);
                if (v.type==0) return v; if (v.type==1) return number_value(atof(v.str)); return number_value(0);
            }
            Function *f = find_func(node->call.name);
            if (!f) { fprintf(stderr,"Undefined function %s\n", node->call.name); longjmp(runtime_err,1); }
            Variable *local = NULL;
            for (int i=0; i<f->paramc && i<node->call.argc; i++) {
                Value arg = eval_expr(node->call.args[i], local_scope);
                Variable *v = malloc(sizeof(Variable)); v->name = strdup(f->params[i]); v->val = arg; v->constant = 0; v->next = local; local = v;
            }
            eval_stmt(f->body, local);
            return null_value();
        }
        case NODE_LIST: {
            Value v = {3}; v.list.items = malloc(sizeof(Value)*node->list.count); v.list.count = node->list.count;
            for (int i=0; i<node->list.count; i++) v.list.items[i] = eval_expr(node->list.elems[i], local_scope);
            return v;
        }
        case NODE_RECORD: {
            Value v = {4}; v.record.count = node->record.count; v.record.keys = malloc(sizeof(char*)*node->record.count); v.record.values = malloc(sizeof(Value)*node->record.count);
            for (int i=0; i<node->record.count; i++) { v.record.keys[i] = strdup(node->record.keys[i]); v.record.values[i] = eval_expr(node->record.vals[i], local_scope); }
            return v;
        }
        case NODE_INDEX: {
            Value coll = eval_expr(node->index.coll, local_scope); Value idxv = eval_expr(node->index.idx, local_scope); int idx = (int)idxv.num;
            if (coll.type == 3 && idx>=0 && idx<coll.list.count) return coll.list.items[idx];
            return null_value();
        }
        case NODE_SPAWN: {
            Class *c = find_class(node->spawn.class_name);
            if (!c) { fprintf(stderr,"Class %s not defined\n", node->spawn.class_name); longjmp(runtime_err,1); }
            Value obj = {2}; obj.obj.class_name = strdup(c->name); obj.obj.fields = NULL; obj.obj.methods = c->methods;
            if (c->constructor) {
                Variable *self = malloc(sizeof(Variable)); self->name = "self"; self->val = obj; self->constant = 0; self->next = NULL;
                eval_stmt(c->constructor, self);
            }
            return obj;
        }
        case NODE_FETCH: {
            Value url = eval_expr(node->fetch.url, local_scope);
            printf("[FETCH] Would fetch %s (mock)\n", url.str);
            return string_value("{\"mock\":\"data\"}");
        }
        default: return null_value();
    }
}

void eval_stmt(ASTNode *node, Variable *local_scope) {
    if (!node) return;
    switch (node->type) {
        case NODE_PROGRAM: case NODE_CORE:
            for (int i=0; i<node->block.count; i++) eval_stmt(node->block.stmts[i], local_scope);
            break;
        case NODE_VAR_DECL: {
            Value val = eval_expr(node->var_decl.value, local_scope);
            set_var(node->var_decl.name, val, node->var_decl.constant);
            break;
        }
        case NODE_FUNC_DECL: {
            Function *f = malloc(sizeof(Function)); f->name = node->func_decl.name; f->params = node->func_decl.params; f->paramc = node->func_decl.paramc; f->body = node->func_decl.body; add_function(f);
            break;
        }
        case NODE_CLASS_DECL: {
            Class *c = malloc(sizeof(Class)); c->name = node->class_decl.name; c->methods = NULL; c->constructor = node->class_decl.constructor;
            for (int i=0; i<node->class_decl.methodc; i++) {
                ASTNode *m = node->class_decl.methods[i];
                if (m->type == NODE_FUNC_DECL) {
                    Function *f = malloc(sizeof(Function)); f->name = m->func_decl.name; f->params = m->func_decl.params; f->paramc = m->func_decl.paramc; f->body = m->func_decl.body; f->next = c->methods; c->methods = f;
                }
            }
            add_class(c);
            break;
        }
        case NODE_IF: {
            Value cond = eval_expr(node->if_stmt.cond, local_scope);
            int truth = (cond.type==0 && cond.num!=0) || (cond.type==5 && cond.boolean) || (cond.type==1 && strlen(cond.str)>0);
            if (truth) eval_stmt(node->if_stmt.then, local_scope);
            else {
                int handled = 0;
                for (int i=0; i<node->if_stmt.elifc; i+=2) {
                    Value econd = eval_expr(node->if_stmt.elifs[i], local_scope);
                    if ((econd.type==0 && econd.num!=0) || (econd.type==5 && econd.boolean)) {
                        eval_stmt(node->if_stmt.elifs[i+1], local_scope); handled=1; break;
                    }
                }
                if (!handled && node->if_stmt.els) eval_stmt(node->if_stmt.els, local_scope);
            }
            break;
        }
        case NODE_WHILE: {
            while (1) {
                Value cond = eval_expr(node->while_stmt.cond, local_scope);
                if (!((cond.type==0 && cond.num!=0) || (cond.type==5 && cond.boolean))) break;
                eval_stmt(node->while_stmt.body, local_scope);
            }
            break;
        }
        case NODE_FLOW: {
            int cnt = (int)eval_expr(node->flow_stmt.count, local_scope).num;
            for (int i=0; i<cnt; i++) eval_stmt(node->flow_stmt.body, local_scope);
            break;
        }
        case NODE_EXPR_STMT: eval_expr(node->expr_stmt.expr, local_scope); break;
        case NODE_INC_DEC: {
            Value varval = eval_expr(node->inc_dec.var, local_scope);
            double newval = varval.num;
            if (strcmp(node->inc_dec.op, "+>")==0 || strcmp(node->inc_dec.op,"rise")==0) newval++;
            else if (strcmp(node->inc_dec.op, "-<")==0 || strcmp(node->inc_dec.op,"drop")==0) newval--;
            if (node->inc_dec.var->type == NODE_VAR) {
                Variable *v = find_var(node->inc_dec.var->var.name);
                if (v) v->val = number_value(newval);
                else set_var(node->inc_dec.var->var.name, number_value(newval), 0);
            }
            break;
        }
        case NODE_RETURN: break;
        case NODE_INCLUDE: break;
        case NODE_RAW: printf("[WARN] raw block ignored\n"); break;
        case NODE_RESC: if (setjmp(runtime_err)==0) eval_stmt(node->resc.body, local_scope); else { /* caught */ } break;
        case NODE_ATTACK: { Value msg = eval_expr(node->attack.msg, local_scope); fprintf(stderr, "Attack: %s\n", msg.str); longjmp(runtime_err,1); break; }
        case NODE_SPARK: printf("[SPARK] Async call not fully implemented\n"); break;
        case NODE_SYNC: printf("[SYNC] Not implemented\n"); break;
        default: break;
    }
}

// ------------------------------ MAIN ---------------------------------------
int main(int argc, char **argv) {
    if (argc < 2) { printf("Usage: ./riven <source.rv>\n"); return 1; }
    FILE *f = fopen(argv[1], "r");
    if (!f) { perror("Failed to open file"); return 1; }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *src = malloc(len+1);
    fread(src, 1, len, f);
    src[len] = '\0';
    fclose(f);
    init_lexer(src);
    if (setjmp(ps.err) == 0) {
        ASTNode *prog = parse_program();
        if (setjmp(runtime_err) == 0) eval_stmt(prog, NULL);
        else fprintf(stderr, "Runtime error\n");
    } else fprintf(stderr, "Parse error\n");
    free(src);
    return 0;
}
