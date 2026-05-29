#ifndef AST_H
#define AST_H

typedef enum {

    NODE_BLOCK,

    NODE_NUMBER,
    NODE_STRING,

    NODE_VARIABLE,
    NODE_ASSIGNMENT,

    NODE_BINARY,

    NODE_STAMP,
    NODE_INPUT,

    NODE_IF,
    NODE_FLOW,
    NODE_WHILE,

    NODE_FUNCTION,
    NODE_CALL,
    NODE_RETURN,

    NODE_ARRAY,
    NODE_INDEX,

    NODE_INCREMENT,
    NODE_DECREMENT,

    NODE_IMPORT

} NodeType;

typedef struct ASTNode {

    NodeType type;

    char* value;

    char* name;

    char* param_name;

    char* param2_name;

    struct ASTNode* left;

    struct ASTNode* right;

    struct ASTNode* third;

    struct ASTNode** children;

    int child_count;

} ASTNode;

ASTNode* create_node(
    NodeType type
);

ASTNode* create_array_node();

#endif
