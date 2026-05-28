#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "ast.h"

static Token current_token;
static Token next_token;

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

static ASTNode* parse_array() {

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
        TOKEN_LBRACKET
    ) {

        return parse_array();

    }

    if (
        current_token.type ==
        TOKEN_INPUT
    ) {

        ASTNode* node =
            create_node(
                NODE_INPUT
            );

        eat(TOKEN_INPUT);

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
        TOKEN_EQUAL_EQUAL

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
            sizeof(ASTNode*) * 100
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

    if (
        current_token.type ==
        TOKEN_ELSE
    ) {

        eat(TOKEN_ELSE);

        node->third =
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

    eat(TOKEN_RPAREN);

    node->right =
        parse_block();

    return node;

}

static ASTNode* parse_return() {

    ASTNode* node =
        create_node(
            NODE_RETURN
        );

    eat(TOKEN_RETURN);

    node->left =
        parse_expression();

    return node;

}

static ASTNode* parse_statement() {

    if (
        current_token.type ==
        TOKEN_CRAFT
    ) {

        return parse_function();

    }

    if (
        current_token.type ==
        TOKEN_RETURN
    ) {

        return parse_return();

    }

    if (
        current_token.type ==
        TOKEN_STAMP
    ) {

        return parse_stamp();

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

    eat(TOKEN_RIVEN);

    eat(TOKEN_CORE);

    return parse_block();

}
