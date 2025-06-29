#include "semantic.h"

#include <iostream>
#include <ostream>

using namespace std;

ExpressionInfo *create_expression_info() {
    auto *info = new ExpressionInfo();
    return info;
}

DeclarationInfo *create_declaration_for_variable(const std::string &name, const int type) {
    auto declaration_info = new DeclarationInfo();
    declaration_info->identifier = name;

    if (type < 5) {
        declaration_info->user_type = static_cast<UserType>(type);
    } else {
        declaration_info->system_type = static_cast<SystemType>(type);
    }

    return declaration_info;
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

SubprogramDeclaration *create_subprogram_declaration(bool is_function, UserType type, AstNode *variable_declarations) {
    SubprogramDeclaration *subprogram = new SubprogramDeclaration();
    subprogram->is_function = is_function;
    subprogram->return_type = type;

    while (variable_declarations != nullptr) {
        subprogram->variable_types.push_back(to_user_type(variable_declarations->member->variable_declaration.type));
        variable_declarations = variable_declarations->next;
    }
    // subprogram->variable_declarations = variable_declarations;
    return subprogram;
}

SubprogramDeclaration *create_subprogram_declaration(bool is_function, AstNode *variable_declarations) {
    SubprogramDeclaration *subprogram = new SubprogramDeclaration();
    subprogram->is_function = is_function;

    while (variable_declarations != nullptr) {
        subprogram->variable_types.push_back(to_user_type(variable_declarations->member->variable_declaration.type));
        variable_declarations = variable_declarations->next;
    }
    return subprogram;
}

Declaration *handle_var_or_function(AstNode *root, NonTerminal non_terminal, Declaration *declaration) {
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
        case NT_FOR_LOOP: {
            string var_name = string(root->member->for_loop.name);
            bool ha = has_declaration(declaration, var_name);
            if (ha) {
                cerr << "Variable was already declared " << var_name << endl;
                return declaration;
            }
            auto declaration_info = create_declaration_info(var_name, TYPE_INTEGER); // todo
            declaration->variable_declarations.push_back(declaration_info);

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
            auto declaration_info = create_declaration_for_variable(var_name, to_user_type(var_type)); // todo

            declaration->variable_declarations.push_back(declaration_info);

            return declaration;
        }
        case NT_FUNCTION: {
            auto declaration_info = create_declaration_info(root->member->function_declaration.name,
                                                            root->member->function_declaration.return_type);

            if (subprogram_declarations.count(root->member->function_declaration.name) > 0) {
                cerr << "Subprogram was declared " << declaration_info->identifier << endl;
            }
            auto subprogram_declaration = create_subprogram_declaration(
                true, root->member->function_declaration.return_type, root->tree->tree);
            subprogram_declarations[string(root->member->function_declaration.name)] = subprogram_declaration;
            declaration->subprogram_declarations.push_back(declaration_info);

            return create_declaration(declaration, root);
        }
        case NT_PROCEDURE: {
            DeclarationInfo *declaration_info = create_declaration_info(root->member->function_declaration.name);

            if (subprogram_declarations.count(root->member->function_declaration.name) > 0) {
                cerr << "Subprogram was declared " << declaration_info->identifier << endl;
            }
            auto subprogram_declaration = create_subprogram_declaration(false, root->tree->tree);
            subprogram_declarations[string(root->member->function_declaration.name)] = subprogram_declaration;
            declaration->subprogram_declarations.push_back(declaration_info);

            return create_declaration(declaration, root);
        }
        case NT_BODY_LIST:
        case NT_PROGRAM: {
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
        if (check_function_and_return_stmt(node)) {
            break;
        }
        node = node->next;
    }
}

void handle_arith_ops(AstNode *root, AstNode *left, AstNode *right) {
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

void handle_comp_ops(AstNode *root, AstNode *left, AstNode *right) {
    if ((left->member->expression.type == TYPE_DOUBLE || left->member->expression.type == TYPE_INTEGER) &&
        (right->member->expression.type == TYPE_DOUBLE || right->member->expression.type == TYPE_INTEGER)) {
        root->member->expression.type = TYPE_BOOLEAN;
        return;
    }

    cerr << "Incorrect types for operator " << root->member->expression.op << endl;
}

void handle_eq_ops(AstNode *root, AstNode *left, AstNode *right) {
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

void handle_logical_bi_ops(AstNode *root, AstNode *left, AstNode *right) {
    if (left->member->expression.type == TYPE_BOOLEAN || right->member->expression.type == TYPE_BOOLEAN) {
        root->member->expression.type = TYPE_BOOLEAN;
        return;
    }

    cerr << "Incorrect types for operator " << root->member->expression.op << endl;
}

void handle_logical_unary_ops(AstNode *root, AstNode *left) {
    if (left->member->expression.type == TYPE_BOOLEAN) {
        root->member->expression.type = TYPE_BOOLEAN;
        return;
    }

    cerr << "Incorrect types for operator " << root->member->expression.op << endl;
}

bool handle_non_terminal_op(AstNode *root, NonTerminal non_terminal) {
    cout << "Non-terminal handling..." << to_nt_string(non_terminal) << endl;
    switch (non_terminal) {
        case NT_FUNCTION: {
            auto subprogram_info = subprogram_declarations[root->member->function_declaration.name];

            auto return_node = root->tree->next->next;
            handle_non_terminal_op(return_node, return_node->non_terminal);
            if (subprogram_info->return_type != return_node->member->expression.type) {
                cerr << "Incorrect return type. Expected: " << to_user_type(subprogram_info->return_type) << ". Given: "
                        << to_user_type(return_node->member->expression.type) << endl;
            }
            return false;
        }
        case NT_WHILE_LOOP: {
            auto condition_expression = root->tree;
            handle_non_terminal_op(condition_expression, condition_expression->non_terminal);
            if (condition_expression->member->expression.type != TYPE_BOOLEAN) {
                cerr << "Incorrect type for while loop condition. Expected:  " << to_user_type(TYPE_BOOLEAN) <<
                        ". Given: " << to_user_type(condition_expression->member->expression.type) << endl;
            }
            return true;
        }
        case NT_IF_BLOCK: {
            auto condition_expression = root->tree;
            handle_non_terminal_op(condition_expression, condition_expression->non_terminal);
            if (condition_expression->member->expression.type != TYPE_BOOLEAN) {
                cerr << "Incorrect type for if condition. Expected:  " << to_user_type(TYPE_BOOLEAN) << ". Given: " <<
                        to_user_type(condition_expression->member->expression.type) << endl;
            }
            return false;
        }
        case NT_INVOCATION: {
            auto subprogram_info = subprogram_declarations[root->member->invocation.identifier];
            auto var_type = subprogram_info->variable_types.begin();

            auto param = root->tree->tree;
            while (param != nullptr) {
                handle_non_terminal_op(param, param->non_terminal);
                if (param->member->expression.type != *var_type) {
                    cerr << "Incorrect subprogram invocation: " << root->member->invocation.identifier;
                }
                ++var_type;
                param = param->next;
                if ((var_type == subprogram_info->variable_types.end() && param != nullptr)
                    || (var_type != subprogram_info->variable_types.end() && param == nullptr)) {
                    cerr << "Incorrect subprogram invocation: " << root->member->invocation.identifier;
                    break;
                }
            }
            if ((var_type == subprogram_info->variable_types.end() && param != nullptr)
                || (var_type != subprogram_info->variable_types.end() && param == nullptr)) {
                cerr << "Incorrect subprogram invocation: " << root->member->invocation.identifier;
            }

            return true;
        }
        case NT_ASSIGN_VARIABLE: {
            auto declaration_info = find_declaration(declaration_root, root, root->member->variable_assignation.name);
            auto expression = root->tree;
            handle_non_terminal_op(expression, expression->non_terminal);
            if (expression->member->expression.type != declaration_info->user_type) {
                cerr << "Incorrect type for variable assignation. Expected:  " <<
                        to_user_type(declaration_info->user_type) << ". Given: " << to_user_type(
                            expression->member->expression.type) << endl;
            }
            return true;
        }
        case NT_EXPRESSION: {
            if (root->member->expression.expression_type == VARIABLE) {
                DeclarationInfo *declaration_info = find_declaration(declaration_root, root,
                                                                     root->member->expression.identifier);
                auto var_type = declaration_info->user_type;
                root->member->expression.type = var_type;
                return true;
            }
            if (root->member->expression.expression_type == INVOCATION) {
                auto invocation = root->tree;
                auto subprogram_info = subprogram_declarations[invocation->member->invocation.identifier];
                auto left = root->tree;
                handle_non_terminal_op(left, left->non_terminal);

                root->member->expression.type = subprogram_info->return_type;
                return true;
            }

            if (root->member->expression.expression_type == NON_TERMINAL) {
                if (root->member->expression.op == nullptr) {
                    auto left = root->tree;
                    handle_non_terminal_op(left, left->non_terminal);
                    root->member->expression.type = left->member->expression.type;
                    return true;
                }
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
                    if (eq_operators.find(string(root->member->expression.op)) != eq_operators.end()) {
                        handle_eq_ops(root, left, right);
                        return true;
                    }
                    if (logic_bi_operators.find(string(root->member->expression.op)) != logic_bi_operators.end()) {
                        handle_logical_bi_ops(root, left, right);
                        return true;
                    }
                }
                if (un_operators.find(string(root->member->expression.op)) != un_operators.end()) {
                    auto left = root->tree;
                    handle_non_terminal_op(left, left->non_terminal);
                    handle_logical_unary_ops(root, left);
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

void has_main() {
    auto subprogram_declaration = subprogram_declarations["main"];
    if (subprogram_declaration == nullptr) {
        cerr << "Program doesnt contain an entrypoint" << endl;
        return;
    }

    if (subprogram_declaration->is_function) {
        cerr << "Entrypoint should be a procedure" << endl;
        return;
    }

    if (!subprogram_declaration->variable_types.empty()) {
        cerr << "Entrypoint shouldn't contain procedure params" << endl;
    }
}

void make_semantic(AstNode *root) {
    declaration_root = create_declaration(nullptr, root);
    check_variable_and_function_visibility(root, declaration_root);
    check_operation_types(root);
    has_main();
    // subprogram_declarations.begin();
    // cout << subprogram_declarations << endl;
}
