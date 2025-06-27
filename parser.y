%{

#include "syntax.h"
#include "semantic.h"
#include "codegen.h"

extern FILE *yyin;
int yyparse(void);
extern int yylex(void);
void yyerror(char *);
%}


%union {
    int    num;
    char*  boolean;
    float  frac;
    char*  string;
    AstNode* node;
}

%token ONE_STRING_COMMENT
%token BEGIN_KW
%token END
%token IF
%token IN
%token TO
%token ELSE
%token FOR
%token WHILE
%token FUNCTION
%token PROCEDURE
%token VAR
%token RETURN
%token <frac> E
%token <frac> PI
%token <boolean> TRUE
%token <boolean> FALSE
%token <string> STRING
%token <string> INTEGER
%token <string> BOOLEAN
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

%token <node> RECT
%token <node> OVAL
%token <node> LINE
%token <node> POLYLINE
%token <node> TEXT

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

%left EQUALS
%left BIGGER_OR_EQUALS
%left LESS_OR_EQUALS
%left BIGGER
%left LESS

%left OR
%left AND
%left NOT

%left PLUS
%left MINUS
%left MOD
%left MULT
%left DIV

%nonassoc IF
%nonassoc ELSE

%type <num> user_var_type
%type <num> system_var_type
%type <node> if_else_statement
%type <node> sub_programs
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
%type <node> comment
%type <node> invocation
%type <node> enumeration
%type <node> while_loop
%type <node> for_loop

%type <node> create_line_node

%%

program: sub_programs {
    root_node_ptr = $1;
    print_tree();
    make_semantic(root_node_ptr);
    print_tree();
    generate_code(root_node_ptr);
}

sub_programs: { printf("End of the sub_program "); $$ = create_node(NT_PROGRAM); }
      | sub_programs function { printf("End of the function "); $$ = add_equal_node($1, $2); }
      | sub_programs procedure { printf("End of the procedure "); $$ = add_equal_node($1, $2); }
      ;

function: FUNCTION IDENTIFIER OPEN_ROUND_BRACKETS subprog_params CLOSE_ROUND_BRACKETS COLON user_var_type BEGIN_KW body_list RETURN expression END {
    $$ = add_function_node($2, (UserType) $7, $4, $9, $11);
};

procedure: PROCEDURE IDENTIFIER OPEN_ROUND_BRACKETS subprog_params CLOSE_ROUND_BRACKETS BEGIN_KW body_list END {
    $$ = add_procedure_node($2, $4, $7);
};


subprog_params: { $$ = create_node(NT_SUBPROG_PARAMS); }
      | declare_variable COMMA subprog_params {  $$ = add_equal_node($3, $1); }
      | declare_variable { $$ = create_nodes(NT_SUBPROG_PARAMS, {$1}); }
      ;

enumeration: { $$ = create_node(NT_ENUMERATION); }
      | expression COMMA enumeration { printf("End of the enumeration\n"); $$ = add_equal_node($3, $1); }
      | expression { printf("End of the enumeration\n"); $$ = create_nodes(NT_ENUMERATION, {$1});}
      ;

function_body: body_list RETURN expression { printf("End of the function_body\n"); $$ = create_nodes(NT_FUNCTION_BODY, {$1, $3}); }
      ;

body_list: { $$ = create_node(NT_BODY_LIST); }
      | body_list body  { $$ = add_equal_node($1, $2); }
      ;

body: declare_variable { $$ = create_nodes(NT_BODY, {$1}); }
      | assign_variable { $$ = create_nodes(NT_BODY, {$1}); }
      | expression { $$ = create_nodes(NT_BODY, {$1}); }
      | while_loop { $$ = create_nodes(NT_BODY, {$1}); }
      | for_loop { $$ = create_nodes(NT_BODY, {$1}); }
      | comment { $$ = nullptr; }
      | if_else_statement { $$ = $$; }
      | invocation { $$ = create_nodes(NT_BODY, {$1}); }
      ;

assign_variable: IDENTIFIER ASSIGN expression { $$ = add_variable_assignation_node($1, $3); }
    | IDENTIFIER ASSIGN create_line_node { $$ = add_variable_assignation_node($1, $3); }
    ;


declare_variable: VAR IDENTIFIER COLON user_var_type { $$ = add_variable_declaration_node(to_user_type((UserType) $4), $2); }
    | VAR IDENTIFIER COLON system_var_type { $$ = add_variable_declaration_node(to_system_type((SystemType) $4), $2); }
    ;

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
    | TRUE { $$ = add_expression_node(to_bool($1)); }
    | FALSE { $$ = add_expression_node(to_bool($1)); }
    | DOUBLE_NUMBER { $$ = add_expression_node($1); }
    | STRING_LITERAL { $$ = add_expression_node($1, 0); }
    | invocation { $$ = add_expression_invocation_node($1); }
    | OPEN_ROUND_BRACKETS expression CLOSE_ROUND_BRACKETS { $$ = add_expression_node($2); }
    | IDENTIFIER { $$ = add_expression_node($1); }
    ;

while_loop: WHILE OPEN_ROUND_BRACKETS expression CLOSE_ROUND_BRACKETS BEGIN_KW body_list END { $$ = create_nodes(NT_WHILE_LOOP, {$3, $6}); };

for_loop: FOR IDENTIFIER IN expression TO expression BEGIN_KW body_list END { $$ = add_for_loop($2, $4, $6, $8); };

invocation: IDENTIFIER OPEN_ROUND_BRACKETS enumeration CLOSE_ROUND_BRACKETS { printf("End of invocation\n"); $$ = add_invocation($1, $3); };

comment: ONE_STRING_COMMENT { printf("Comment ignoring\n") };

user_var_type: STRING { $$ = to_user_type($1); }
        | INTEGER { $$ = to_user_type($1); }
        | DOUBLE { $$ = to_user_type($1); }
        | BOOLEAN { $$ = to_user_type($1); }
        ;

system_var_type: SHAPE { $$ = to_system_type($1); }
        | CONTEXT { $$ = to_system_type($1); }
        ;

create_line_node: LINE OPEN_ROUND_BRACKETS IDENTIFIER COMMA expression COMMA expression CLOSE_ROUND_BRACKETS { $$ = add_create_line_node($3, $5, $7)}

if_else_statement: IF OPEN_ROUND_BRACKETS expression CLOSE_ROUND_BRACKETS BEGIN_KW body_list END %prec IF { $$ = create_nodes(NT_IF_BLOCK, {$3, $6});}
        | IF OPEN_ROUND_BRACKETS expression CLOSE_ROUND_BRACKETS BEGIN_KW body_list END ELSE if_else_statement { $$ = create_nodes(NT_IF_BLOCK, {$3, $6, $9});}
        | IF OPEN_ROUND_BRACKETS expression CLOSE_ROUND_BRACKETS BEGIN_KW body_list END ELSE BEGIN_KW body_list END { $$ = create_nodes(NT_IF_BLOCK, {$3, $6, $10});}
        ;


%%

void yyerror(char *s) {
    fprintf(stderr, "Error: %s\n", s);
}