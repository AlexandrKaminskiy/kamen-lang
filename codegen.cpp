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

            return constant_name + DEFINE_BYTE + root->member->expression.value.string + ", 0\n";
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

int define_location_in_program_for_variable_declarations_in_subprogram(Declaration *root, int shift) {
    if (root == nullptr) {
        return 0;
    }

    int max_stack_depth = 0;
    for (const auto declaration_info: root->variable_declarations) {
        declaration_info->is_register = false;
        declaration_info->location_in_stack = shift;
        shift += STACK_BIT_DEPTH;
    }

    max_stack_depth += shift;

    int max_depth_in_child = 0;
    for (const auto child_declarations: root->children) {
        int depth = define_location_in_program_for_variable_declarations_in_subprogram(child_declarations, shift);
        if (depth > max_depth_in_child) {
            max_depth_in_child = depth;
        }
    }
    max_stack_depth += max_depth_in_child;
    return max_stack_depth;
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
            int max_depth = 0;
            for (const auto child_declaration: root->children) {
                int depth = define_location_in_program_for_variable_declarations_in_subprogram(
                    child_declaration, STACK_BIT_DEPTH);
                if (depth > max_depth) {
                    max_depth = depth;
                }
            }
            root->max_stack_depth = max_depth;
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
    stack_shift += STACK_BIT_DEPTH;
    return one_operands_operation(POP_OP, into);
}

std::string perform_push(std::string value, std::string size) {
    stack_shift -= STACK_BIT_DEPTH;
    return PUSH_OP + " " + size + " " + value + "\n";
}

std::string perform_push(std::string value) {
    stack_shift -= STACK_BIT_DEPTH;
    return one_operands_operation(PUSH_OP, value);
}

std::string to_location_in_stack(int location) {
    return "[" + RBP_REG + " - (" + std::to_string(location) + ")]"; //todo rewrite if more than 6 params
}

std::string to_location_in_data_segment(std::string label) {
    return "[rel " + label + "]\n";
}

std::string to_location_in_register(std::string reg) {
    return "[" + reg + "]";
}

std::string create_label() {
    return LABEL_NAME_PREFIX + std::to_string(label_counter++);
}

std::string create_subprog_label(std::string label) {
    return "_" + label;
}

std::string perform_pop_for_float(std::string into) {
    stack_shift += STACK_BIT_DEPTH;
    auto load = two_operands_operation(MOVSD_OP, into, to_location_in_register(RSP_REG));
    auto add_sp = two_operands_operation(ADD_OP, RSP_REG, std::to_string(STACK_BIT_DEPTH));
    return load + add_sp;
}

std::string perform_push_for_float(std::string value) {
    stack_shift -= STACK_BIT_DEPTH;
    auto sub_sp = two_operands_operation(SUB_OP, RSP_REG, std::to_string(STACK_BIT_DEPTH));
    auto load = two_operands_operation(MOVSD_OP, to_location_in_register(RSP_REG), value);
    return sub_sp + load;
}

