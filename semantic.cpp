#include "semantic.h"

#include <iostream>
#include <ostream>

using namespace std;

Declaration *create_declaration(Declaration *parent) {
    auto *root = new Declaration();
    root->parent = parent;
    return root;
}

bool has_declaration(Declaration *declaration, string variable) {
    if (declaration->identifiers.find(variable) != declaration->identifiers.end()) {
        return true;
    }

    if (declaration->parent != nullptr) {
        return has_declaration(declaration->parent, variable);
    }

    return false;
}

bool has_subprog_declaration(Declaration *declaration, string subprogram) {
    if (declaration->subprogram_identifiers.find(subprogram) != declaration->subprogram_identifiers.end()) {
        return true;
    }

    if (declaration->parent != nullptr) {
        return has_subprog_declaration(declaration->parent, subprogram);
    }

    return false;
}

Declaration * handle_var_or_function(AstNode *root, NonTerminal non_terminal, Declaration *declaration) {
    switch (non_terminal) {

        case NT_EXPRESSION: {
            if (root->member->expression.expression_type != VARIABLE) {
                return declaration;
            }
            string var_name = string(root->member->expression.identifier);
            bool ha = has_declaration(declaration, var_name);

            if (!ha) {
                cerr << "Variable was not declared " << var_name << endl;
                return declaration;
            }

            return declaration;
        }
        case NT_INVOCATION: {
            auto subprog_name = string(root->member->invocation.identifier);

            bool ha = has_subprog_declaration(declaration, subprog_name);

            if (!ha) {
                cerr << "Subprogram was not declared " << subprog_name << endl;
                return declaration;
            }

            return declaration;
        }
        case NT_ASSIGN_VARIABLE: {
            string var_name = string(root->member->variable_assignation.name);

            bool ha = has_declaration(declaration, var_name);

            if (!ha) {
                cerr << "Variable was not declared " << var_name << endl;
                return declaration;
            }

            return declaration;
        }
        case NT_DECLARE_VARIABLE: {
            string var_name = string(root->member->variable_declaration.name);
            string var_type = string(root->member->variable_declaration.type);

            bool ha = has_declaration(declaration, var_name);

            if (ha) {
                cerr << "Variable was already declared " << var_name << endl;
                return declaration;
            }

            declaration->identifiers.insert(var_name);
            // declaration->types.insert();

            return declaration;
        }
        case NT_FUNCTION: {
            declaration->subprogram_identifiers.insert(root->member->function_declaration.name);
            declaration->types.insert(root->member->function_declaration.return_type);

            return create_declaration(declaration);
        }
        case NT_PROCEDURE: {
            declaration->subprogram_identifiers.insert(root->member->procedure_declaration.name);
            return create_declaration(declaration);
        }
        case NT_PROGRAM:
        case NT_BODY_LIST: {
            return create_declaration(declaration);
        }


        default: return declaration;
    }
}

void check_variable_and_function_visibility(AstNode *root, Declaration *declaration) {
    AstNode *node = root;
    while (node != nullptr) {
        Declaration *created_declaration = handle_var_or_function(node, node->non_terminal, declaration);
        if (created_declaration != declaration) {
            declaration->children.push_back(created_declaration);
        }
        if (node->tree != nullptr) {
            check_variable_and_function_visibility(node->tree, created_declaration);
        }
        node = node->next;
    }
}

void handle_non_terminal_op(AstNode *root, NonTerminal non_terminal) {
    switch (non_terminal) {
        case NT_EXPRESSION: {

        }
    }
}

void check_operation_types(AstNode *root) {
    AstNode *node = root;
    while (node != nullptr) {
        handle_non_terminal_op(node, node->non_terminal);
        if (node->tree != nullptr) {
            check_operation_types(node->tree);
        }
        node = node->next;
    }
}

void make_semantic(AstNode *root) {
    declaration_root = create_declaration(nullptr);
    check_variable_and_function_visibility(root, declaration_root);
    check_operation_types(root);
    cout << declaration_root << endl;
}


