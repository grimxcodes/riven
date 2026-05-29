#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "ast.h"

static Token current_token;
static Token next_token;

static ASTNode* global_nodes[1000];
static int global_count = 0;

static void advance_parser() {

    current_token = next_token;

    next_token = get_next_token();

}

static void eat(TokenType type) {

    if (
        current_token.type == type
    ) {

        advance_parser();

    }

    else {

        printf(
            "Unexpected token: %s\n",
            current_token.value
        );

        exit(1);

    }

}

static ASTNode* parse_expression();

static ASTNode* parse_primary() {

    if (
        current_token.type ==
        TOKEN_NUMBER
    ) {

        ASTNode* node =
            create_node(
                NODE_NUMBER
            );

        node->value =
            strdup(current_token.value);

        eat(TOKEN_NUMBER);

        return node;

    }

    if (
        current_token.type ==
        TOKEN_STRING
    ) {

        ASTNode* node =
            create_node(
                NODE_STRING
            );

        node->value =
            strdup(current_token.value);

        eat(TOKEN_STRING);

        return node;

    }

    if (
        current_token.type ==
        TOKEN_CORRECT
    ) {

        ASTNode* node =
            create_node(
                NODE_BOOLEAN
            );

        node->value =
            strdup("1");

        eat(TOKEN_CORRECT);

        return node;

    }

    if (
        current_token.type ==
        TOKEN_INCORRECT
    ) {

        ASTNode* node =
            create_node(
                NODE_BOOLEAN
            );

        node->value =
            strdup("0");

        eat(TOKEN_INCORRECT);

        return node;

    }

    if (
        current_token.type ==
        TOKEN_EMP
    ) {

        ASTNode* node =
            create_node(
                NODE_NULL
            );

        node->value =
            strdup("0");

        eat(TOKEN_EMP);

        return node;

    }

    if (
        current_token.type ==
        TOKEN_GRAB
    ) {

        ASTNode* node =
            create_node(
                NODE_INPUT
            );

        eat(TOKEN_GRAB);

        eat(TOKEN_LPAREN);
        eat(TOKEN_RPAREN);

        return node;

    }

    if (
        current_token.type ==
        TOKEN_SPAWN
    ) {

        eat(TOKEN_SPAWN);

        ASTNode* node =
            create_node(
                NODE_OBJECT
            );

        node->name =
            strdup(current_token.value);

        eat(TOKEN_IDENTIFIER);

        eat(TOKEN_LPAREN);
        eat(TOKEN_RPAREN);

        return node;

    }

    if (
        current_token.type ==
        TOKEN_IDENTIFIER
    ) {

        if (
            next_token.type ==
            TOKEN_INCREMENT
        ) {

            ASTNode* node =
                create_node(
                    NODE_INCREMENT
                );

            node->name =
                strdup(current_token.value);

            eat(TOKEN_IDENTIFIER);
            eat(TOKEN_INCREMENT);

            return node;

        }

        if (
            next_token.type ==
            TOKEN_DECREMENT
        ) {

            ASTNode* node =
                create_node(
                    NODE_DECREMENT
                );

            node->name =
                strdup(current_token.value);

            eat(TOKEN_IDENTIFIER);
            eat(TOKEN_DECREMENT);

            return node;

        }

        if (
            next_token.type ==
            TOKEN_LBRACKET
        ) {

            ASTNode* node =
                create_node(
                    NODE_INDEX
                );

            node->name =
                strdup(current_token.value);

            eat(TOKEN_IDENTIFIER);

            eat(TOKEN_LBRACKET);

            node->left =
                parse_expression();

            eat(TOKEN_RBRACKET);

            return node;

        }

        if (
            next_token.type ==
            TOKEN_LPAREN
        ) {

            ASTNode* node =
                create_node(
                    NODE_CALL
                );

            node->name =
                strdup(current_token.value);

            eat(TOKEN_IDENTIFIER);

            eat(TOKEN_LPAREN);

            if (
                current_token.type !=
                TOKEN_RPAREN
            ) {

                node->left =
                    parse_expression();

                if (
                    current_token.type ==
                    TOKEN_COMMA
                ) {

                    eat(TOKEN_COMMA);

                    node->right =
                        parse_expression();

                }

            }

            eat(TOKEN_RPAREN);

            return node;

        }

        ASTNode* node =
            create_node(
                NODE_VARIABLE
            );

        node->name =
            strdup(current_token.value);

        eat(TOKEN_IDENTIFIER);

        return node;

    }

    if (
        current_token.type ==
        TOKEN_LBRACKET
    ) {

        ASTNode* node =
            create_array_node();

        eat(TOKEN_LBRACKET);

        while (

            current_token.type !=
            TOKEN_RBRACKET

        ) {

            node->children[
                node->child_count++
            ] = parse_expression();

            if (
                current_token.type ==
                TOKEN_COMMA
            ) {

                eat(TOKEN_COMMA);

            }

        }

        eat(TOKEN_RBRACKET);

        return node;

    }

    printf(
        "Invalid expression\n"
    );

    exit(1);

}

