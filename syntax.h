#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;

typedef struct AstNode AstNode;
typedef union Member member;

/*
 * User type - can use anywhere
 */
typedef enum {
    U_TYPE_INCORRECT = 0,
    TYPE_INTEGER,
    TYPE_STRING,
    TYPE_DOUBLE,
    TYPE_BOOLEAN,
} UserType;

/*
 * System type - can use only in embedded function invocation or declaration (and assignation)
 */
typedef enum {
    S_TYPE_INCORRECT = 5,
    TYPE_SHAPE,
    TYPE_CONTEXT,
} SystemType;

typedef enum {
    NT_PROGRAM = 1,
    NT_SUBPROGRAMS,
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
    NT_FOR_LOOP,
    NT_INVOCATION,
    NT_ENUMERATION,
    NT_CREATE_LINE,
    NT_IF_BLOCK,
} NonTerminal;

typedef enum {
    TERMINAL = 1,
    NON_TERMINAL,
    OPERATOR,
    INVOCATION,
    VARIABLE,
} ExpressionType;

// typedef struct {
//     const char *name;
//     const char *type;
//     int scope_level;
// } Identifiers;

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
} CreateLine;

typedef struct {
    const char *name;
} VariableAssignation;

typedef struct {
    UserType return_type;
    const char *name;
} FunctionDeclaration;

typedef struct {
    UserType return_type;
    const char *name;
} ProcedureDeclaration;

typedef struct {
    const char *name;
} ForLoop;

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
    UserType type;
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
    ProcedureDeclaration procedure_declaration;
    Invocation invocation;
    ForLoop for_loop;
    CreateLine create_line;
};

inline AstNode root_node = AstNode();
inline AstNode *root_node_ptr = &root_node;

AstNode *create_node(NonTerminal non_terminal);

UserType to_user_type(std::string string);

std::string to_user_type(UserType user_type);

SystemType to_system_type(std::string string);

std::string to_system_type(SystemType system_type);

AstNode *add_seq_node(AstNode *what);

AstNode *add_variable_declaration_node(std::string type, std::string name);

AstNode *add_variable_assignation_node(std::string name, AstNode *value);

void print_tree();

std::string _print_tree(AstNode *root, std::string indent, std::string string);

AstNode *add_function_node(char *name, UserType return_type, AstNode *subprog_params, AstNode *function_body);

AstNode *add_procedure_node(char *name, AstNode *subprog_params, AstNode *procedure_body);

AstNode *create_subprog_param_node();

AstNode *add_body_node(AstNode *body);

AstNode *create_function_body_node();

AstNode *create_nodes(NonTerminal non_terminal, std::initializer_list<AstNode *> nodes);

AstNode *add_equal_node(AstNode *main, AstNode *node);

AstNode *add_expression_node();

AstNode *add_expression_node(int value);

AstNode *add_expression_node(float value);

AstNode *add_expression_node(char* value, int stub); //todo fix

AstNode *add_expression_node(const char *identifier);

AstNode *add_expression_node(AstNode *node);

AstNode *add_expression_node(AstNode *node, char *op);

AstNode *add_expression_node(AstNode *left, AstNode *right, char *op);

AstNode *add_invocation(char *name, AstNode *enumeration);

AstNode *add_for_loop(char *loop_var, AstNode *from, AstNode *to, AstNode *body);

AstNode *add_create_line_node(char *name, AstNode *first, AstNode *second);

AstNode *add_if_statement(AstNode *if_block, AstNode *if_body);

AstNode *add_if_statement(AstNode *if_block, AstNode *if_body, AstNode *else_block, AstNode *else_body);
