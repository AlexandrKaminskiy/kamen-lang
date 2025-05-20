#include "syntax.h"

using namespace std;

std::string to_user_type(UserType user_type) {
    switch (user_type) {
        case TYPE_DOUBLE: return "Double";
        case TYPE_INTEGER: return "Integer";
        case TYPE_BOOLEAN: return "Boolean";
        case TYPE_STRING: return "String";
        default: return "U_Incorrect";
    }
}

UserType to_user_type(std::string string) {
    if (string == "Integer") {
        return TYPE_INTEGER;
    }
    if (string == "String") {
        return TYPE_STRING;
    }
    if (string == "Double") {
        return TYPE_DOUBLE;
    }
    if (string == "Boolean") {
        return TYPE_BOOLEAN;
    }
    return U_TYPE_INCORRECT;
}


std::string to_system_type(SystemType system_type) {
    switch (system_type) {
        case TYPE_SHAPE: return "Shape";
        case TYPE_CONTEXT: return "Context";
        default: return "SYS_Incorrect";
    }
}

SystemType to_system_type(std::string string) {
    if (string == "Shape") {
        return TYPE_SHAPE;
    }
    if (string == "Context") {
        return TYPE_CONTEXT;
    }

    return S_TYPE_INCORRECT;
}

AstNode *create_node(const NonTerminal non_terminal) {
    auto *node_ptr = new AstNode();
    node_ptr->next = nullptr;
    node_ptr->tree = nullptr;
    node_ptr->member = new Member();

    node_ptr->non_terminal = non_terminal;
    return node_ptr;
}

AstNode *add_seq_node(AstNode *what) {
    auto *node_ptr = new AstNode();
    node_ptr->next = what;
    return node_ptr;
}

void print_tree() {
    cout << "Printing tree" << endl;
    cout << _print_tree(root_node_ptr, "", "");
}

std::string _print_declare_variable(AstNode *root) {
    if (root == nullptr) {
        return "";
    }

    std::string result = "NAME " + std::string(root->member->variable_declaration.name) + " ";
    result += ("TYPE " + std::string(root->member->variable_declaration.type) + " ");
    return result;
}

std::string _print_assign_variable(AstNode *root) {
    if (root == nullptr) {
        return "";
    }

    std::string result = "NAME " + std::string(root->member->variable_assignation.name) + " ";
    return result;
}

std::string _print_function(AstNode *root) {
    if (root == nullptr) {
        return "";
    }

    std::string result = "NAME " + std::string(root->member->function_declaration.name) + " ";
    result += ("RETURN TYPE " + to_user_type(root->member->function_declaration.return_type) + " ");
    return result;
}

std::string _print_procedure(AstNode *root) {
    if (root == nullptr) {
        return "";
    }

    std::string result = "NAME " + std::string(root->member->procedure_declaration.name) + " ";
    return result;
}

std::string _print_invocation(AstNode *root) {
    if (root == nullptr) {
        return "";
    }

    std::string result = "NAME " + std::string(root->member->invocation.identifier) + " ";
    return result;
}

std::string _print_for_loop(AstNode *root) {
    if (root == nullptr) {
        return "";
    }

    std::string result = "LOOP VARIABLE " + std::string(root->member->for_loop.name) + " ";
    return result;
}

std::string _print_enumeration(AstNode *root) {
    if (root == nullptr) {
        return "";
    }

    return _print_tree(root->next, "", "");
}

std::string _print_create_line(AstNode *root) {
    if (root == nullptr) {
        return "";
    }

    return "CREATE LINE INVOCATION, VARIABLE " + std::string(root->member->create_line.name) + " ";
}

// std::string _print_if_block(AstNode *root) {
//     if (root == nullptr) {
//         return "";
//     }
//
//     return "IF_ELSE_BLOCK ";
// }

