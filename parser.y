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
%token <string> STRING
%token <string> INTEGER
%token <string> FLOAT
%token <string> DOUBLE
%token <string> SHAPE
%token <string> CONTEXT
%token COMMA
%token COLON
%token OPEN_ROUND_BRACKETS
%token CLOSE_ROUND_BRACKETS
%token OPEN_SQUARE_BRACKETS
%token CLOSE_SQUARE_BRACKETS
%token ASSIGN

%token <string> STRING_LITERAL
%token <num> INTEGER_NUMBER
%token <frac> DOUBLE_NUMBER
%token <string> IDENTIFIER

%token <string> NOT
%token <string> AND
%token <string> OR


%token <string> EQUALS
%token <string> BIGGER_OR_EQUALS
%token <string> LESS_OR_EQUALS
%token <string> BIGGER
%token <string> LESS

%token <string> PLUS
%token <string> MINUS
%token <string> MOD
%token <string> MULT
%token <string> DIV

%left OR
%left AND
%left NOT

%left EQUALS
%left BIGGER_OR_EQUALS
%left LESS_OR_EQUALS
%left BIGGER
%left LESS

%left PLUS
%left MINUS
%left MOD
%left MULT
%left DIV

%type <string> var_type
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
%type <node> loop_operator
%type <node> comment
%type <node> invocation
%type <node> enumeration
%type <node> while_loop

%%

program: {  }
      | function program  {  root_node_ptr = add_seq_node($1); print_tree(); printf("End of the program"); }
      | procedure program {  root_node_ptr = add_seq_node($1); print_tree(); printf("End of the program"); }
      ;

function: FUNCTION IDENTIFIER OPEN_ROUND_BRACKETS subprog_params CLOSE_ROUND_BRACKETS COLON var_type BEGIN_KW function_body END {
    printf("End of the function %d\n", $7);
    $$ = add_function_node($2, $7, $4, $9);
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


subprog_params: { printf("End of the subprog_params\n"); $$ = create_node(NT_SUBPROG_PARAMS); }
      | declare_variable COMMA subprog_params { printf("End of the subprog_params\n"); $$ = add_equal_node($3, $1); }
      | declare_variable { printf("End of the subprog_params\n"); $$ = create_nodes(NT_SUBPROG_PARAMS, {$1}); }
      ;

enumeration: { $$ = create_node(NT_ENUMERATION); }
      | expression COMMA enumeration { printf("End of the enumeration\n"); $$ = add_equal_node($3, $1); }
      | expression { printf("End of the enumeration\n"); $$ = create_nodes(NT_ENUMERATION, {$1});}
      ;

function_body: body_list RETURN expression { printf("End of the function_body\n"); $$ = create_nodes(NT_FUNCTION_BODY, {$1, $3}); }
      ;

body_list: { printf("1 End of the body_list"); $$ = create_node(NT_BODY_LIST); }
      | body_list body  { printf("2 End of the body_list"); $$ = add_equal_node($1, $2); }
      ;

body: declare_variable { $$ = create_nodes(NT_BODY, {$1}); }
      | assign_variable { $$ = create_nodes(NT_BODY, {$1}); }
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
    $$ = add_variable_assignation_node($1, $3);
};


declare_variable: VAR IDENTIFIER COLON var_type {
    $$ = add_variable_declaration_node($4, $2);
};

expression: expression PLUS expression { $$ = add_expression_node($1, $3, $2) }
    | expression MINUS expression { $$ = add_expression_node($1, $3, $2) }
    | expression MULT expression { $$ = add_expression_node($1, $3, $2) }
    | expression DIV expression { $$ = add_expression_node($1, $3, $2) }
    | expression MOD expression { $$ = add_expression_node($1, $3, $2) }
    | expression EQUALS expression { $$ = add_expression_node($1, $3, $2) }
    | expression BIGGER_OR_EQUALS expression { $$ = add_expression_node($1, $3, $2) }
    | expression LESS_OR_EQUALS expression { $$ = add_expression_node($1, $3, $2) }
    | expression BIGGER expression { $$ = add_expression_node($1, $3, $2) }
    | expression LESS expression { $$ = add_expression_node($1, $3, $2) }
    | expression AND expression { $$ = add_expression_node($1, $3, $2) }
    | NOT expression { $$ = add_expression_node($2, $1) }
    | expression OR expression { $$ = add_expression_node($1, $3, $2) }
    | INTEGER_NUMBER { $$ = add_expression_node($1); }
    | DOUBLE_NUMBER { $$ = add_expression_node($1); }
    | STRING_LITERAL { $$ = add_expression_node($1, 0); }
    | invocation { $$ = create_nodes(NT_EXPRESSION, {$1}); }
    | OPEN_ROUND_BRACKETS expression CLOSE_ROUND_BRACKETS { $$ = add_expression_node($2); }
    | IDENTIFIER { $$ = add_expression_node($1); }
    ;

loop_operator: while_loop { printf("End of loop\n"); }
      ;

while_loop: WHILE OPEN_ROUND_BRACKETS expression CLOSE_ROUND_BRACKETS BEGIN_KW body_list END { $$ = create_nodes(NT_WHILE_LOOP, {$3, $6}); };

invocation: IDENTIFIER OPEN_ROUND_BRACKETS enumeration CLOSE_ROUND_BRACKETS { printf("End of invocation\n"); $$ = add_invocation($1, $3); };

comment: ONE_STRING_COMMENT { printf("Comment ignoring\n") };

var_type: STRING { $$ = $1; }
        | INTEGER { $$ = $1; }
        | FLOAT { $$ = $1; }
        | DOUBLE { $$ = $1; }
        | SHAPE { $$ = $1; }
        | CONTEXT { $$ = $1; };


%%

void yyerror(char *s) {
    fprintf(stderr, "Error: %s\n", s);
}