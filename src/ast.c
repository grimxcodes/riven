#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"

ASTNode* create_node(
    NodeType type
) {

    ASTNode* node =
        malloc(
            sizeof(ASTNode)
        );

    node->type = type;

    node->value = NULL;

    node->name = NULL;

    node->extra = NULL;

    node->param_name = NULL;

    node->param2_name = NULL;

    node->left = NULL;

    node->right = NULL;

    node->third = NULL;

    node->children = NULL;

    node->child_count = 0;

    return node;

}

ASTNode* create_array_node() {

    ASTNode* node =
        create_node(
            NODE_ARRAY
        );

    node->children =
        malloc(
            sizeof(ASTNode*) * 1000
        );

    node->child_count = 0;

    return node;

}
