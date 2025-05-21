#include "semantic.h"

using namespace std;

void handle_non_terminal(AstNode *node, NonTerminal non_terminal) {
    switch (non_terminal) {
        case NT_ASSIGN_VARIABLE: return;
        case NT_DECLARE_VARIABLE: return;
    }
}

void go_through_tree(AstNode *root) {
    AstNode *node = root;
    while (node != nullptr) {
        handle_non_terminal(node, node->non_terminal);
        if (node->tree != nullptr) {
            go_through_tree(node->tree);
        }
        node = node->next;
    }
}

void make_semantic(AstNode *root) {

}
