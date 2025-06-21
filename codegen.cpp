//
// Created by Alexandr Kaminskiy on 21.06.2025.
//

#include "codegen.h"

std::string handle_constant(AstNode *root) {
    if (root->non_terminal != NT_EXPRESSION) {
        return"";
    }
    if (root->member->expression.expression_type != TERMINAL) {
        return "";
    }

    switch (root->member->expression.type) {
        case TYPE_DOUBLE: {
            const std::string constant_name = CONSTANT_NAME + std::to_string(constant_counter++);

            float_constant_map[root->member->expression.value.floating] = constant_name;


            return constant_name + DEFINE_QUAD + std::to_string(root->member->expression.value.floating) + "\n";
        }
        case TYPE_BOOLEAN: {
            const std::string constant_name = CONSTANT_NAME + std::to_string(constant_counter++);

            float_constant_map[root->member->expression.value.boolean] = constant_name;

            std::string result = root->member->expression.value.boolean ? "1" : "0" ;

            return constant_name + DEFINE_BYTE + result + "\n";
        }

        case TYPE_STRING: {
            const std::string constant_name = CONSTANT_NAME + std::to_string(constant_counter++);

            string_constant_map[root->member->expression.value.string] = constant_name;

            return constant_name + DEFINE_BYTE + root->member->expression.value.string + "\n";
        }
        default: return "";
    }
}

std::string create_constants(AstNode *root) {
    std::string declarations;
    AstNode *node = root;
    while (node != nullptr) {
        declarations += handle_constant(node);
        if (node->tree != nullptr) {
            declarations += create_constants(node->tree);
        }
        node = node->next;
    }
    return declarations;
}

void define_location_in_program_for_variable_declarations_in_subprogram(Declaration *root, int shift) {
    if (root == nullptr) {
        return;
    }

    for (const auto declaration_info : root->variable_declarations) {
        declaration_info->is_register = false;
        declaration_info->location_in_stack = shift;
        shift += STACK_BIT_DEPTH;
    }

    for (const auto child_declarations : root->children) {
        define_location_in_program_for_variable_declarations_in_subprogram(child_declarations, shift);
    }
}

void define_location_in_program_for_variable_declarations_in_prolog(const std::list<DeclarationInfo *>& variables) {
    int number_int = 0;
    int number_frac = 0;
    for (const auto declaration_info : variables) {
        switch (declaration_info->user_type) {
            case TYPE_DOUBLE: {
                declaration_info->is_register = true;
                declaration_info->reg = register_frac_list[number_frac++];
                break;
            }
            case TYPE_BOOLEAN: {
                declaration_info->is_register = true;
                declaration_info->reg = register_int_list[number_int++];
                break;
            }
            case TYPE_STRING: {
                declaration_info->is_register = true;
                declaration_info->reg = register_int_list[number_int++];
                break;
            }
            case TYPE_INTEGER: {
                declaration_info->is_register = true;
                declaration_info->reg = register_int_list[number_int++];
                break;
            }
            default: break;
        }
    }
}

void define_location_in_program_for_variable_declarations(Declaration *root) {
    if (root == nullptr) {
        return;
    }

    switch (root->node->non_terminal) {
        case NT_PROGRAM: {
            for (const auto child_declaration : root->children) {
                define_location_in_program_for_variable_declarations(child_declaration);
            }
            break;
        }
        case NT_PROCEDURE:
        case NT_FUNCTION: {
            define_location_in_program_for_variable_declarations_in_prolog(root->variable_declarations);
            for (const auto child_declaration : root->children) {
                define_location_in_program_for_variable_declarations_in_subprogram(child_declaration, STACK_BIT_DEPTH);
            }
            break;
        }
        default: return;
    }
}




void generate_code(AstNode *root) {
    std::ofstream file("../out.asm");

    file << "global _start\n";
    file << "section .text\n";
    file << "_start:\n";

    auto basic_string = create_constants(root);
    define_location_in_program_for_variable_declarations(declaration_root);
    declaration_root->children.begin();
    string_constant_map.clear();
    float_constant_map.clear();
    file.close();
}