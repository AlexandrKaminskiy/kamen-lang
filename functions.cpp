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



AstNode* create_node(const NonTerminal non_terminal) {
  auto* node_ptr = new AstNode();
  node_ptr->non_terminal = non_terminal;
  return node_ptr;
}

AstNode* add_seq_node(AstNode* what) {
  auto* node_ptr = new AstNode();
  node_ptr->next = what;
  return node_ptr;
}


void print_tree() {
  cout << "Printing tree" << endl;
  _print_tree(root_node_ptr, "");
}

void _print_tree(AstNode *root, std::string indent) {
  if (root != nullptr) {
    cout << indent
         << root->member.variable_declaration.type
         << " "
         << root->member.variable_declaration.name
         << endl;
  }
}

AstNode *create_function_node(AstNode *subprog_params, AstNode *function_body) {
  AstNode* root = create_node(NT_FUNCTION);
  AstNode* node0 = subprog_params;
  AstNode* node1 = function_body;
  root->tree = node0;
  node0->next = node1;
  return root;
}

AstNode* add_body_node(AstNode *body) {
  AstNode* root = create_node(NT_BODY);
  root->tree = body;
  return root;
}

AstNode *create_subprog_param_node() {
  AstNode* root = create_node(NT_SUBPROG_PARAMS);
  return root;
}

AstNode* add_variable_declaration_node(const int type, const std::string name) {
  AstNode* root = create_node(NT_DECLARE_VARIABLE);
  const VariableDeclaration variable = {type, name.c_str()};
  root->member.variable_declaration = variable;
  cout << "adding declaration node " << name << endl;
  return root;
}

AstNode* add_variable_assignation_node(const std::string name, const Value value) {
  AstNode* root = create_node(NT_ASSIGN_VARIABLE);
  const VariableAssignation variable = {name.c_str(), value};
  root->member.variable_assignation = variable;
  cout << "adding assignation node " << name << endl;
  return root;
}

AstNode* add_expression_node() {
  return create_node(NT_EXPRESSION);
}


AstNode* create_nodes(NonTerminal non_terminal, std::initializer_list<AstNode*> nodes) {
  AstNode* root = create_node(non_terminal);
  root->tree = *nodes.begin();
  AstNode* prev_node = root->tree;
  int i = 0;
  for (AstNode* node : nodes) {
    if (i++ == 0) {
      continue;
    }
    prev_node->next = node;
    prev_node = node;
  }
  return root;
}

int main() {
  AstNode* tree = create_nodes(NT_SUBPROG_PARAMS, {create_node(NT_SUBPROG_PARAMS), create_node(NT_EXPRESSION), create_node(NT_BODY)});
  cout << endl;
}