static ASTNode* parse_expression() {

    ASTNode* left =
        parse_primary();

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
        TOKEN_AND ||

        current_token.type ==
        TOKEN_OR ||

        current_token.type ==
        TOKEN_AND_SYMBOL ||

        current_token.type ==
        TOKEN_OR_SYMBOL

    ) {

        ASTNode* op =
            create_node(
                NODE_BINARY
            );

        op->value =
            strdup(current_token.value);

        TokenType type =
            current_token.type;

        eat(type);

        op->left = left;

        op->right =
            parse_primary();

        left = op;

    }

    return left;

}

static ASTNode* parse_statement();

static ASTNode* parse_block() {

    ASTNode* block =
        create_node(
            NODE_BLOCK
        );

    block->children =
        malloc(
            sizeof(ASTNode*) * 1000
        );

    eat(TOKEN_LBRACE);

    while (

        current_token.type !=
        TOKEN_RBRACE

    ) {

        block->children[
            block->child_count++
        ] = parse_statement();

    }

    eat(TOKEN_RBRACE);

    return block;

}

static ASTNode* parse_assignment() {

    ASTNode* node =
        create_node(
            NODE_ASSIGNMENT
        );

    node->name =
        strdup(current_token.value);

    eat(TOKEN_IDENTIFIER);

    eat(TOKEN_ASSIGN);

    node->right =
        parse_expression();

    return node;

}

static ASTNode* parse_firm() {

    ASTNode* node =
        create_node(
            NODE_CONSTANT_ASSIGNMENT
        );

    eat(TOKEN_FIRM);

    node->name =
        strdup(current_token.value);

    eat(TOKEN_IDENTIFIER);

    eat(TOKEN_ASSIGN);

    node->right =
        parse_expression();

    return node;

}

static ASTNode* parse_stamp() {

    ASTNode* node =
        create_node(
            NODE_STAMP
        );

    eat(TOKEN_STAMP);

    eat(TOKEN_LPAREN);

    node->left =
        parse_expression();

    eat(TOKEN_RPAREN);

    return node;

}

static ASTNode* parse_if() {

    ASTNode* node =
        create_node(
            NODE_IF
        );

    eat(TOKEN_IF);

    node->left =
        parse_expression();

    node->right =
        parse_block();

    ASTNode* current =
        node;

    while (

        current_token.type ==
        TOKEN_ALTIF

    ) {

        ASTNode* altif =
            create_node(
                NODE_IF
            );

        eat(TOKEN_ALTIF);

        altif->left =
            parse_expression();

        altif->right =
            parse_block();

        current->third =
            altif;

        current = altif;

    }

    if (
        current_token.type ==
        TOKEN_ELSE
    ) {

        eat(TOKEN_ELSE);

        current->third =
            parse_block();

    }

    return node;

}

static ASTNode* parse_flow() {

    ASTNode* node =
        create_node(
            NODE_FLOW
        );

    eat(TOKEN_FLOW);

    node->left =
        parse_expression();

    node->right =
        parse_block();

    return node;

}

static ASTNode* parse_during() {

    ASTNode* node =
        create_node(
            NODE_WHILE
        );

    eat(TOKEN_DURING);

    node->left =
        parse_expression();

    node->right =
        parse_block();

    return node;

}

