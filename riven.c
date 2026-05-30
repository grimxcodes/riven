// riven.c
#include "riven.h"

// ---------- Lexer Implementation (simplified) ----------
static const char* src;
static int pos;
static int line = 1;

void init_lexer(const char* s) { src = s; pos = 0; line = 1; }

static char peek() { return src[pos]; }
static char advance() { char c = src[pos++]; if (c=='\n') line++; return c; }

static Token make_token(TokenType type, char* val) {
    Token t = {type, strdup(val), line}; return t;
}

static Token parse_ident_or_keyword() {
    char buf[64]; int i=0;
    while (isalnum(peek()) || peek()=='_') buf[i++] = advance();
    buf[i]=0;
    if (strcmp(buf,"craft")==0) return make_token(TOK_KEYWORD, buf);
    if (strcmp(buf,"if")==0) return make_token(TOK_KEYWORD, buf);
    if (strcmp(buf,"during")==0) return make_token(TOK_KEYWORD, buf);
    if (strcmp(buf,"stamp")==0) return make_token(TOK_KEYWORD, buf);
    return make_token(TOK_IDENT, buf);
}

static Token parse_number() {
    char buf[32]; int i=0;
    while (isdigit(peek()) || peek()=='.') buf[i++] = advance();
    buf[i]=0;
    return make_token(TOK_NUMBER, buf);
}

static Token parse_string() {
    advance(); // skip "
    char buf[256]; int i=0;
    while (peek() != '"' && peek() != 0) buf[i++] = advance();
    advance(); // closing "
    buf[i]=0;
    return make_token(TOK_STRING, buf);
}

Token next_token() {
    while (isspace(peek())) advance();
    if (peek() == 0) return make_token(TOK_EOF, "");
    char c = peek();
    if (isalpha(c)) return parse_ident_or_keyword();
    if (isdigit(c)) return parse_number();
    if (c == '"') return parse_string();
    advance();
    switch (c) {
        case '{': return make_token(TOK_LBRACE, "{");
        case '}': return make_token(TOK_RBRACE, "}");
        case '(': return make_token(TOK_LPAREN, "(");
        case ')': return make_token(TOK_RPAREN, ")");
        case '=': if (peek() == '=') { advance(); return make_token(TOK_EQ, "=="); }
                  return make_token(TOK_ASSIGN, "=");
        case ';': return make_token(TOK_SEMI, ";");
        case '+': if (peek() == '>') { advance(); return make_token(TOK_INC, "+>"); }
                  return make_token(TOK_PLUS, "+");
        case '-': if (peek() == '<') { advance(); return make_token(TOK_DEC, "-<"); }
                  return make_token(TOK_MINUS, "-");
        case '*': return make_token(TOK_STAR, "*");
        case '/': return make_token(TOK_SLASH, "/");
        case '<': return make_token(TOK_LT, "<");
        case '>': return make_token(TOK_GT, ">");
        case '!': if (peek() == '=') { advance(); return make_token(TOK_NE, "!="); }
                  return make_token(TOK_NOT, "!");
        case '&': if (peek() == '&') { advance(); return make_token(TOK_AND, "&&"); } break;
        case '|': if (peek() == '|') { advance(); return make_token(TOK_OR, "||"); } break;
        case ',': return make_token(TOK_COMMA, ",");
        case '.': return make_token(TOK_DOT, ".");
    }
    return make_token(TOK_EOF, "");
}

// ---------- Parser (Recursive Descent) ----------
Token current;
void advance_token() { current = next_token(); }

