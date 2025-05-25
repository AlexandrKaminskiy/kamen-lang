#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>

using namespace std;

Declaration *create_declaration(Declaration *parent, AstNode *node);

void make_semantic(AstNode *root);

inline unordered_map<string, set<UserType>> operator_map;