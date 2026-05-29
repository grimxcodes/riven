#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"
#include "token.h"

static Token current_token;

static void advance() {

    current_token =
        get_next_token();

}

static void error(
    const char* msg
) {

    printf(
        "%s\n",
        msg
    );

    exit(1);

}

static void expect(
    TokenType type
) {

    if (
        current_token.type != type
    ) {

        printf(
            "Unexpected token: %s\n",
            current_token.value
        );

        exit(1);

    }

    advance();

}

static ASTNode* parse_expression();

static ASTNode* parse_block();

static ASTNode* parse_statement();

static ASTNode* parse_factor() {

    ASTNode* node;

    if (
        current_token.type ==
        TOKEN_NUMBER
    ) {

        node =
            create_node(
                NODE_NUMBER
            );

        node->value =
            strdup(
                current_token.value
            );

        advance();

        return node;

    }

    if (
        current_token.type ==
        TOKEN_STRING
    ) {

        node =
            create_node(
                NODE_STRING
            );

        node->value =
            strdup(
                current_token.value
            );

        advance();

        return node;

    }

    if (

        current_token.type ==
        TOKEN_CORRECT ||

        current_token.type ==
        TOKEN_INCORRECT

    ) {

        node =
            create_node(
                NODE_BOOLEAN
            );

        if (
            current_token.type ==
            TOKEN_CORRECT
        ) {

            node->value =
                strdup("1");

        }

        else {

            node->value =
                strdup("0");

        }

        advance();

        return node;

    }

    if (
        current_token.type ==
        TOKEN_EMP
    ) {

        node =
            create_node(
                NODE_NULL
            );

        advance();

        return node;

    }

    if (
        current_token.type ==
        TOKEN_GRAB
    ) {

        advance();

        expect(
            TOKEN_LPAREN
        );

        if (
            current_token.type ==
            TOKEN_STRING
        ) {

            advance();

        }

        expect(
            TOKEN_RPAREN
        );

        node =
            create_node(
                NODE_INPUT
            );

        return node;

    }

    if (
        current_token.type ==
        TOKEN_IDENTIFIER
    ) {

        char name[256];

        strcpy(
            name,
            current_token.value
        );

        advance();

        if (
            current_token.type ==
            TOKEN_LPAREN
        ) {

            advance();

            ASTNode* arg1 = NULL;
            ASTNode* arg2 = NULL;

            if (
                current_token.type !=
                TOKEN_RPAREN
            ) {

                arg1 =
                    parse_expression();

                if (
                    current_token.type ==
                    TOKEN_COMMA
                ) {

                    advance();

                    arg2 =
                        parse_expression();

                }

            }

            expect(
                TOKEN_RPAREN
            );

            node =
                create_node(
                    NODE_CALL
                );

            node->name =
                strdup(name);

            node->left = arg1;

            node->right = arg2;

            return node;

        }

        if (
            current_token.type ==
            TOKEN_LBRACKET
        ) {

            advance();

            ASTNode* index =
                parse_expression();

            expect(
                TOKEN_RBRACKET
            );

            node =
                create_node(
                    NODE_INDEX
                );

            node->name =
                strdup(name);

            node->left = index;

            return node;

        }

        node =
            create_node(
                NODE_VARIABLE
            );

        node->name =
            strdup(name);

        return node;

    }

    if (
        current_token.type ==
        TOKEN_LBRACKET
    ) {

        advance();

        node =
            create_array_node();

        while (

            current_token.type !=
            TOKEN_RBRACKET

        ) {

            ASTNode* item =
                parse_expression();

            node->children[
                node->child_count++
            ] = item;

            if (
                current_token.type ==
                TOKEN_COMMA
            ) {

                advance();

            }

        }

        expect(
            TOKEN_RBRACKET
        );

        return node;

    }

    error(
        "Invalid expression"
    );

    return NULL;

}

static ASTNode* parse_term() {

    ASTNode* node =
        parse_factor();

    while (

        current_token.type ==
        TOKEN_MULTIPLY ||

        current_token.type ==
        TOKEN_DIVIDE

    ) {

        Token op =
            current_token;

        advance();

        ASTNode* right =
            parse_factor();

        ASTNode* binary =
            create_node(
                NODE_BINARY
            );

        binary->value =
            strdup(op.value);

        binary->left = node;

        binary->right = right;

        node = binary;

    }

    return node;

}

