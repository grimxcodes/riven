#ifndef AST_H
#define AST_H

typedef enum {

    NODE_NUMBER,
    NODE_STRING,
    NODE_BOOLEAN,
    NODE_NULL,

    NODE_VARIABLE,

    NODE_ASSIGNMENT,
    NODE_CONSTANT_ASSIGNMENT,

    NODE_BINARY,

    NODE_STAMP,
    NODE_INPUT,

    NODE_IF,
    NODE_FLOW,
    NODE_WHILE,

    NODE_FUNCTION,
    NODE_CALL,
    NODE_RETURN,

    NODE_INCREMENT,
    NODE_DECREMENT,

    NODE_ARRAY,
    NODE_INDEX,

    NODE_BLOCK,

    NODE_FRAME,
    NODE_OBJECT,
    NODE_MEMBER,

    NODE_RECORD,

    NODE_PTR,
    NODE_REF,

    NODE_RAW,

    NODE_FETCH,

    NODE_SPARK,
    NODE_SYNC,

    NODE_FILE,

    NODE_CAST,

    NODE_MEMBER_ASSIGNMENT,

    NODE_METHOD_CALL

} NodeType;

typedef struct ASTNode {

    NodeType type;

    char* value;

    char* name;

    char* extra;

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
