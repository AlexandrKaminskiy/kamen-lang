//
// Created by Alexandr Kaminskiy on 21.06.2025.
//

#include "codegen.h"

std::string handle_constant(AstNode *root) {
    if (root->non_terminal != NT_EXPRESSION) {
        return "";
    }
    if (root->member->expression.expression_type != TERMINAL) {
        return "";
    }

    switch (root->member->expression.type) {
        case TYPE_DOUBLE: {
            if (float_constant_map.count(root->member->expression.value.floating)) {
                return "";
            }

            const std::string constant_name = CONSTANT_NAME + std::to_string(constant_counter++);

            float_constant_map[root->member->expression.value.floating] = constant_name;


            return constant_name + DEFINE_QUAD + std::to_string(root->member->expression.value.floating) + "\n";
        }
        case TYPE_BOOLEAN: {
            if (bool_constant_map.count(root->member->expression.value.boolean)) {
                return "";
            }

            const std::string constant_name = CONSTANT_NAME + std::to_string(constant_counter++);

            bool_constant_map[root->member->expression.value.boolean] = constant_name;

            std::string result = root->member->expression.value.boolean ? "1" : "0";

            return constant_name + DEFINE_QUAD + result + "\n";
        }

        case TYPE_STRING: {
            if (string_constant_map.count(root->member->expression.value.string)) {
                return "";
            }
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

    for (const auto declaration_info: root->variable_declarations) {
        declaration_info->is_register = false;
        declaration_info->location_in_stack = shift;
        shift += STACK_BIT_DEPTH;
    }

    for (const auto child_declarations: root->children) {
        define_location_in_program_for_variable_declarations_in_subprogram(child_declarations, shift);
    }
}

void define_location_in_program_for_variable_declarations_in_prolog(const std::list<DeclarationInfo *> &variables) {
    int number_int = 0;
    int number_frac = 0;
    for (const auto declaration_info: variables) {
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
            for (const auto child_declaration: root->children) {
                define_location_in_program_for_variable_declarations(child_declaration);
            }
            break;
        }
        case NT_PROCEDURE:
        case NT_FUNCTION: {
            define_location_in_program_for_variable_declarations_in_prolog(root->variable_declarations);
            for (const auto child_declaration: root->children) {
                define_location_in_program_for_variable_declarations_in_subprogram(child_declaration, STACK_BIT_DEPTH);
            }
            break;
        }
        default: return;
    }
}

std::string no_operands_operation(std::string op) {
    return op + "\n";
}

std::string one_operands_operation(std::string op, std::string operand) {
    return op + " " + operand + "\n";
}

std::string two_operands_operation(std::string op, std::string operand1, std::string operand2) {
    return op + " " + operand1 + ", " + operand2 + "\n";
}

std::string debug_comment(std::string op) {
    return "; " + op + "\n";
}

std::string zero_register(std::string reg) {
    return two_operands_operation(XOR_OP, reg, reg);
}

std::string perform_pop(std::string into) {
    return one_operands_operation(POP_OP, into);
}

std::string perform_push(std::string value, std::string size) {
    return PUSH_OP + " " + size + " " + value + "\n";
}

std::string perform_push(std::string value) {
    return one_operands_operation(PUSH_OP, value);
}

std::string to_location_in_stack(int location) {
    return "[rel BP + " + std::to_string(location) + "]\n";
}

std::string to_location_in_data_segment(std::string label) {
    return "[rel " + label + "]\n";
}

std::string to_location_in_register(std::string reg) {
    return "[" + reg + "]";
}

std::string perform_pop_for_float(std::string into) {
    auto load = two_operands_operation(MOVSD_OP, into, to_location_in_register(RSP_REG));
    auto add_sp = two_operands_operation(ADD_OP, RSP_REG, std::to_string(STACK_BIT_DEPTH));
    return load + add_sp;
}

std::string perform_push_for_float(std::string value) {
    auto sub_sp = two_operands_operation(SUB_OP, RSP_REG, std::to_string(STACK_BIT_DEPTH));
    auto load = two_operands_operation(MOVSD_OP, to_location_in_register(RSP_REG), value);
    return sub_sp + load;
}


std::string handle_expression_terminal(AstNode *current) {
    switch (current->member->expression.type) {
        case TYPE_DOUBLE: {
            return perform_push(
                to_location_in_data_segment(float_constant_map[current->member->expression.value.floating]),
                QWORD_SIZE);
        }
        case TYPE_BOOLEAN: {
            return perform_push(
                to_location_in_data_segment(bool_constant_map[current->member->expression.value.boolean]), QWORD_SIZE);
        }
        case TYPE_STRING: {
            return perform_push(
                to_location_in_data_segment(string_constant_map[current->member->expression.value.string]),
                QWORD_SIZE); // todo QWORD???
        }
        case TYPE_INTEGER: {
            return perform_push(std::to_string(current->member->expression.value.integer));
        }
        default: return "";
    }
}

std::string handle_pop_with_float_in_expression(bool is_int, std::string reg) {
    if (is_int) {
        auto pop_int = perform_pop(RAX_REG);
        auto load = two_operands_operation(CVTSI2SD_OP, reg, RAX_REG);
        return pop_int + load;
    } else {
        return perform_pop_for_float(reg);
    }
}

std::string handle_arith_ops(AstNode *current, std::string op) {
    switch (current->member->expression.type) {
        case TYPE_INTEGER: {
            if (op == "+") {
                auto right = perform_pop(RBX_REG);
                auto left = perform_pop(RAX_REG);
                auto operation = two_operands_operation(ADD_OP, RAX_REG, RBX_REG);
                auto result = perform_push(RAX_REG);
                return (right + left + operation + result);
            }
            if (op == "-") {
                auto right = perform_pop(RBX_REG);
                auto left = perform_pop(RAX_REG);
                auto operation = two_operands_operation(SUB_OP, RAX_REG, RBX_REG);
                auto result = perform_push(RAX_REG);
                return (right + left + operation + result);
            }
            if (op == "*") {
                auto right = perform_pop(RBX_REG);
                auto left = perform_pop(RAX_REG);
                auto operation = two_operands_operation(IMUL_OP, RAX_REG, RBX_REG);
                auto result = perform_push(RAX_REG);
                return (right + left + operation + result);
            }
            if (op == "/") {
                auto right = perform_pop(RBX_REG);
                auto left = perform_pop(RAX_REG);
                auto zero_rdx = zero_register(RDX_REG);
                auto cqo = no_operands_operation(CQO_OP);
                auto operation = one_operands_operation(IDIV_OP, RBX_REG);
                auto result = perform_push(RAX_REG);
                return (right + left + zero_rdx + cqo + operation + result);
            }
            if (op == "mod") {
                auto right = perform_pop(RBX_REG);
                auto left = perform_pop(RAX_REG);
                auto zero_rdx = zero_register(RDX_REG);
                auto cqo = no_operands_operation(CQO_OP);
                auto operation = one_operands_operation(IDIV_OP, RAX_REG);
                auto result = perform_push(RDX_REG);
                return (right + left + zero_rdx + cqo + operation + result);
            }
            break;
        }
        case TYPE_DOUBLE: {
            bool left_int = current->tree->member->expression.type == TYPE_INTEGER;
            bool right_int = current->tree->next->member->expression.type == TYPE_INTEGER;

            if (op == "+") {
                auto right = handle_pop_with_float_in_expression(right_int, XMM1_REG);
                auto left = handle_pop_with_float_in_expression(left_int, XMM0_REG);
                auto operation = two_operands_operation(ADDSD_OP, XMM0_REG, XMM1_REG);
                auto result = perform_push_for_float(XMM0_REG);
                auto debug_check = debug_comment(two_operands_operation(CVTSD2SI_OP, RAX_REG, XMM0_REG));
                return (right + left + operation + result + debug_check);
            }
            if (op == "-") {
                auto right = handle_pop_with_float_in_expression(right_int, XMM1_REG);
                auto left = handle_pop_with_float_in_expression(left_int, XMM0_REG);
                auto operation = two_operands_operation(SUBSD_OP, XMM0_REG, XMM1_REG);
                auto result = perform_push_for_float(XMM0_REG);
                auto debug_check = debug_comment(two_operands_operation(CVTSD2SI_OP, RAX_REG, XMM0_REG));
                return (right + left + operation + result + debug_check);
            }
            if (op == "*") {
                auto right = handle_pop_with_float_in_expression(right_int, XMM1_REG);
                auto left = handle_pop_with_float_in_expression(left_int, XMM0_REG);
                auto operation = two_operands_operation(MULSD_OP, XMM0_REG, XMM1_REG);
                auto result = perform_push_for_float(XMM0_REG);
                auto debug_check = debug_comment(two_operands_operation(CVTSD2SI_OP, RAX_REG, XMM0_REG));
                return (right + left + operation + result + debug_check);
            }
            if (op == "/") {
                auto right = handle_pop_with_float_in_expression(right_int, XMM1_REG);
                auto left = handle_pop_with_float_in_expression(left_int, XMM0_REG);
                auto operation = two_operands_operation(DIVSD_OP, XMM0_REG, XMM1_REG);
                auto result = perform_push_for_float(XMM0_REG);
                auto debug_check = debug_comment(two_operands_operation(CVTSD2SI_OP, RAX_REG, XMM0_REG));
                return (right + left + operation + result + debug_check);
            }
            break;
        }
        default: break;
    }
}

std::string handle_logic_bi_ops(AstNode *current, std::string op) {
    switch (current->member->expression.type) {
        case TYPE_BOOLEAN: {
            if (op == "&&") {
                auto right = perform_pop(RBX_REG);
                auto left = perform_pop(RAX_REG);
                auto operation = two_operands_operation(AND_OP, RAX_REG, RBX_REG);
                auto result = perform_push(RAX_REG);
                return (right + left + operation + result);
            }
            if (op == "||") {
                auto right = perform_pop(RBX_REG);
                auto left = perform_pop(RAX_REG);
                auto operation = two_operands_operation(OR_OP, RAX_REG, RBX_REG);
                auto result = perform_push(RAX_REG);
                return (right + left + operation + result);
            }
        }
        default: break;
    }
}

std::string handle_un_ops(AstNode *current, char *op) {
    switch (current->member->expression.type) {
        case TYPE_BOOLEAN: {
            if (std::strcmp(op, "!") == 0) {
                auto left = perform_pop(RAX_REG);
                auto operation = two_operands_operation(XOR_OP, RAX_REG, "0x1");
                auto result = perform_push(RAX_REG);
                return (left + operation + result);
            }
        }
        default: break;
    }
}

std::string handle_comp_operators(AstNode *current, std::string op) {
    bool left_int = current->tree->member->expression.type == TYPE_INTEGER;
    bool right_int = current->tree->next->member->expression.type == TYPE_INTEGER;

    if (left_int && right_int) {
        std::string set_result;
        if (op == ">=") {
            set_result = one_operands_operation(SETGE_OP, AL_REG);
        }
        if (op == "<=") {
            set_result = one_operands_operation(SETLE_OP, AL_REG);
        }
        if (op == "<") {
            set_result = one_operands_operation(SETG_OP, AL_REG);
        }
        if (op == ">") {
            set_result = one_operands_operation(SETL_OP, AL_REG);
        }
        auto right = perform_pop(RBX_REG);
        auto left = perform_pop(RAX_REG);
        auto operation = two_operands_operation(CMP_OP, RAX_REG, RBX_REG);
        auto extend = two_operands_operation(MOVZX_OP, RAX_REG, AL_REG);
        auto result = perform_push(RAX_REG);
        return (right + left + operation + set_result + extend + result);
    } else {
        std::string set_result;
        if (op == ">=") {
            set_result = one_operands_operation(SETAE_OP, AL_REG);
        }

        if (op == "<=") {
            set_result = one_operands_operation(SETBE_OP, AL_REG);
        }

        if (op == ">") {
            set_result = one_operands_operation(SETA_OP, AL_REG);
        }

        if (op == "<") {
            set_result = one_operands_operation(SETB_OP, AL_REG);
        }
        auto right = handle_pop_with_float_in_expression(right_int, XMM1_REG);
        auto left = handle_pop_with_float_in_expression(left_int, XMM0_REG);
        auto operation = two_operands_operation(UCOMISD_OP, XMM0_REG, XMM1_REG);
        auto extend = two_operands_operation(MOVZX_OP, RAX_REG, AL_REG);
        auto result = perform_push(RAX_REG);
        return (right + left + operation + set_result + extend + result);
    }
}

std::string handle_eq_operators(AstNode *current, std::string op) {
    bool left_double = current->tree->member->expression.type == TYPE_DOUBLE;
    bool right_double = current->tree->next->member->expression.type == TYPE_DOUBLE;

    if (left_double || right_double) {
        bool left_int = current->tree->member->expression.type == TYPE_INTEGER;
        bool right_int = current->tree->next->member->expression.type == TYPE_INTEGER;
        if (op == "=") {
            auto right = handle_pop_with_float_in_expression(right_int, XMM1_REG);
            auto left = handle_pop_with_float_in_expression(left_int, XMM0_REG);
            auto operation = two_operands_operation(UCOMISD_OP, XMM0_REG, XMM1_REG);
            auto set_result = one_operands_operation(SETE_OP, AL_REG);
            auto extend = two_operands_operation(MOVZX_OP, RAX_REG, AL_REG);
            auto result = perform_push(RAX_REG);
            return (right + left + operation + set_result + extend + result);
        }
    } else {
        if (op == "=") {
            auto right = perform_pop(RBX_REG);
            auto left = perform_pop(RAX_REG);
            auto operation = two_operands_operation(CMP_OP, RAX_REG, RBX_REG);
            auto set_result = one_operands_operation(SETE_OP, AL_REG);
            auto extend = two_operands_operation(MOVZX_OP, RAX_REG, AL_REG);
            auto result = perform_push(RAX_REG);
            return (right + left + operation + set_result + extend + result);
        }
    }
}

std::string handle_expression(AstNode *node) {
    std::string result;
    AstNode *current = node;

    if (current->tree != nullptr) {
        result += handle_expression(current->tree);
    }

    if (current->member->expression.op == nullptr) {
        switch (current->member->expression.expression_type) {
            case TERMINAL: {
                result += handle_expression_terminal(current);
                break;
            }
            case VARIABLE: {
                auto declaration_info = find_declaration(declaration_root, node, current->member->expression.identifier);
                if (declaration_info->is_register) {
                    result += perform_push(declaration_info->reg);
                } else {
                    result += perform_push(to_location_in_stack(declaration_info->location_in_stack));
                }
                break;
            }
            case INVOCATION: break; //todo
        }
    } else {
        if (bi_operators.find(std::string(current->member->expression.op)) != bi_operators.end()) {
            if (arith_operators.find(std::string(current->member->expression.op)) != arith_operators.end()) {
                result += handle_arith_ops(current, current->member->expression.op);
            }
            if (comp_operators.find(std::string(current->member->expression.op)) != comp_operators.end()) {
                result += handle_comp_operators(current, current->member->expression.op);
            }
            if (eq_operators.find(std::string(current->member->expression.op)) != eq_operators.end()) {
                result += handle_eq_operators(current, current->member->expression.op);
            }
            if (logic_bi_operators.find(std::string(current->member->expression.op)) != logic_bi_operators.end()) {
                result += handle_logic_bi_ops(current, current->member->expression.op);
            }
        }
        if (un_operators.find(std::string(current->member->expression.op)) != un_operators.end()) {
            result += handle_un_ops(current, current->member->expression.op);
        }
    }

    if (current->next != nullptr) {
        result += handle_expression(current->next);
    }

    return result;
}

bool handle_non_terminal_operation(AstNode *node, NonTerminal non_terminal) {
    if (non_terminal != NT_EXPRESSION) {
        return false;
    }

    EXPRESSION_LISTING += handle_expression(node);
    return true;
}

void handle_operations(AstNode *root) {
    AstNode *node = root;
    while (node != nullptr) {
        bool handled = handle_non_terminal_operation(node, node->non_terminal);

        if (!handled && node->tree != nullptr) {
            handle_operations(node->tree);
        }
        node = node->next;
    }
}

void generate_code(AstNode *root) {
    std::ofstream file("../out.asm");

    file << "global _start\n";
    file << "section .text\n";
    file << "_start:\n";

    auto basic_string = create_constants(root);
    define_location_in_program_for_variable_declarations(declaration_root);
    handle_operations(root);
    file << EXPRESSION_LISTING;
    file << (".data:\n" + basic_string);

    declaration_root->children.begin();
    string_constant_map.clear();
    float_constant_map.clear();
    file.close();
}
