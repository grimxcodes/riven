#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "executor.h"
#include "runtime.h"

static int eval(ASTNode* node) {

    if (node->type == NODE_NUMBER) {

        return atoi(node->value);

    }

    if (node->type == NODE_VARIABLE) {

        return atoi(
            runtime_get_variable(node->name)
        );

    }

    if (node->type == NODE_BINARY) {

        int left = eval(node->left);

        int right = eval(node->right);

        if (strcmp(node->value, "+") == 0)
            return left + right;

        if (strcmp(node->value, "-") == 0)
            return left - right;

        if (strcmp(node->value, "<") == 0)
            return left < right;

        if (strcmp(node->value, ">") == 0)
            return left > right;

        if (strcmp(node->value, "==") == 0)
            return left == right;

    }

    return 0;

}

void execute(ASTNode* node) {

    if (node == NULL)
        return;

    switch (node->type) {

        case NODE_ASSIGNMENT: {

            int result = eval(node->right);

            char buffer[100];

            sprintf(buffer, "%d", result);

            runtime_set_variable(
                node->name,
                buffer
            );

            break;

        }

        case NODE_STAMP: {

            if (
                node->left->type ==
                NODE_STRING
            ) {

                runtime_stamp(
                    node->left->value
                );

            }

            else {

                int result = eval(node->left);

                char buffer[100];

                sprintf(buffer, "%d", result);

                runtime_stamp(buffer);

            }

            break;

        }

        case NODE_BLOCK: {

            for (
                int i = 0;
                i < node->child_count;
                i++
            ) {

                execute(
                    node->children[i]
                );

            }

            break;

        }

        case NODE_IF: {

            int condition =
                eval(node->left);

            if (condition) {

                execute(node->right);

            }

            break;

        }

        case NODE_FLOW: {

            while (
                eval(node->left)
            ) {

                execute(node->right);

            }

            break;

        }

        default:
            break;

    }

}
