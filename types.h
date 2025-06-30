#ifndef TYPES_H
#define TYPES_H
#include <list>
#include <map>
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

int to_type(std::string string);

UserType to_user_type(std::string string);

std::string to_user_type(UserType user_type);

SystemType to_system_type(std::string string);

std::string to_system_type(SystemType system_type);

bool to_bool(char *string);

DeclarationInfo *find_var(std::string identifier, std::list<DeclarationInfo *> declaration_infos);

bool in_correct_node(AstNode* root, AstNode* node_to_find);

DeclarationInfo *find_declaration(Declaration* current, AstNode* node_to_find, std::string name_to_find);

bool check_function_and_return_stmt(AstNode *node);

bool check_several_body_lists(AstNode *node);

bool check_several_subprograms_and_bodylists(AstNode *node);

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
    SystemType system_type;
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
    std::list<int> variable_types;
    UserType return_type;
};

inline bool dbg = true;
inline bool syntax_errors = false;
inline bool semantic_errors = false;

inline std::unordered_map<std::string, SubprogramDeclaration *> subprogram_declarations;

inline Declaration *declaration_root = nullptr;

inline std::set<std::string> bi_operators{"+", "-", "*", "/", "mod", ">", "<", ">=", "<=", "=", "&&", "||"};
inline std::set<std::string> arith_operators{"+", "-", "*", "/", "mod"};
inline std::set<std::string> comp_operators{">", "<", ">=", "<="};
inline std::set<std::string> eq_operators{"="};
inline std::set<std::string> logic_bi_operators{"&&", "||"};
inline std::set<std::string> logic_un_operators{"!"};
inline std::set<std::string> un_operators{"!"};

inline std::map<std::string, std::list<int>> built_in_functions = {
    {"Render", {TYPE_CONTEXT}},
    {"CreateContext", {TYPE_STRING, TYPE_INTEGER, TYPE_INTEGER}},
    {"DrawBezier", {TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE}},
    {"DrawText", {TYPE_STRING, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_STRING}},
    {"DrawCircle", {TYPE_DOUBLE, TYPE_DOUBLE, TYPE_INTEGER}},
    {"DrawRect", {TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE}},
    {"DrawLine", {TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE, TYPE_DOUBLE}},
    {"Fill", {TYPE_SHAPE, TYPE_INTEGER, TYPE_INTEGER, TYPE_INTEGER}},
    {"Rotate", {TYPE_SHAPE, TYPE_DOUBLE}},
    {"Reflect", {TYPE_SHAPE, TYPE_DOUBLE, TYPE_DOUBLE}},
    {"AddToContext", {TYPE_CONTEXT, TYPE_SHAPE}},
    {"RemoveFromContext", {TYPE_CONTEXT, TYPE_SHAPE}},
};

inline std::map<std::string, std::string> built_in_function_labels = {
    {"Render", "Render"},
    {"CreateContext", "CreateContext"},
    {"DrawBezier", "DrawBezier"},
    {"DrawText", "DrawText"},
    {"DrawCircle", "DrawCircle"},
    {"DrawRect", "DrawRect"},
    {"DrawLine", "DrawLine"},
    {"Fill", "Fill"},
    {"Rotate", "Rotate"},
    {"Reflect", "Reflect"},
    {"AddToContext", "AddToContext"},
    {"RemoveFromContext", "RemoveFromContext"},
};

inline std::map<std::string, int> built_in_return_type = {
    {"Render", U_TYPE_INCORRECT},
    {"CreateContext", TYPE_CONTEXT},
    {"DrawBezier", TYPE_SHAPE},
    {"DrawText", TYPE_SHAPE},
    {"DrawCircle", TYPE_SHAPE},
    {"DrawRect", TYPE_SHAPE},
    {"DrawLine", TYPE_SHAPE},
    {"Fill", U_TYPE_INCORRECT},
    {"Rotate", U_TYPE_INCORRECT},
    {"Reflect", TYPE_SHAPE},
    {"AddToContext", U_TYPE_INCORRECT},
    {"RemoveFromContext", U_TYPE_INCORRECT},
};

#endif
