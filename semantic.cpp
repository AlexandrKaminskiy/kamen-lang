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

Declaration * handle_non_terminal(AstNode *root, NonTerminal non_terminal, Declaration *declaration) {
    switch (non_terminal) {
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

            bool ha = has_declaration(declaration, var_name);

            if (ha) {
                cerr << "Variable was already declared " << var_name << endl;
                return declaration;
            }

            declaration->identifiers.insert(var_name);

            return declaration;
        }
        case NT_PROGRAM:
        case NT_FUNCTION:
        case NT_PROCEDURE:
        case NT_BODY_LIST: {
            return create_declaration(declaration);
        }

        default: return declaration;
    }
}

void go_through_tree(AstNode *root, Declaration *declaration) {

    AstNode *node = root;
    while (node != nullptr) {
        Declaration *created_declaration = handle_non_terminal(node, node->non_terminal, declaration);
        if (created_declaration != declaration) {
            declaration->children.push_back(created_declaration);
        }
        if (node->tree != nullptr) {
            go_through_tree(node->tree, created_declaration);
        }
        node = node->next;
    }
}

void make_semantic(AstNode *root) {
    declaration_root = create_declaration(nullptr);
    go_through_tree(root, declaration_root);
    cout << declaration_root << endl;
}
