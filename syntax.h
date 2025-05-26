#include "types.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

inline AstNode root_node = AstNode();
inline AstNode *root_node_ptr = &root_node;

AstNode *create_node(NonTerminal non_terminal);

AstNode *add_seq_node(AstNode *what);

AstNode *add_variable_declaration_node(std::string type, std::string name);

AstNode *add_variable_assignation_node(std::string name, AstNode *value);

void print_tree();

std::string _print_tree(AstNode *root, std::string indent, std::string string);

AstNode *add_function_node(char *name, UserType return_type, AstNode *subprog_params, AstNode *function_body);

AstNode *add_procedure_node(char *name, AstNode *subprog_params, AstNode *procedure_body);

AstNode *create_subprog_param_node();

AstNode *add_body_node(AstNode *body);

AstNode *create_function_body_node();

AstNode *create_nodes(NonTerminal non_terminal, std::initializer_list<AstNode *> nodes);

AstNode *add_equal_node(AstNode *main, AstNode *node);

AstNode *add_expression_node();

AstNode *add_expression_node(int value);

AstNode *add_expression_invocation_node(AstNode *node);

AstNode *add_expression_node(bool value);

AstNode *add_expression_node(float value);

AstNode *add_expression_node(char* value, int stub); //todo fix

AstNode *add_expression_node(const char *identifier);

AstNode *add_expression_node(AstNode *node);

AstNode *add_expression_node(AstNode *node, char *op);

AstNode *add_expression_node(AstNode *left, AstNode *right, char *op);

AstNode *add_invocation(char *name, AstNode *enumeration);

AstNode *add_for_loop(char *loop_var, AstNode *from, AstNode *to, AstNode *body);

AstNode *add_create_line_node(char *name, AstNode *first, AstNode *second);

AstNode *add_if_statement(AstNode *if_block, AstNode *if_body);

AstNode *add_if_statement(AstNode *if_block, AstNode *if_body, AstNode *else_block, AstNode *else_body);