std::string handle_invocation(AstNode *node) {
    std::string subprog_label;
    auto it = built_in_function_labels.find(std::string(node->member->invocation.identifier));
    if (it != built_in_function_labels.end()) {
        subprog_label = it->second;
    } else {
        subprog_label = subprog_label_map[std::string(node->member->invocation.identifier)];
    }

    auto param = node->tree->tree;

    int float_params = 0;
    int int_params = 0;

    std::string stack_align;
    std::string stack_revert;
    std::string params_set;

    while (param != nullptr) {
        params_set += handle_expression(param, false);
        auto expr_type = param->member->expression.type;
        if (expr_type == TYPE_DOUBLE) {
            params_set += perform_pop_for_float(register_frac_list[float_params]);
            float_params++;
        } else {
            params_set += perform_pop(register_int_list[int_params]);
            int_params++;
        }
        param = param->next;
    }

    bool is_stack_aligned = stack_shift % (STACK_BIT_DEPTH * 2) == 0; // 16 byte align

    if (!is_stack_aligned) {
        stack_align += two_operands_operation(SUB_OP, RSP_REG, std::to_string(STACK_BIT_DEPTH));
    }

    auto invocation = one_operands_operation(CALL_OP, subprog_label);

    if (!is_stack_aligned) {
        stack_revert += two_operands_operation(ADD_OP, RSP_REG, std::to_string(STACK_BIT_DEPTH));
    }

    return params_set + stack_align + invocation + stack_revert;
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
            auto get_string = two_operands_operation(LEA_OP, RAX_REG, to_location_in_data_segment(string_constant_map[current->member->expression.value.string]));

            return get_string + perform_push(RAX_REG);
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
            if (op == "mod") {//todo fix
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
            set_result = one_operands_operation(SETL_OP, AL_REG);
        }
        if (op == ">") {
            set_result = one_operands_operation(SETG_OP, AL_REG);
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

std::string handle_expression(AstNode *node, bool handle_next) {
    std::string result;
    AstNode *current = node;

    if (current->tree != nullptr && current->member->expression.expression_type != INVOCATION) {
        result += handle_expression(current->tree, true);
    }

    if (current->member->expression.op == nullptr) {
        switch (current->member->expression.expression_type) {
            case TERMINAL: {
                result += handle_expression_terminal(current);
                break;
            }
            case VARIABLE: {
                auto declaration_info =
                        find_declaration(declaration_root, current, current->member->expression.identifier);
                if (declaration_info->is_register) {
                    if (declaration_info->user_type == TYPE_DOUBLE) {
                        result += perform_push_for_float(declaration_info->reg);
                    } else {
                        result += perform_push(declaration_info->reg);
                    }
                } else {
                    result += perform_push(to_location_in_stack(declaration_info->location_in_stack), QWORD_SIZE);
                }
                break;
            }
            case INVOCATION: {
                result += handle_invocation(current->tree);
                if (current->member->expression.type == TYPE_DOUBLE) {
                    result += perform_push_for_float(XMM0_REG);
                } else {
                    result += perform_push(RAX_REG);
                }
                break;
            }
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

    if (current->next != nullptr && handle_next) {
        result += handle_expression(current->next, true);
    }

    return result;
}

std::string handle_assign_variable(AstNode *node) {
    auto declaration_info = find_declaration(declaration_root, node, node->member->variable_assignation.name);
    auto expression = node->tree;
    bool handled;
    auto expression_handle = handle_non_terminal_operation(expression, expression->non_terminal, &handled);
    auto get_result = perform_pop(RAX_REG);

    std::string save_result;
    if (declaration_info->is_register) {
        save_result = two_operands_operation(MOV_OP, declaration_info->reg, RAX_REG);
    } else {
        save_result = two_operands_operation(MOV_OP, to_location_in_stack(declaration_info->location_in_stack),
                                             RAX_REG);
    }
    return expression_handle + get_result + save_result;
}

std::string handle_return_expression(AstNode *node) {
    std::string save_result_expression;

    if (node->member->expression.type == TYPE_DOUBLE) {
        save_result_expression = perform_pop_for_float(XMM0_REG);
    } else {
        save_result_expression = perform_pop(RAX_REG);
    }
    auto return_expr = handle_expression(node, true);

    return return_expr + save_result_expression;
}

std::string handle_subprogram(AstNode *node, bool is_procedure) {
    auto root = declaration_root->children.front();
    Declaration* subprog_declaration = nullptr;
    for (auto declaration : root->children) {
        if (declaration->node == node) {
            subprog_declaration = declaration;
            break;
        }
    }

    int max_stack_depth = subprog_declaration->max_stack_depth;
    int addition_to_stack = max_stack_depth + max_stack_depth % (STACK_BIT_DEPTH * 2);
    auto body = node->tree->next;
    auto return_expr = node->tree->next->next;

    std::string subprog_label;
    if (is_procedure) {
        subprog_label = create_subprog_label(node->member->procedure_declaration.name);
        subprog_label_map[node->member->procedure_declaration.name] = subprog_label;
        subprog_label += ":\n";
    } else {
        subprog_label = create_subprog_label(node->member->function_declaration.name);
        subprog_label_map[node->member->function_declaration.name] = subprog_label;
        subprog_label += ":\n";
    }

    auto save_base_ptr = perform_push(RBP_REG);
    auto set_base_ptr = two_operands_operation(MOV_OP, RBP_REG, RSP_REG);
    auto align_stack_ptr = two_operands_operation(SUB_OP, RSP_REG, std::to_string(addition_to_stack)); // System V AMD 64 requirement

    auto subprog_body = handle_operations(body);

    std::string return_expression;
    if (is_procedure) {
        return_expression = "";
    } else {
        return_expression = handle_return_expression(return_expr);
    }

    auto restore_stack_ptr = two_operands_operation(MOV_OP, RSP_REG, RBP_REG);
    auto restore_base_ptr = perform_pop(RBP_REG);
    auto transfer_control = no_operands_operation(RET_OP);

    return subprog_label
        + save_base_ptr + set_base_ptr + align_stack_ptr
        + subprog_body
        + return_expression
        + restore_stack_ptr + restore_base_ptr + transfer_control;
}

std::string handle_if_else_block(AstNode *node) {
    std::string result;

    auto else_label = create_label();
    auto post_if_label = create_label();

    auto condition_calculation = handle_expression(node->tree, false);
    auto get_condition_result = perform_pop(RAX_REG);
    auto compare_result = two_operands_operation(CMP_OP, RAX_REG, "0");
    auto jump_to_post_if_in_if_else = one_operands_operation(JMP_OP, post_if_label);
    auto jump_to_post_if = one_operands_operation(JE_OP, post_if_label);

    AstNode* then_block = node->tree->next;
    AstNode* else_block = then_block->next;

    std::string then_branch = handle_operations(then_block);
    std::string else_branch = handle_operations(else_block);


    bool has_else_block = else_block != nullptr;


    if (has_else_block) {
        auto jump_to_else_condition = one_operands_operation(JE_OP, else_label);
        return condition_calculation + get_condition_result
            + compare_result
            + jump_to_else_condition
            + then_branch
            + jump_to_post_if_in_if_else
            + else_label + ":\n"
            + else_branch
            + post_if_label + ":\n";
    }

    return condition_calculation + get_condition_result
           + compare_result
           + jump_to_post_if
           + then_branch
           + post_if_label + ":\n";
}

std::string handle_while_block(AstNode *node) {
    auto while_label = create_label();
    auto post_while_label = create_label();

    auto condition_calculation = handle_expression(node->tree, false);
    auto get_condition_result = perform_pop(RAX_REG);
    auto compare_result = two_operands_operation(CMP_OP, RAX_REG, "0");
    auto jump_to_post_while = one_operands_operation(JE_OP, post_while_label);

    auto jump_to_while_label = one_operands_operation(JMP_OP, while_label);

    AstNode* while_block = node->tree->next;

    std::string while_branch = handle_operations(while_block);

    return while_label + ":\n"
           + condition_calculation + get_condition_result
           + compare_result
           + jump_to_post_while
           + while_branch
           + jump_to_while_label
           + post_while_label + ":\n";
}

std::string handle_for_block(AstNode * node) {

    auto declaration_info = find_declaration(declaration_root, node, node->member->for_loop.name);
    auto from_expr = node->tree->member->expression.value.integer;
    auto to_expr = node->tree->next->member->expression.value.integer;
    auto save_result = two_operands_operation(
        MOV_OP,
        QWORD_SIZE + to_location_in_stack(declaration_info->location_in_stack),
        std::to_string(from_expr)
    );


    auto for_label = create_label();
    auto post_for_label = create_label();

    auto condition_calculation = two_operands_operation(
        CMP_OP,
        QWORD_SIZE + to_location_in_stack(declaration_info->location_in_stack),
        std::to_string(to_expr)
    );

    auto jump_to_post_for = one_operands_operation(JG_OP, post_for_label);

    auto jump_to_for_label = one_operands_operation(JMP_OP, for_label);

    AstNode* for_block = node->tree->next->next;

    std::string for_branch = handle_operations(for_block);
    std::string inc_loop_var = one_operands_operation(INC_OP,  QWORD_SIZE + to_location_in_stack(declaration_info->location_in_stack));

    return save_result
        + for_label + ":\n"
        + condition_calculation
        + jump_to_post_for
        + for_branch
        + inc_loop_var
        + jump_to_for_label
        + post_for_label + ":\n";
}

std::string handle_non_terminal_operation(AstNode *node, NonTerminal non_terminal, bool *handled) {
    switch (non_terminal) {
        case NT_PROCEDURE: {
            *handled = true;
            return handle_subprogram(node, true);
        }
        case NT_FUNCTION: {
            *handled = true;
            return handle_subprogram(node, false);
        }
        case NT_ASSIGN_VARIABLE: {
            *handled = true;
            return handle_assign_variable(node);
        }
        case NT_EXPRESSION: {
            *handled = true;
            return handle_expression(node, true);
        }
        case NT_INVOCATION: {
            *handled = true;
            return handle_invocation(node);
        }
        case NT_IF_BLOCK: {
            *handled = true;
            return handle_if_else_block(node);
        }
        case NT_WHILE_LOOP: {
            *handled = true;
            return handle_while_block(node);
        }
        case NT_FOR_LOOP: {
            *handled = true;
            return handle_for_block(node);
        }
        default: {
            *handled = false;
            return "";
        }
    }
}

std::string handle_operations(AstNode *root) {
    AstNode *node = root;
    std::string result;
    while (node != nullptr) {
        bool handled;
        result += handle_non_terminal_operation(node, node->non_terminal, &handled);

        if (!handled && node->tree != nullptr) {
            result += handle_operations(node->tree);
        }
        if (check_function_and_return_stmt(node)) {
            break;
        }
        if (check_several_body_lists(node)) {
            break;
        }
        node = node->next;
    }
    return result;
}

std::string add_extern_decl() {
    std::string result;
    for (auto bif: built_in_functions) {
        result += "extern " + bif.first + "\n";
    }
    return result;
}
void generate_code(AstNode *root) {
    std::ofstream file("out.asm");
    file << add_extern_decl();
    file << "global main\n";
    file << "section .text\n";
    file << "main:\n";
    file << "CALL _entrypoint\n";
    file << "JMP _termination\n";

    auto constant_string = create_constants(root);
    define_location_in_program_for_variable_declarations(declaration_root);
    file << handle_operations(root);
    // file << EXPRESSION_LISTING;
    file << "_termination:\n";
    file << "MOV RAX, 60\n";
    file << "XOR RDI, RDI\n";
    file << "SYSCALL\n";

    file << (".data:\n" + constant_string);

    declaration_root->children.begin();
    string_constant_map.clear();
    float_constant_map.clear();
    file.close();
}