static ASTNode* parse_expression() {

    ASTNode* node =
        parse_term();

    while (

        current_token.type ==
        TOKEN_PLUS ||

        current_token.type ==
        TOKEN_MINUS ||

        current_token.type ==
        TOKEN_LESS ||

        current_token.type ==
        TOKEN_GREATER ||

        current_token.type ==
        TOKEN_EQUAL_EQUAL ||

        current_token.type ==
        TOKEN_NOT_EQUAL ||

        current_token.type ==
        TOKEN_AND ||

        current_token.type ==
        TOKEN_OR ||

        current_token.type ==
        TOKEN_AND_SYMBOL ||

        current_token.type ==
        TOKEN_OR_SYMBOL

    ) {

        Token op =
            current_token;

        advance();

        ASTNode* right =
            parse_term();

        ASTNode* binary =
            create_node(
                NODE_BINARY
            );

        binary->value =
            strdup(op.value);

        binary->left = node;

        binary->right = right;

        node = binary;

    }

    return node;

}

static ASTNode* parse_if() {

    advance();

    ASTNode* condition =
        parse_expression();

    ASTNode* body =
        parse_block();

    ASTNode* node =
        create_node(
            NODE_IF
        );

    node->left = condition;

    node->right = body;

    if (
        current_token.type ==
        TOKEN_ALTIF
    ) {

        node->third =
            parse_if();

    }

    else if (
        current_token.type ==
        TOKEN_ELSE
    ) {

        advance();

        node->third =
            parse_block();

    }

    return node;

}

static ASTNode* parse_flow() {

    advance();

    ASTNode* count =
        parse_expression();

    ASTNode* body =
        parse_block();

    ASTNode* node =
        create_node(
            NODE_FLOW
        );

    node->left = count;

    node->right = body;

    return node;

}

static ASTNode* parse_while() {

    advance();

    ASTNode* condition =
        parse_expression();

    ASTNode* body =
        parse_block();

    ASTNode* node =
        create_node(
            NODE_WHILE
        );

    node->left = condition;

    node->right = body;

    return node;

}

static ASTNode* parse_function() {

    advance();

    ASTNode* node =
        create_node(
            NODE_FUNCTION
        );

    node->name =
        strdup(
            current_token.value
        );

    expect(
        TOKEN_IDENTIFIER
    );

    expect(
        TOKEN_LPAREN
    );

    if (
        current_token.type ==
        TOKEN_IDENTIFIER
    ) {

        node->param_name =
            strdup(
                current_token.value
            );

        advance();

        if (
            current_token.type ==
            TOKEN_COMMA
        ) {

            advance();

            node->param2_name =
                strdup(
                    current_token.value
                );

            expect(
                TOKEN_IDENTIFIER
            );

        }

    }

    expect(
        TOKEN_RPAREN
    );

    node->right =
        parse_block();

    return node;

}

static ASTNode* parse_return() {

    advance();

    ASTNode* node =
        create_node(
            NODE_RETURN
        );

    node->left =
        parse_expression();

    return node;

}

static ASTNode* parse_frame() {

    advance();

    ASTNode* node =
        create_node(
            NODE_FRAME
        );

    node->name =
        strdup(
            current_token.value
        );

    expect(
        TOKEN_IDENTIFIER
    );

    expect(
        TOKEN_LBRACE
    );

    int brace = 1;

    while (

        brace > 0 &&

        current_token.type !=
        TOKEN_EOF

    ) {

        if (
            current_token.type ==
            TOKEN_LBRACE
        ) {

            brace++;

        }

        else if (
            current_token.type ==
            TOKEN_RBRACE
        ) {

            brace--;

        }

        advance();

    }

    return node;

}

