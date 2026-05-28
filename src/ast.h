#ifndef AST_H
#define AST_H

typedef enum {

    NODE_NUMBER,
    NODE_STRING,
    NODE_VARIABLE,

    NODE_BINARY,

    NODE_ASSIGNMENT,

    NODE_STAMP,

    NODE_IF,
    NODE_ELSE,

    NODE_FLOW,

    NODE_FUNCTION,
    NODE_CALL,

    NODE_RETURN,

    NODE_BLOCK

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

#endif