std::string _print_expression(AstNode *root) {
    if (root == nullptr) {
        return "";
    }

    if (root->member->expression.expression_type == TERMINAL) {
        if (root->member->expression.type == TYPE_INTEGER) {
            return to_user_type(root->member->expression.type) + " " + std::to_string(root->member->expression.value.integer);
        }

        if (root->member->expression.type == TYPE_DOUBLE) {
            return to_user_type(root->member->expression.type) + " " + std::to_string(root->member->expression.value.floating);
        }

        if (root->member->expression.type == TYPE_STRING) {
            return to_user_type(root->member->expression.type) + " " + std::string(root->member->expression.value.string);
        }
    }

    if (root->member->expression.expression_type == NON_TERMINAL) {
        std::string tree_part = _print_tree(root->member->expression.node, "", "");
        if (root->member->expression.op != nullptr) {
            return "OPERATION " + std::string(root->member->expression.op) + tree_part;
        }
    }

    if (root->member->expression.expression_type == VARIABLE) {
        return "VARIABLE " + std::string(root->member->expression.identifier);
    }

    return "";
}


std::string _print_nt(NonTerminal non_terminal, AstNode *root) {
    switch (non_terminal) {
        case NT_PROGRAM: return "NT_PROGRAM ";
        case NT_FUNCTION: return "NT_FUNCTION " + _print_function(root);
        case NT_PROCEDURE: return "NT_PROCEDURE " + _print_procedure(root);
        case NT_SUBPROG_PARAMS: return "NT_SUBPROG_PARAMS";
        case NT_FOR_LOOP: return "NT_FOR_LOOP " + _print_for_loop(root);
        case NT_FUNCTION_BODY: return "NT_FUNCTION_BODY";
        case NT_BODY_LIST: return "NT_BODY_LIST";
        case NT_BODY: return "NT_BODY";
        case NT_ASSIGN_VARIABLE: return "NT_ASSIGN_VARIABLE " + _print_assign_variable(root);
        case NT_DECLARE_VARIABLE: return "NT_DECLARE_VARIABLE " + _print_declare_variable(root);
        case NT_EXPRESSION: return "NT_EXPRESSION " + _print_expression(root);
        case NT_WHILE_LOOP: return "NT_WHILE_LOOP";
        case NT_INVOCATION: return "NT_INVOCATION " + _print_invocation(root);
        case NT_ENUMERATION: return "NT_ENUMERATION" + _print_enumeration(root); ;
        case NT_CREATE_LINE: return "NT_CREATE_LINE " + _print_create_line(root); ;
        case NT_IF_BLOCK: return "NT_IF_BLOCK ";
    }
}

std::string _print_tree(AstNode *root, std::string indent, std::string string) {
    AstNode *node = root;
    while (node != nullptr) {
        string += indent + _print_nt(node->non_terminal, node) + "\n";
        if (node->tree != nullptr) {
            string += _print_tree(node->tree, indent + std::string(4, ' '), "");
        }
        node = node->next;
    }
    return string;
}

AstNode *add_function_node(char* name, UserType return_type, AstNode *subprog_params, AstNode *function_body) {
    AstNode *root = create_nodes(NT_FUNCTION, {subprog_params, function_body});
    root->member->function_declaration.name = strdup(name);
    root->member->function_declaration.return_type = return_type;
    return root;
}

AstNode *add_procedure_node(char *name, AstNode *subprog_params, AstNode *procedure_body) {
    AstNode *root = create_nodes(NT_PROCEDURE, {subprog_params, procedure_body});
    root->member->procedure_declaration.name = strdup(name);
    return root;
}


AstNode *add_body_node(AstNode *body) {
    AstNode *root = create_node(NT_BODY);
    root->tree = body;
    return root;
}

AstNode *create_subprog_param_node() {
    AstNode *root = create_node(NT_SUBPROG_PARAMS);
    return root;
}

AstNode *add_variable_declaration_node(const std::string type, const std::string name) {
    AstNode *root = create_node(NT_DECLARE_VARIABLE);
    const VariableDeclaration variable = {type.c_str(), name.c_str()};
    root->member->variable_declaration = variable;
    cout << "adding declaration node " << name << endl;
    return root;
}

