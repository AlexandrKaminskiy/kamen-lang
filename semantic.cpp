#include "semantic.h"

#include <iostream>
#include <ostream>

using namespace std;

ExpressionInfo *create_expression_info() {
    auto *info = new ExpressionInfo();
    return info;
}

DeclarationInfo *create_declaration_info(const std::string &name, const UserType type) {
    auto declaration_info = new DeclarationInfo();
    declaration_info->identifier = name;
    declaration_info->user_type = type;
    return declaration_info;
}

DeclarationInfo *create_declaration_info(const std::string &name) {
    auto declaration_info = new DeclarationInfo();
    declaration_info->identifier = name;
    return declaration_info;
}

DeclarationInfo *find_var(std::string identifier, std::list<DeclarationInfo *> declaration_infos) {
    for (const auto declaration_info : declaration_infos) {
        if (declaration_info->identifier == identifier) {
            return declaration_info;
        }
    }
    return nullptr;
}

bool in_correct_node(AstNode* root, AstNode* node_to_find) {
    if (root == node_to_find) {
        return true;
    }
    if (root->tree != nullptr) {
        bool result = in_correct_node(root->tree, node_to_find);
        if (result == true) {
            return true;
        }
    }
    while (root->next != nullptr) {
        bool result = in_correct_node(root->next, node_to_find);
        if (result == true) {
            return true;
        }
        root = root->next;
    }
    return false;
}

DeclarationInfo *find_declaration(Declaration* current, AstNode* node_to_find, string name_to_find) {
    auto declaration_info = find_var(name_to_find, current->variable_declarations);

    if (declaration_info != nullptr && in_correct_node(current->node, node_to_find)) {
        cout << "Declaration " << name_to_find << " found!" << endl;
        return declaration_info;
    }

    for (Declaration *decl : current->children) {
        DeclarationInfo* found_decl = find_declaration(decl, node_to_find, name_to_find);
        if (found_decl != nullptr) {
            return found_decl;
        }
    }
    return nullptr;
}

Declaration *create_declaration(Declaration *parent, AstNode *node) {
    auto *root = new Declaration();
    root->node = node;
    root->parent = parent;
    return root;
}

bool has_declaration(Declaration *declaration, string variable) {
    if (find_var(variable, declaration->variable_declarations) != nullptr) {
        return true;
    }

    if (declaration->parent != nullptr) {
        return has_declaration(declaration->parent, variable);
    }

    return false;
}

bool has_subprog_declaration(Declaration *declaration, string subprogram) {
    if (find_var(subprogram, declaration->subprogram_declarations) != nullptr) {
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
            auto declaration_info = create_declaration_info(var_name, to_user_type(var_type)); // todo

            declaration->variable_declarations.push_back(declaration_info);

            return declaration;
        }
        case NT_FUNCTION: {
            auto declaration_info = create_declaration_info(root->member->function_declaration.name, root->member->function_declaration.return_type);
            declaration->subprogram_declarations.push_back(declaration_info);

            return create_declaration(declaration, root);
        }
        case NT_PROCEDURE: {
            DeclarationInfo* declaration_info = create_declaration_info(root->member->function_declaration.name);
            declaration->subprogram_declarations.push_back(declaration_info);
            return create_declaration(declaration, root);
        }
        case NT_PROGRAM:
        case NT_BODY_LIST: {
            return create_declaration(declaration, root);
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

void handle_arith_ops(AstNode* root, AstNode *left, AstNode *right) {
    if (left->member->expression.type == TYPE_INTEGER && right->member->expression.type == TYPE_INTEGER) {
        root->member->expression.type = TYPE_INTEGER;
        return;
    }

    if ((left->member->expression.type == TYPE_DOUBLE || left->member->expression.type == TYPE_INTEGER) &&
        (right->member->expression.type == TYPE_DOUBLE || right->member->expression.type == TYPE_INTEGER)) {
        root->member->expression.type = TYPE_DOUBLE;
        return;
    }
    cerr << "Incorrect types for operator " << root->member->expression.op << endl;
}

void handle_comp_ops(AstNode* root, AstNode *left, AstNode *right) {

    if ((left->member->expression.type == TYPE_DOUBLE || left->member->expression.type == TYPE_INTEGER) &&
        (right->member->expression.type == TYPE_DOUBLE || right->member->expression.type == TYPE_INTEGER)) {
        root->member->expression.type = TYPE_BOOLEAN;
        return;
    }

    if (left->member->expression.type == TYPE_BOOLEAN || right->member->expression.type == TYPE_BOOLEAN) {
        root->member->expression.type = TYPE_BOOLEAN;
        return;
    }

    cerr << "Incorrect types for operator " << root->member->expression.op << endl;
}

void handle_logical_bi_ops(AstNode* root, AstNode *left, AstNode *right) {

    if (left->member->expression.type == TYPE_BOOLEAN || right->member->expression.type == TYPE_BOOLEAN) {
        root->member->expression.type = TYPE_BOOLEAN;
        return;
    }

    cerr << "Incorrect types for operator " << root->member->expression.op << endl;
}

void handle_logical_unary_ops(AstNode* root, AstNode *left) {

    if (left->member->expression.type == TYPE_BOOLEAN) {
        root->member->expression.type = TYPE_BOOLEAN;
        return;
    }

    cerr << "Incorrect types for operator " << root->member->expression.op << endl;
}

bool handle_non_terminal_op(AstNode *root, NonTerminal non_terminal) {
    cout << "Non-terminal handling..." << to_nt_string(non_terminal) << endl;
    switch (non_terminal) {
        case NT_EXPRESSION: {
            if (root->member->expression.expression_type == VARIABLE) {
                DeclarationInfo* declaration_info = find_declaration(declaration_root, root, root->member->expression.identifier);
                auto var_type = declaration_info->user_type;
                root->member->expression.type = var_type;
            }
            // if (root->member->expression.expression_type == INVOCATION) {
            //     return;
            // }

            if (root->member->expression.expression_type == NON_TERMINAL) {
                if (bi_operators.find(string(root->member->expression.op)) != bi_operators.end()) {
                    auto left = root->tree;
                    auto right = root->tree->next;
                    handle_non_terminal_op(left, left->non_terminal);
                    handle_non_terminal_op(right, right->non_terminal);
                    if (arith_operators.find(string(root->member->expression.op)) != arith_operators.end()) {
                        handle_arith_ops(root, left, right);
                        return true;
                    }
                    if (comp_operators.find(string(root->member->expression.op)) != comp_operators.end()) {
                        handle_comp_ops(root, left, right);
                        return true;
                    }
                    if (logic_bi_operators.find(string(root->member->expression.op)) != logic_bi_operators.end()) {
                        handle_logical_bi_ops(root, left, right);
                        return true;
                    }
                }
            }
            return true;
        }
        default: return false;
    }
}

void check_operation_types(AstNode *root) {
    AstNode *node = root;
    while (node != nullptr) {
        bool handled = handle_non_terminal_op(node, node->non_terminal);

        if (!handled && node->tree != nullptr) {
            check_operation_types(node->tree);
        }
        node = node->next;
    }
}

void make_semantic(AstNode *root) {
    declaration_root = create_declaration(nullptr, root);
    check_variable_and_function_visibility(root, declaration_root);
    check_operation_types(root);
    cout << declaration_root << endl;
}