static ASTNode* parse_function() {

    ASTNode* node =
        create_node(
            NODE_FUNCTION
        );

    eat(TOKEN_CRAFT);

    node->name =
        strdup(current_token.value);

    eat(TOKEN_IDENTIFIER);

    eat(TOKEN_LPAREN);

    if (
        current_token.type ==
        TOKEN_IDENTIFIER
    ) {

        node->param_name =
            strdup(current_token.value);

        eat(TOKEN_IDENTIFIER);

        if (
            current_token.type ==
            TOKEN_COMMA
        ) {

            eat(TOKEN_COMMA);

            node->param2_name =
                strdup(current_token.value);

            eat(TOKEN_IDENTIFIER);

        }

    }

    eat(TOKEN_RPAREN);

    node->right =
        parse_block();

    return node;

}

static ASTNode* parse_returns() {

    ASTNode* node =
        create_node(
            NODE_RETURN
        );

    eat(TOKEN_RETURNS);

    node->left =
        parse_expression();

    return node;

}

static ASTNode* parse_rise() {

    ASTNode* node =
        create_node(
            NODE_INCREMENT
        );

    eat(TOKEN_RISE);

    node->name =
        strdup(current_token.value);

    eat(TOKEN_IDENTIFIER);

    return node;

}

static ASTNode* parse_drop() {

    ASTNode* node =
        create_node(
            NODE_DECREMENT
        );

    eat(TOKEN_DROP);

    node->name =
        strdup(current_token.value);

    eat(TOKEN_IDENTIFIER);

    return node;

}

static ASTNode* parse_frame() {

    ASTNode* node =
        create_node(
            NODE_FRAME
        );

    eat(TOKEN_FRAME);

    node->name =
        strdup(current_token.value);

    eat(TOKEN_IDENTIFIER);

    eat(TOKEN_LBRACE);

    while (

        current_token.type !=
        TOKEN_RBRACE

    ) {

        if (
            current_token.type ==
            TOKEN_HIDDEN
        ) {

            eat(TOKEN_HIDDEN);

            eat(TOKEN_IDENTIFIER);

            eat(TOKEN_ASSIGN);

            parse_expression();

        }

        else if (
            current_token.type ==
            TOKEN_OPEN
        ) {

            eat(TOKEN_OPEN);

            parse_function();

        }

        else if (
            current_token.type ==
            TOKEN_BOOT
        ) {

            eat(TOKEN_BOOT);

            eat(TOKEN_LPAREN);
            eat(TOKEN_RPAREN);

            parse_block();

        }

        else {

            printf(
                "Invalid frame syntax\n"
            );

            exit(1);

        }

    }

    eat(TOKEN_RBRACE);

    return node;

}

static ASTNode* parse_statement() {

    if (
        current_token.type ==
        TOKEN_STAMP
    ) {

        return parse_stamp();

    }

    if (
        current_token.type ==
        TOKEN_FIRM
    ) {

        return parse_firm();

    }

    if (
        current_token.type ==
        TOKEN_FRAME
    ) {

        return parse_frame();

    }

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

        return parse_during();

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

        return parse_returns();

    }

    if (
        current_token.type ==
        TOKEN_RISE
    ) {

        return parse_rise();

    }

    if (
        current_token.type ==
        TOKEN_DROP
    ) {

        return parse_drop();

    }

    if (
        current_token.type ==
        TOKEN_IDENTIFIER
    ) {

        if (
            next_token.type ==
            TOKEN_ASSIGN
        ) {

            return parse_assignment();

        }

        return parse_expression();

    }

    printf(
        "Unknown statement\n"
    );

    exit(1);

}

ASTNode* parse_program() {

    current_token =
        get_next_token();

    next_token =
        get_next_token();

    while (

        current_token.type ==
        TOKEN_CONSISTOF

    ) {

        eat(TOKEN_CONSISTOF);

        eat(TOKEN_STRING);

    }

    while (

        current_token.type ==
        TOKEN_FIRM ||

        current_token.type ==
        TOKEN_CRAFT ||

        current_token.type ==
        TOKEN_FRAME

    ) {

        global_nodes[
            global_count++
        ] = parse_statement();

    }

    eat(TOKEN_RIVEN);

    eat(TOKEN_CORE);

    ASTNode* root =
        parse_block();

    for (
        int i = global_count - 1;
        i >= 0;
        i--
    ) {

        for (
            int j = root->child_count;
            j > 0;
            j--
        ) {

            root->children[j] =
                root->children[j - 1];

        }

        root->children[0] =
            global_nodes[i];

        root->child_count++;

    }

    return root;

}
