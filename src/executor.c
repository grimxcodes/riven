#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "executor.h"
#include "runtime.h"

typedef struct {

    char name[100];

    char param1[100];

    char param2[100];

    ASTNode* body;

} Function;

static Function functions[100];

static int function_count = 0;

static int return_value = 0;

static int eval(ASTNode* node);

static int eval_binary(ASTNode* node) {

    int left =
        eval(node->left);

    int right =
        eval(node->right);

    if (
        strcmp(node->value, "+") == 0
    ) {

        return left + right;

    }

    if (
        strcmp(node->value, "-") == 0
    ) {

        return left - right;

    }

    if (
        strcmp(node->value, "<") == 0
    ) {

        return left < right;

    }

    if (
        strcmp(node->value, ">") == 0
    ) {

        return left > right;

    }

    if (
        strcmp(node->value, "==") == 0
    ) {

        return left == right;

    }

    return 0;

}

static int eval(ASTNode* node) {

    if (node == NULL)
        return 0;

    switch (node->type) {

        case NODE_NUMBER:

            return atoi(node->value);

        case NODE_VARIABLE: {

            char* value =
                runtime_get_variable(
                    node->name
                );

            return atoi(value);

        }

        case NODE_INDEX: {

            int index =
                eval(node->left);

            char* value =
                runtime_get_array(
                    node->name,
                    index
                );

            return atoi(value);

        }

        case NODE_INPUT: {

            char* text =
                runtime_input();

            return atoi(text);

        }

        case NODE_BINARY:

            return eval_binary(node);

        case NODE_CALL: {

            for (
                int i = 0;
                i < function_count;
                i++
            ) {

                if (
                    strcmp(
                        functions[i].name,
                        node->name
                    ) == 0
                ) {

                    if (node->left) {

                        int value1 =
                            eval(node->left);

                        char buffer1[100];

                        sprintf(
                            buffer1,
                            "%d",
                            value1
                        );

                        runtime_set_variable(

                            functions[i].param1,

                            buffer1
                        );

                    }

                    if (node->right) {

                        int value2 =
                            eval(node->right);

                        char buffer2[100];

                        sprintf(
                            buffer2,
                            "%d",
                            value2
                        );

                        runtime_set_variable(

                            functions[i].param2,

                            buffer2
                        );

                    }

                    execute(
                        functions[i].body
                    );

                    return return_value;

                }

            }

            return 0;

        }

        default:
            return 0;

    }

}

void execute(ASTNode* node) {

    if (node == NULL)
        return;

    switch (node->type) {

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

        case NODE_IMPORT: {

            printf(
                "[imported %s]\n",
                node->value
            );

            break;

        }

        case NODE_ASSIGNMENT: {

            if (
                node->right->type ==
                NODE_ARRAY
            ) {

                for (
                    int i = 0;
                    i < node->right->child_count;
                    i++
                ) {

                    ASTNode* item =
                        node->right->children[i];

                    char buffer[100];

                    if (
                        item->type ==
                        NODE_STRING
                    ) {

                        runtime_set_array(
                            node->name,
                            i,
                            item->value
                        );

                    }

                    else {

                        int value =
                            eval(item);

                        sprintf(
                            buffer,
                            "%d",
                            value
                        );

                        runtime_set_array(
                            node->name,
                            i,
                            buffer
                        );

                    }

                }

            }

            else if (
                node->right->type ==
                NODE_STRING
            ) {

                runtime_set_variable(
                    node->name,
                    node->right->value
                );

            }

            else {

                int result =
                    eval(node->right);

                char buffer[100];

                sprintf(
                    buffer,
                    "%d",
                    result
                );

                runtime_set_variable(
                    node->name,
                    buffer
                );

            }

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

            else if (
                node->left->type ==
                NODE_VARIABLE
            ) {

                runtime_stamp(

                    runtime_get_variable(
                        node->left->name
                    )

                );

            }

            else if (
                node->left->type ==
                NODE_INDEX
            ) {

                int index =
                    eval(node->left->left);

                runtime_stamp(

                    runtime_get_array(
                        node->left->name,
                        index
                    )

                );

            }

            else {

                int result =
                    eval(node->left);

                char buffer[100];

                sprintf(
                    buffer,
                    "%d",
                    result
                );

                runtime_stamp(
                    buffer
                );

            }

            break;

        }

        case NODE_IF: {

            int condition =
                eval(node->left);

            if (condition) {

                execute(
                    node->right
                );

            }

            else if (
                node->third
            ) {

                execute(
                    node->third
                );

            }

            break;

        }

        case NODE_FLOW: {

            while (
                eval(node->left)
            ) {

                execute(
                    node->right
                );

            }

            break;

        }

        case NODE_FUNCTION: {

            strcpy(

                functions[
                    function_count
                ].name,

                node->name

            );

            if (
                node->param_name
            ) {

                strcpy(

                    functions[
                        function_count
                    ].param1,

                    node->param_name

                );

            }

            if (
                node->param2_name
            ) {

                strcpy(

                    functions[
                        function_count
                    ].param2,

                    node->param2_name

                );

            }

            functions[
                function_count
            ].body = node->right;

            function_count++;

            break;

        }

        case NODE_CALL: {

            eval(node);

            break;

        }

        case NODE_RETURN: {

            return_value =
                eval(node->left);

            break;

        }

        default:
            break;

    }

}
