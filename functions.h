#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;


typedef enum {
    NT_PROGRAM = 1,
    NT_FUNCTION,
    NT_PROCEDURE,
    NT_SUBPROG_PARAMS,
    NT_FUNCTION_BODY,
    NT_BODY_LIST,
    NT_BODY,
    NT_ASSIGN_VARIABLE,
    NT_DECLARE_VARIABLE,
    NT_EXPRESSION,
    NT_WHILE_LOOP,
    NT_CONDITION_EXPRESSION,
    NT_INVOCATION,
} NonTerminal;

typedef enum {
    TERMINAL = 1,
    NON_TERMINAL,
    OPERATOR,
    INVOCATION
} ExpressionType;

typedef struct {
    const char *name;
    const char *type;
    int scope_level;
} Identifiers;

typedef union {
    int integer;
    float floating;
    const char *string;
} Value;

typedef struct {
    int type;
    const char *name;
} VariableDeclaration;

typedef struct {
    const char *name;
    Value value;
} VariableAssignation;

// typedef struct {
//     int type;
//     const char* name;
//     Value value;
// } VariableInitialization;


typedef struct AstNode AstNode;
typedef union Member member;

struct AstNode {
    AstNode *tree;
    AstNode *next;
    NonTerminal non_terminal;
    Member *member;
};


typedef union {
    ExpressionType expression_type;
    AstNode *node;
    char *op;
    Value value;
} Expression;

union Member {
    VariableDeclaration variable_declaration;
    VariableAssignation variable_assignation;
    Expression expression;
};

inline AstNode root_node = AstNode();
inline AstNode *root_node_ptr = &root_node;

AstNode *create_node(NonTerminal non_terminal);

void mylog(std::string string);

AstNode *add_seq_node(AstNode *what);

AstNode *add_variable_declaration_node(int type, std::string name);

AstNode *add_variable_assignation_node(std::string name, Value value);

void print_tree();

void _print_tree(AstNode *root, std::string str);

AstNode *create_function_node(AstNode *subprog_params, AstNode *function_body);

AstNode *create_subprog_param_node();

AstNode *add_body_node(AstNode *body);


AstNode *create_function_body_node();

AstNode *create_nodes(NonTerminal non_terminal, std::initializer_list<AstNode *> nodes);

AstNode *add_equal_node(AstNode *main, AstNode *node);

AstNode *add_expression_node();

AstNode *add_expression_node(float value);

AstNode *add_expression_node(char *op);

AstNode *add_expression_node(AstNode *node);

AstNode *add_expression_node(AstNode *left, AstNode *right, char *op);
