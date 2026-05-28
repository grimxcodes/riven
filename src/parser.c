#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"
#include "token.h"
#include "ast.h"

static Token current_token;

static void eat(TokenType type) {

    if (
        current_token.type ==
        type
    ) {

        current_token =
            get_next_token();

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

static ASTNode* parse_number() {

    ASTNode* node =
        create_node(
            NODE_NUMBER
        );

    node->value =
        strdup(current_token.value);

    eat(TOKEN_NUMBER);

    return node;

}

static ASTNode* parse_variable() {

    ASTNode* node =
        create_node(
            NODE_VARIABLE
        );

    node->name =
        strdup(current_token.value);

    eat(TOKEN_IDENTIFIER);

    return node;

}

static ASTNode* parse_expression() {

    ASTNode* left;

    if (
        current_token.type ==
        TOKEN_NUMBER
    ) {

        left =
            parse_number();

    }

    else {

        left =
            parse_variable();

    }

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
            strdup(
                current_token.value
            );

        TokenType type =
            current_token.type;

        eat(type);

        ASTNode* right;

        if (
            current_token.type ==
            TOKEN_NUMBER
        ) {

            right =
                parse_number();

        }

        else {

            right =
                parse_variable();

        }

        op->left = left;
        op->right = right;

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

    if (
        current_token.type ==
        TOKEN_STRING
    ) {

        ASTNode* str =
            create_node(
                NODE_STRING
            );

        str->value =
            strdup(current_token.value);

        node->left = str;

        eat(TOKEN_STRING);

    }

    else {

        node->left =
            parse_expression();

    }

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

    eat(TOKEN_RPAREN);

    node->right =
        parse_block();

    return node;

}

static ASTNode* parse_call() {

    ASTNode* node =
        create_node(
            NODE_CALL
        );

    node->name =
        strdup(current_token.value);

    eat(TOKEN_IDENTIFIER);

    eat(TOKEN_LPAREN);

    if (
        current_token.type ==
        TOKEN_STRING
    ) {

        ASTNode* arg =
            create_node(
                NODE_STRING
            );

        arg->value =
            strdup(current_token.value);

        node->left = arg;

        eat(TOKEN_STRING);

    }

    eat(TOKEN_RPAREN);

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

        Token next =
            peek_next_token();

        if (
            next.type ==
            TOKEN_LPAREN
        ) {

            return parse_call();

        }

        return parse_assignment();

    }

    printf(
        "Unknown statement\n"
    );

    exit(1);

}

ASTNode* parse_program() {

    current_token =
        get_next_token();

    eat(TOKEN_RIVEN);

    eat(TOKEN_CORE);

    return parse_block();

}