static ASTNode* parse_statement() {

    if (
        current_token.type ==
        TOKEN_IF
    ) {

        return parse_if();

    }

    if (
        current_token.type ==
        TOKEN_FLOW
    ) {

        return parse_flow();

    }

    if (
        current_token.type ==
        TOKEN_DURING
    ) {

        return parse_while();

    }

    if (
        current_token.type ==
        TOKEN_CRAFT
    ) {

        return parse_function();

    }

    if (
        current_token.type ==
        TOKEN_RETURNS
    ) {

        return parse_return();

    }

    if (
        current_token.type ==
        TOKEN_FRAME
    ) {

        return parse_frame();

    }

    if (
        current_token.type ==
        TOKEN_STAMP
    ) {

        advance();

        expect(
            TOKEN_LPAREN
        );

        ASTNode* expr =
            parse_expression();

        expect(
            TOKEN_RPAREN
        );

        ASTNode* node =
            create_node(
                NODE_STAMP
            );

        node->left = expr;

        return node;

    }

    if (
        current_token.type ==
        TOKEN_CONSISTOF
    ) {

        advance();

        if (
            current_token.type ==
            TOKEN_STRING
        ) {

            advance();

        }

        return NULL;

    }

    if (
        current_token.type ==
        TOKEN_FIRM
    ) {

        advance();

        ASTNode* node =
            create_node(
                NODE_CONSTANT_ASSIGNMENT
            );

        node->name =
            strdup(
                current_token.value
            );

        expect(
            TOKEN_IDENTIFIER
        );

        expect(
            TOKEN_ASSIGN
        );

        node->right =
            parse_expression();

        return node;

    }

    if (
        current_token.type ==
        TOKEN_RISE
    ) {

        advance();

        ASTNode* node =
            create_node(
                NODE_INCREMENT
            );

        node->name =
            strdup(
                current_token.value
            );

        expect(
            TOKEN_IDENTIFIER
        );

        return node;

    }

    if (
        current_token.type ==
        TOKEN_DROP
    ) {

        advance();

        ASTNode* node =
            create_node(
                NODE_DECREMENT
            );

        node->name =
            strdup(
                current_token.value
            );

        expect(
            TOKEN_IDENTIFIER
        );

        return node;

    }

    if (
        current_token.type ==
        TOKEN_IDENTIFIER
    ) {

        char name[256];

        strcpy(
            name,
            current_token.value
        );

        advance();

        if (
            current_token.type ==
            TOKEN_INCREMENT
        ) {

            advance();

            ASTNode* node =
                create_node(
                    NODE_INCREMENT
                );

            node->name =
                strdup(name);

            return node;

        }

        if (
            current_token.type ==
            TOKEN_DECREMENT
        ) {

            advance();

            ASTNode* node =
                create_node(
                    NODE_DECREMENT
                );

            node->name =
                strdup(name);

            return node;

        }

        if (
            current_token.type ==
            TOKEN_ASSIGN
        ) {

            advance();

            ASTNode* node =
                create_node(
                    NODE_ASSIGNMENT
                );

            node->name =
                strdup(name);

            if (
                current_token.type ==
                TOKEN_SPAWN
            ) {

                advance();

                ASTNode* obj =
                    create_node(
                        NODE_OBJECT
                    );

                obj->name =
                    strdup(
                        current_token.value
                    );

                expect(
                    TOKEN_IDENTIFIER
                );

                expect(
                    TOKEN_LPAREN
                );

                expect(
                    TOKEN_RPAREN
                );

                node->right = obj;

                return node;

            }

            node->right =
                parse_expression();

            return node;

        }

        if (
            current_token.type ==
            TOKEN_LPAREN
        ) {

            advance();

            ASTNode* arg1 = NULL;
            ASTNode* arg2 = NULL;

            if (
                current_token.type !=
                TOKEN_RPAREN
            ) {

                arg1 =
                    parse_expression();

                if (
                    current_token.type ==
                    TOKEN_COMMA
                ) {

                    advance();

                    arg2 =
                        parse_expression();

                }

            }

            expect(
                TOKEN_RPAREN
            );

            ASTNode* node =
                create_node(
                    NODE_CALL
                );

            node->name =
                strdup(name);

            node->left = arg1;

            node->right = arg2;

            return node;

        }

    }

    error(
        "Unknown statement"
    );

    return NULL;

}

static ASTNode* parse_block() {

    expect(
        TOKEN_LBRACE
    );

    ASTNode* block =
        create_node(
            NODE_BLOCK
        );

    block->children =
        malloc(
            sizeof(ASTNode*) * 1000
        );

    block->child_count = 0;

    while (

        current_token.type !=
        TOKEN_RBRACE

    ) {

        ASTNode* stmt =
            parse_statement();

        if (stmt) {

            block->children[
                block->child_count++
            ] = stmt;

        }

    }

    expect(
        TOKEN_RBRACE
    );

    return block;

}

ASTNode* parse_program() {

    advance();

    ASTNode* program =
        create_node(
            NODE_BLOCK
        );

    program->children =
        malloc(
            sizeof(ASTNode*) * 2000
        );

    program->child_count = 0;

    while (
        current_token.type !=
        TOKEN_RIVEN
    ) {

        ASTNode* stmt =
            parse_statement();

        if (stmt) {

            program->children[
                program->child_count++
            ] = stmt;

        }

    }

    advance();

    expect(
        TOKEN_CORE
    );

    ASTNode* core =
        parse_block();

    for (
        int i = 0;
        i < core->child_count;
        i++
    ) {

        program->children[
            program->child_count++
        ] = core->children[i];

    }

    return program;

}
