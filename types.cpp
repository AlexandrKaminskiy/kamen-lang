#include "types.h"

std::string to_nt_string(const NonTerminal non_terminal) {
    switch (non_terminal) {
        case NT_PROGRAM: return "NT_PROGRAM";
        case NT_SUBPROGRAMS: return "NT_SUBPROGRAMS";
        case NT_FUNCTION: return "NT_FUNCTION";
        case NT_PROCEDURE: return "NT_PROCEDURE";
        case NT_SUBPROG_PARAMS: return "NT_SUBPROG_PARAMS";
        case NT_FUNCTION_BODY: return "NT_FUNCTION_BODY";
        case NT_BODY_LIST: return "NT_BODY_LIST";
        case NT_BODY: return "NT_BODY";
        case NT_ASSIGN_VARIABLE: return "NT_ASSIGN_VARIABLE";
        case NT_DECLARE_VARIABLE: return "NT_DECLARE_VARIABLE";
        case NT_EXPRESSION: return "NT_EXPRESSION";
        case NT_WHILE_LOOP: return "NT_WHILE_LOOP";
        case NT_FOR_LOOP: return "NT_FOR_LOOP";
        case NT_INVOCATION: return "NT_INVOCATION";
        case NT_ENUMERATION: return "NT_ENUMERATION";
        case NT_CREATE_LINE: return "NT_CREATE_LINE";
        case NT_IF_BLOCK: return "NT_IF_BLOCK";
    }
    return "";
}

std::string to_user_type(UserType user_type) {
    switch (user_type) {
        case TYPE_DOUBLE: return "Double";
        case TYPE_INTEGER: return "Integer";
        case TYPE_BOOLEAN: return "Boolean";
        case TYPE_STRING: return "String";
        default: return "U_Incorrect";
    }
}

int to_type(std::string string) {
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
    if (string == "Shape") {
        return TYPE_SHAPE;
    }
    if (string == "Context") {
        return TYPE_CONTEXT;
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

bool to_bool(char* string) {
    if (strcmp(string, "true") == 0) {
        return true;
    }
    return false;
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

DeclarationInfo *find_declaration(Declaration* current, AstNode* node_to_find, std::string name_to_find) {
    auto declaration_info = find_var(name_to_find, current->variable_declarations);

    if (declaration_info != nullptr && in_correct_node(current->node, node_to_find)) {
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