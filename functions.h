#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

typedef struct AstNode AstNode;
typedef union Member member;


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
    NT_INVOCATION,
    NT_ENUMERATION,
} NonTerminal;

typedef enum {
    TERMINAL = 1,
    NON_TERMINAL,
    OPERATOR,
    INVOCATION,
    VARIABLE,
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
    const char *type;
    const char *name;
} VariableDeclaration;

typedef struct {
    const char *name;
    Value value;
} VariableAssignation;

typedef struct {
    AstNode *variable_declarations;
    const char *return_type;
    const char *name;
} FunctionDeclaration;

// typedef struct {
//     int type;
//     const char* name;
//     Value value;
// } VariableInitialization;


struct AstNode {
    AstNode *tree;
    AstNode *next;
    NonTerminal non_terminal;
    Member *member;
};

typedef struct {
    ExpressionType expression_type;
    AstNode *node;
    char *op;
    Value value;
    char *identifier;
} Expression;

typedef struct {
    char *identifier;
} Invocation;

union Member {
    VariableDeclaration variable_declaration;
    VariableAssignation variable_assignation;
    Expression expression;
    FunctionDeclaration function_declaration;
    Invocation invocation;
};

inline AstNode root_node = AstNode();
inline AstNode *root_node_ptr = &root_node;

AstNode *create_node(NonTerminal non_terminal);

void mylog(std::string string);

AstNode *add_seq_node(AstNode *what);

AstNode *add_variable_declaration_node(std::string type, std::string name);

AstNode *add_variable_assignation_node(std::string name, Value value);

void print_tree();

std::string _print_tree(AstNode *root, std::string indent, std::string string);

AstNode *add_function_node(char *name, char *return_type, AstNode *subprog_params, AstNode *function_body);

AstNode *create_subprog_param_node();

AstNode *add_body_node(AstNode *body);

AstNode *create_function_body_node();

AstNode *create_nodes(NonTerminal non_terminal, std::initializer_list<AstNode *> nodes);

AstNode *add_equal_node(AstNode *main, AstNode *node);

AstNode *add_expression_node();

AstNode *add_expression_node(int value);

AstNode *add_expression_node(const char *identifier);

AstNode *add_expression_node(AstNode *node);

AstNode *add_expression_node(AstNode *node, char *op);

AstNode *add_expression_node(AstNode *left, AstNode *right, char *op);

AstNode *add_invocation(char *name, AstNode *enumeration);