ASTNode* parse_expression();
ASTNode* parse_primary() {
    if (current.type == TOK_NUMBER) {
        ASTNode* node = calloc(1, sizeof(ASTNode));
        node->type = NODE_NUM;
        node->num.num = atof(current.value);
        advance_token();
        return node;
    }
    if (current.type == TOK_STRING) {
        ASTNode* node = calloc(1, sizeof(ASTNode));
        node->type = NODE_STR;
        node->str.str = strdup(current.value);
        advance_token();
        return node;
    }
    if (current.type == TOK_IDENT) {
        char* name = strdup(current.value);
        advance_token();
        if (current.type == TOK_LPAREN) { // function call
            advance_token();
            ASTNode** args = NULL;
            int argc = 0;
            if (current.type != TOK_RPAREN) {
                args = realloc(args, sizeof(ASTNode*)*(argc+1));
                args[argc++] = parse_expression();
                while (current.type == TOK_COMMA) {
                    advance_token();
                    args = realloc(args, sizeof(ASTNode*)*(argc+1));
                    args[argc++] = parse_expression();
                }
            }
            advance_token(); // consume ')'
            ASTNode* node = calloc(1, sizeof(ASTNode));
            node->type = NODE_CALL;
            node->call.func = name;
            node->call.args = args;
            node->call.argc = argc;
            return node;
        }
        // variable reference
        ASTNode* node = calloc(1, sizeof(ASTNode));
        node->type = NODE_VAR;
        node->var.name = name;
        return node;
    }
    if (current.type == TOK_LPAREN) {
        advance_token();
        ASTNode* expr = parse_expression();
        if (current.type == TOK_RPAREN) advance_token();
        return expr;
    }
    return NULL;
}

ASTNode* parse_binop(int min_prec) {
    ASTNode* left = parse_primary();
    while (1) {
        int prec = 0;
        char op = 0;
        if (current.type == TOK_PLUS) { prec=10; op='+'; }
        else if (current.type == TOK_MINUS) { prec=10; op='-'; }
        else if (current.type == TOK_STAR) { prec=20; op='*'; }
        else if (current.type == TOK_SLASH) { prec=20; op='/'; }
        else if (current.type == TOK_EQ) { prec=5; op='='; }
        else if (current.type == TOK_LT) { prec=5; op='<'; }
        else if (current.type == TOK_GT) { prec=5; op='>'; }
        else break;
        if (prec < min_prec) break;
        advance_token();
        ASTNode* right = parse_binop(prec+1);
        ASTNode* node = calloc(1, sizeof(ASTNode));
        node->type = NODE_BINOP;
        node->binop.left = left;
        node->binop.op = op;
        node->binop.right = right;
        left = node;
    }
    return left;
}
ASTNode* parse_expression() { return parse_binop(0); }

ASTNode* parse_statement() {
    if (current.type == TOK_KEYWORD && strcmp(current.value,"if")==0) {
        advance_token();
        ASTNode* cond = parse_expression();
        if (current.type != TOK_LBRACE) { fprintf(stderr,"Expected {\n"); longjmp(error_jmp,1); }
        advance_token();
        ASTNode* then_body = parse_statement(); // for simplicity, single statement
        if (current.type == TOK_RBRACE) advance_token();
        ASTNode* else_body = NULL;
        if (current.type == TOK_KEYWORD && strcmp(current.value,"else")==0) {
            advance_token();
            if (current.type != TOK_LBRACE) { fprintf(stderr,"Expected {\n"); longjmp(error_jmp,1); }
            advance_token();
            else_body = parse_statement();
            if (current.type == TOK_RBRACE) advance_token();
        }
        ASTNode* node = calloc(1,sizeof(ASTNode));
        node->type = NODE_IF;
        node->ifstmt.cond = cond;
        node->ifstmt.then = then_body;
        node->ifstmt.els = else_body;
        return node;
    }
    if (current.type == TOK_KEYWORD && strcmp(current.value,"during")==0) {
        advance_token();
        ASTNode* cond = parse_expression();
        if (current.type != TOK_LBRACE) { fprintf(stderr,"Expected {\n"); longjmp(error_jmp,1); }
        advance_token();
        ASTNode* body = parse_statement();
        if (current.type == TOK_RBRACE) advance_token();
        ASTNode* node = calloc(1,sizeof(ASTNode));
        node->type = NODE_WHILE;
        node->whilestmt.cond = cond;
        node->whilestmt.body = body;
        return node;
    }
    // assignment or expression
    ASTNode* expr = parse_expression();
    if (current.type == TOK_SEMI) advance_token();
    return expr;
}

