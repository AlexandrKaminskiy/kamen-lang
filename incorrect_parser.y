%{
#include <stdio.h>
int yylex(void);
void yyerror(char *);
%}


%union {
    int    num;
    float  frac;
    char*  string;
}

%token <string> ONE_STRING_COMMENT
%token BEGIN
%token END
%token IF
%token ELSE
%token FOR
%token WHILE
%token FUNCTION
%token VAR
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
%token IDENTIFIER

%token NOT
%token AND
%token OR
%token MOD
%token MULT
%token DIV
%token PLUS
%token MINUS

%left '+'

%type <num> expr

%%
program: { printf("End of the program"); }
      | function program  {   }
      | procedure program {   }
      ;

expr: expr '+' expr   { $$ = $1 + $3; printf("%d\n", $$); }
      | NUMBER          { $$ = $1; }
      ;

function: FUNCTION IDENTIFIER OPEN_ROUND_BRACKETS subprog_params CLOSE_ROUND_BRACKETS COLON type BEGIN function_body END { printf("End of the function"); };

procedure: PROCEDURE IDENTIFIER OPEN_ROUND_BRACKETS subprog_params CLOSE_ROUND_BRACKETS COLON type BEGIN procedure_body END { printf("End of the procedure"); };

subprog_params: { printf("End of the subprog_params"); }
      | describe_variable COMMA subprog_params
      | describe_variable
      ;

function_body:  { printf("End of the function_body"); }
      | function_body_list RETURN IDENTIFIER
      ;

function_body_list:  { printf("End of the function_body_list"); }
      | function_body_list body
      ;

procedure_body:  { printf("End of the function_body_list"); }
      | procedure_body body
      ;

body: declare_variable
      | assign_variable
      | condition_operator
      | expression
      | loop
      | comment
      | invocation
      ;

declare_variable: VAR describe_variable ASSIGN expression
      | VAR describe_variable
      ;

describe_variable: IDENTIFIER COLON type;

condition_operator: IF condition_expression BEGIN body END;

loop_operator: for_loop
      | while_loop
      ;

enumeration:
      | assignator
      | assignator COLON
      ;

assign_variable: IDENTIFIER ASSIGN expression;

condition_expression: condition_expression logical_operator condition
      | NOT condition_expression
      | assignator
      | invocation
      | expression
      ;

expression: expression PLUS expression
      | expression MINUS expression
      | expression MULT expression
      | expression DIV expression
      ;

assignator: IDENTIFIER
      | LITERAL
      ;

invocation: IDENTIFIER OPEN_ROUND_BRACKETS CLOSE_ROUND_BRACKETS

literal: STRING_LITERAL | INTEGER_NUMBER | DOUBLE_NUMBER

logical_operator: BIGGER_OR_EQUALS | LESS_OR_EQUALS | EQUALS
%%


void yyerror(char *s) {
    fprintf(stderr, "Error: %s\n", s);
}

int main(void) {
    return yyparse();
}