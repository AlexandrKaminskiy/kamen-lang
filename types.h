#ifndef TYPES_H
#define TYPES_H
#include <list>
#include <set>
#include <unordered_map>


typedef struct AstNode AstNode;
typedef struct Declaration Declaration;
typedef struct DeclarationInfo DeclarationInfo;
typedef struct ExpressionInfo ExpressionInfo;
typedef struct SubprogramDeclaration SubprogramDeclaration;
typedef union Member member;\


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


UserType to_user_type(std::string string);

std::string to_user_type(UserType user_type);

SystemType to_system_type(std::string string);

std::string to_system_type(SystemType system_type);

bool to_bool(char *string);

DeclarationInfo *find_var(std::string identifier, std::list<DeclarationInfo *> declaration_infos);

bool in_correct_node(AstNode* root, AstNode* node_to_find);

DeclarationInfo *find_declaration(Declaration* current, AstNode* node_to_find, std::string name_to_find);

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

std::string to_nt_string(NonTerminal non_terminal);

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
    bool boolean;
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
    AstNode *node;
    std::list<Declaration *> children;

    std::list<DeclarationInfo *> variable_declarations;
    std::list<DeclarationInfo *> subprogram_declarations;
    int max_stack_depth;
};

struct DeclarationInfo {
    UserType user_type;
    SystemType system_type;
    std::string identifier;
    int location_in_stack;
    std::string reg;
    bool is_register;
};

struct ExpressionInfo {
    AstNode *node;
    UserType type;
    ExpressionInfo *parent;
    std::list<ExpressionInfo *> children;
};

struct SubprogramDeclaration {
    bool is_function;
    std::list<UserType> variable_types;
    UserType return_type;
};

inline std::unordered_map<std::string, SubprogramDeclaration *> subprogram_declarations;

inline Declaration *declaration_root = nullptr;

inline std::set<std::string> bi_operators{"+", "-", "*", "/", "mod", ">", "<", ">=", "<=", "=", "&&", "||"};
inline std::set<std::string> arith_operators{"+", "-", "*", "/", "mod"};
inline std::set<std::string> comp_operators{">", "<", ">=", "<="};
inline std::set<std::string> eq_operators{"="};
inline std::set<std::string> logic_bi_operators{"&&", "||"};
inline std::set<std::string> logic_un_operators{"!"};
inline std::set<std::string> un_operators{"!"};
#endif