ASTNode* parse_block() {
    ASTNode** stmts = NULL;
    int count = 0;
    while (current.type != TOK_RBRACE && current.type != TOK_EOF) {
        stmts = realloc(stmts, sizeof(ASTNode*)*(count+1));
        stmts[count++] = parse_statement();
    }
    if (current.type == TOK_RBRACE) advance_token();
    ASTNode* node = calloc(1,sizeof(ASTNode));
    node->type = NODE_BLOCK;
    node->block.stmts = stmts;
    node->block.count = count;
    return node;
}

ASTNode* parse_program() {
    advance_token();
    return parse_block();
}

// ---------- Interpreter (Variable store & eval) ----------
Variable* find_var(const char* name) {
    Variable* v = global_vars;
    while (v) {
        if (strcmp(v->name, name)==0) return v;
        v = v->next;
    }
    return NULL;
}

void set_var(const char* name, Value val) {
    Variable* v = find_var(name);
    if (!v) {
        v = malloc(sizeof(Variable));
        v->name = strdup(name);
        v->next = global_vars;
        global_vars = v;
    }
    v->val = val;
}

Value eval(ASTNode* n) {
    if (!n) return (Value){.type=2};
    switch (n->type) {
        case NODE_NUM: return (Value){.type=0, .num_val=n->num.num};
        case NODE_STR: return (Value){.type=1, .str_val=n->str.str};
        case NODE_VAR: {
            Variable* v = find_var(n->var.name);
            if (!v) { fprintf(stderr,"Undefined var %s\n", n->var.name); longjmp(error_jmp,1); }
            return v->val;
        }
        case NODE_BINOP: {
            Value l = eval(n->binop.left);
            Value r = eval(n->binop.right);
            if (l.type==0 && r.type==0) {
                double res=0;
                if (n->binop.op == '+') res = l.num_val + r.num_val;
                else if (n->binop.op == '-') res = l.num_val - r.num_val;
                else if (n->binop.op == '*') res = l.num_val * r.num_val;
                else if (n->binop.op == '/') res = l.num_val / r.num_val;
                else if (n->binop.op == '<') res = l.num_val < r.num_val;
                else if (n->binop.op == '=') res = l.num_val == r.num_val;
                return (Value){.type=0, .num_val=res};
            }
            break;
        }
        case NODE_CALL: {
            if (strcmp(n->call.func,"stamp")==0) {
                for (int i=0;i<n->call.argc;i++) {
                    Value v = eval(n->call.args[i]);
                    if (v.type==0) printf("%g", v.num_val);
                    else if (v.type==1) printf("%s", v.str_val);
                }
                printf("\n");
                return (Value){.type=2};
            }
            break;
        }
        case NODE_IF: {
            Value cond = eval(n->ifstmt.cond);
            if (cond.type==0 && cond.num_val != 0) eval(n->ifstmt.then);
            else if (n->ifstmt.els) eval(n->ifstmt.els);
            return (Value){.type=2};
        }
        case NODE_WHILE: {
            Value cond = eval(n->whilestmt.cond);
            while (cond.type==0 && cond.num_val != 0) {
                eval(n->whilestmt.body);
                cond = eval(n->whilestmt.cond);
            }
            return (Value){.type=2};
        }
        case NODE_BLOCK: {
            for (int i=0;i<n->block.count;i++) eval(n->block.stmts[i]);
            return (Value){.type=2};
        }
        default: return (Value){.type=2};
    }
    return (Value){.type=2};
}

void free_ast(ASTNode* n) {
    if (!n) return;
    // recursively free children
    // (simplified)
    free(n);
}

// ---------- Main ----------
int main(int argc, char** argv) {
    if (argc<2) { printf("Usage: ./riven file.rv\n"); return 1; }
    FILE* f = fopen(argv[1], "r");
    if (!f) { perror("File open failed"); return 1; }
    fseek(f,0,SEEK_END);
    long len = ftell(f);
    fseek(f,0,SEEK_SET);
    char* src = malloc(len+1);
    fread(src,1,len,f);
    src[len]=0;
    fclose(f);
    
    init_lexer(src);
    if (setjmp(error_jmp)==0) {
        ASTNode* prog = parse_program();
        eval(prog);
        free_ast(prog);
    } else {
        fprintf(stderr,"Runtime error\n");
    }
    free(src);
    return 0;
}
