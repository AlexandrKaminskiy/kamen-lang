#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>


using namespace std;

Declaration *create_declaration(Declaration *parent);

void make_semantic(AstNode *root);