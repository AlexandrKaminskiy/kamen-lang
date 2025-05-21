#ifndef TYPES_H
#define TYPES_H
#include <list>
#include <set>

typedef struct AstNode AstNode;
typedef struct Declaration Declaration;
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

struct Declaration {
    Declaration *parent;
    std::list<Declaration *> children;
    std::set<std::string> identifiers;
};

inline Declaration *declaration_root = nullptr;
#endif
