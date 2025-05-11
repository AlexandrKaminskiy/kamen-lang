%{

#include "functions.h"

extern FILE *yyin;
int yyparse(void);
extern int yylex(void);
void yyerror(char *);
%}


%union {
    int    num;
    float  frac;
    char*  string;
    AstNode* node;
}

%token ONE_STRING_COMMENT
%token BEGIN_KW
%token END
%token IF
%token ELSE
%token FOR
%token WHILE
%token FUNCTION
%token PROCEDURE
%token VAR
%token RETURN
%token <frac> E
%token <frac> PI
%token <num> TRUE
%token <num> FALSE
%token STRING
%token INTEGER
%token FLOAT
%token DOUBLE
%token SHAPE
%token CONTEXT
%token COMMA
%token COLON
%token OPEN_ROUND_BRACKETS
%token CLOSE_ROUND_BRACKETS
%token OPEN_SQUARE_BRACKETS
%token CLOSE_SQUARE_BRACKETS
%token ASSIGN
%token BIGGER_OR_EQUALS
%token LESS_OR_EQUALS
%token EQUALS


%token BIGGER
%token LESS
%token <string> STRING_LITERAL
%token <num> INTEGER_NUMBER
%token <frac> DOUBLE_NUMBER
%token <string> IDENTIFIER

%token NOT
%token AND
%token OR

%token <string> PLUS
%token <string> MINUS
%token <string> MOD
%token <string> MULT
%token <string> DIV

%left PLUS
%left MINUS
%left MOD
%left MULT
%left DIV

%type <num> var_type
%type <node> program
%type <node> function
%type <node> procedure
%type <node> subprog_params
%type <node> declare_variable
%type <node> function_body
%type <node> body_list
%type <node> expression
%type <node> body
%type <node> assign_variable
%type <node> condition_expression
%type <node> loop_operator
%type <node> comment
%type <node> invocation
%type <node> while_loop

%type <frac> arith_literal

%%

program: {  }
      | function program  {  root_node_ptr = add_seq_node($1); print_tree(); printf("End of the program"); }
      | procedure program {  root_node_ptr = add_seq_node($1); print_tree(); printf("End of the program"); }
      ;

function: FUNCTION IDENTIFIER OPEN_ROUND_BRACKETS subprog_params CLOSE_ROUND_BRACKETS COLON var_type BEGIN_KW function_body END {
    printf("End of the function %d\n", $7);
    $$ = create_function_node($4, $9);
};

procedure: PROCEDURE IDENTIFIER OPEN_ROUND_BRACKETS subprog_params CLOSE_ROUND_BRACKETS BEGIN_KW body_list END {
    AstNode* root = create_node(NT_PROCEDURE);
    AstNode* node0 = add_seq_node($4);
    AstNode* node1 = add_seq_node($7);
    node0->next = node1;
    root->next = node0;
    printf("End of the procedure\n");
    $$ = root
};


subprog_params: { printf("End of the subprog_params\n"); $$ = create_subprog_param_node(); }
      | declare_variable COMMA subprog_params { printf("End of the subprog_params\n"); $$ = create_subprog_param_node(); }
      | declare_variable { printf("End of the subprog_params\n"); $$ = create_subprog_param_node(); }
      ;


function_body: body_list RETURN expression { printf("End of the function_body\n"); $$ = create_nodes(NT_FUNCTION_BODY, {$1, $3}); }
      ;

body_list: { printf("1 End of the body_list"); $$ = create_node(NT_BODY_LIST); }
      | body_list body  { printf("2 End of the body_list"); $$ = add_equal_node($1, $2); }
      ;

body: declare_variable { $$ = create_nodes(NT_BODY, {$1}); }
      | assign_variable { $$ = create_nodes(NT_BODY, {$1}); }
      | condition_expression
      | expression
      | loop_operator { $$ = create_nodes(NT_BODY, {$1}); }
      | comment { $$ = nullptr; }
      | invocation { $$ = create_nodes(NT_BODY, {$1}); }
      ;

describe_variable: VAR describe_variable ASSIGN expression {
    printf("End of the describe_variable\n");

    Value value;
    value.string = "value";
    //add_variable_declaration_node($3, $1, value);
};


assign_variable: IDENTIFIER ASSIGN expression {
    Value value;
    value.integer = 10;
    $$ = add_variable_assignation_node($1, value);
};


declare_variable: VAR IDENTIFIER COLON var_type {
    $$ = add_variable_declaration_node($4, $2);
};

expression: expression PLUS expression { $$ = add_expression_node($1, $3, $2) }
    | expression MINUS expression { $$ = add_expression_node($1, $3, $2) }
    | expression MULT expression { $$ = add_expression_node($1, $3, $2) }
    | expression DIV expression { $$ = add_expression_node($1, $3, $2) }
    | arith_literal { $$ = add_expression_node($1); }
    | invocation { $$ = create_nodes(NT_EXPRESSION, {$1}); }
    | OPEN_ROUND_BRACKETS expression CLOSE_ROUND_BRACKETS { $$ = add_expression_node($2); }
    | IDENTIFIER
    ;


arith_literal: INTEGER_NUMBER { $$ = (float) $1; }
    | DOUBLE_NUMBER { $$ = $1; }
    ;

loop_operator: while_loop { printf("End of loop\n"); }
      ;

while_loop: WHILE OPEN_ROUND_BRACKETS condition_expression CLOSE_ROUND_BRACKETS BEGIN_KW body_list END { $$ = create_nodes(NT_WHILE_LOOP, {$3, $6}); };

invocation: IDENTIFIER OPEN_ROUND_BRACKETS CLOSE_ROUND_BRACKETS { printf("End of invocation\n"); $$ = create_node(NT_INVOCATION); };

condition_expression: { $$ = create_node(NT_CONDITION_EXPRESSION); };

comment: ONE_STRING_COMMENT { printf("Comment ignoring\n") };

var_type: STRING { $$ = STRING; }
        | INTEGER { $$ = INTEGER; }
        | FLOAT { $$ = FLOAT; }
        | DOUBLE { $$ = DOUBLE; }
        | SHAPE { $$ = SHAPE; }
        | CONTEXT { $$ = CONTEXT; };


%%

void yyerror(char *s) {
    fprintf(stderr, "Error: %s\n", s);
}