AstNode *add_variable_assignation_node(const std::string name, AstNode *value) {
    AstNode *root = create_node(NT_ASSIGN_VARIABLE);
    const VariableAssignation variable = {name.c_str()};
    root->member->variable_assignation = variable;
    root->tree = value;
    cout << "adding assignation node " << name << endl;
    return root;
}

AstNode *add_expression_node(const int value) {
    auto root = create_node(NT_EXPRESSION);
    root->member->expression.value.integer = value;
    root->member->expression.type = TYPE_INTEGER;
    root->member->expression.expression_type = TERMINAL;
    return root;
}

AstNode *add_expression_node(const float value) {
    auto root = create_node(NT_EXPRESSION);
    root->member->expression.value.floating = value;
    root->member->expression.type = TYPE_DOUBLE;
    root->member->expression.expression_type = TERMINAL;
    return root;
}

AstNode *add_expression_node(char* value, int stub) {
    auto root = create_node(NT_EXPRESSION);
    root->member->expression.value.string = value;
    root->member->expression.type = TYPE_STRING;
    root->member->expression.expression_type = TERMINAL;
    return root;
}

AstNode *add_expression_node(const char* identifier) {
    auto root = create_node(NT_EXPRESSION);
    root->member->expression.identifier = strdup(identifier);
    root->member->expression.expression_type = VARIABLE;
    return root;
}

AstNode *add_expression_node(AstNode* node) {
    auto root = create_node(NT_EXPRESSION);
    root->member->expression.expression_type = NON_TERMINAL;
    root->tree = node;
    return root;
}

AstNode *add_expression_node(AstNode *node, char *op) {
    auto root = create_node(NT_EXPRESSION);
    root->tree = node;
    root->member->expression.op = strdup(op);
    root->member->expression.expression_type = NON_TERMINAL;

    return root;
}

AstNode *add_expression_node(AstNode* left, AstNode* right, char* op) {
    auto root = create_node(NT_EXPRESSION);
    root->tree = left;
    root->tree->next = right;
    root->member->expression.op = strdup(op);
    root->member->expression.expression_type = NON_TERMINAL;

    return root;
}


AstNode *create_nodes(NonTerminal non_terminal, std::initializer_list<AstNode *> nodes) {
    AstNode *root = create_node(non_terminal);
    root->tree = *nodes.begin();
    AstNode *prev_node = root->tree;
    int i = 0;
    for (AstNode *node: nodes) {
        if (i++ == 0) {
            continue;
        }
        prev_node->next = node;
        prev_node = node;
    }
    return root;
}

AstNode *add_equal_node(AstNode *main, AstNode *node) {
    if (node == nullptr) {
        return main;
    }

    if (main->tree == nullptr) {
        main->tree = node;
        return main;
    }

    auto first = main->tree;

    while (first->next != nullptr) {
        first = first->next;
    }
    first->next = node;
    return main;
}

AstNode *add_invocation(char *name, AstNode *enumeration) {
    AstNode *root = create_node(NT_INVOCATION);
    root->tree = enumeration;
    root->member->invocation.identifier = name;
    return root;
}

AstNode *add_for_loop(char *loop_var, AstNode *from, AstNode *to, AstNode *body) {
    AstNode *root = create_nodes(NT_FOR_LOOP, {from, to, body});
    root->member->for_loop.name = strdup(loop_var);
    return root;
}

AstNode *add_create_line_node(char *name, AstNode *first, AstNode *second) {
    AstNode *root = create_nodes(NT_CREATE_LINE, {first, second});
    root->member->create_line.name = strdup(name);
    return root;
}

AstNode *add_if_statement(AstNode *if_block, AstNode *if_body) {
    AstNode *root = create_nodes(NT_IF_BLOCK, {if_block, if_body});

}

AstNode *add_if_statement(AstNode *if_block, AstNode *if_body, AstNode *else_block, AstNode *else_body) {

}