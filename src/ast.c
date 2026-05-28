#include <stdlib.h>
#include <string.h>

#include "ast.h"

ASTNode* create_node(NodeType type) {

    ASTNode* node = malloc(sizeof(ASTNode));

    node->type = type;

    node->value = NULL;
    node->name = NULL;

    node->left = NULL;
    node->right = NULL;

    node->children = NULL;

    node->child_count = 0;

    return node;

}
