#include "functions.h"
#include <iostream>
#include <string>
#include <vector>
using namespace std;

std::vector<Identifiers> identifiers;


void mylog(std::string string) {
    Identifiers identifier = Identifiers();
    identifier.name = string.c_str();
    identifier.scope_level = 1;
    identifier.type = "function";

    identifiers.push_back(identifier);
    std::cout << string << std::endl;
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
    cout << _print_tree(root_node_ptr->next, "", "");
}

std::string _print_expression(AstNode *root) {
    if (root == nullptr) {
        return "";
    }

    if (root->member->expression.expression_type == TERMINAL) {
        return " " + std::to_string(root->member->expression.value.integer);
    }

    if (root->member->expression.expression_type == NON_TERMINAL) {
        std::string tree_part = _print_tree(root->member->expression.node, "", "");
        if (root->member->expression.op != nullptr) {
            return " OPERATION " + std::string(root->member->expression.op) + tree_part;
        }
    }

    if (root->member->expression.expression_type == VARIABLE) {
        return " VARIABLE " + std::string(root->member->expression.identifier);
    }

    return "";
}


std::string _print_nt(NonTerminal non_terminal, AstNode *root) {
    switch (non_terminal) {
        case NT_PROGRAM: return "NT_PROGRAM";
        case NT_FUNCTION: return "NT_FUNCTION";
        case NT_PROCEDURE: return "NT_PROCEDURE";
        case NT_SUBPROG_PARAMS: return "NT_SUBPROG_PARAMS";
        case NT_FUNCTION_BODY: return "NT_FUNCTION_BODY";
        case NT_BODY_LIST: return "NT_BODY_LIST";
        case NT_BODY: return "NT_BODY";
        case NT_ASSIGN_VARIABLE: return "NT_ASSIGN_VARIABLE";
        case NT_DECLARE_VARIABLE: return "NT_DECLARE_VARIABLE";
        case NT_EXPRESSION: return "NT_EXPRESSION" + _print_expression(root);
        case NT_WHILE_LOOP: return "NT_WHILE_LOOP";
        case NT_INVOCATION: return "NT_INVOCATION";
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

AstNode *create_function_node(AstNode *subprog_params, AstNode *function_body) {
    AstNode *root = create_node(NT_FUNCTION);
    AstNode *node0 = subprog_params;
    AstNode *node1 = function_body;
    root->tree = node0;
    node0->next = node1;
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

AstNode *add_variable_declaration_node(const int type, const std::string name) {
    AstNode *root = create_node(NT_DECLARE_VARIABLE);
    const VariableDeclaration variable = {type, name.c_str()};
    root->member->variable_declaration = variable;
    cout << "adding declaration node " << name << endl;
    return root;
}

AstNode *add_variable_assignation_node(const std::string name, const Value value) {
    AstNode *root = create_node(NT_ASSIGN_VARIABLE);
    const VariableAssignation variable = {name.c_str(), value};
    root->member->variable_assignation = variable;
    cout << "adding assignation node " << name << endl;
    return root;
}

AstNode *add_expression_node(const int value) {
    auto root = create_node(NT_EXPRESSION);
    root->member->expression.value.integer = value;
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
