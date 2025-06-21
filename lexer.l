%{
typedef struct AstNode AstNode;
#include "parser.tab.hpp"
extern FILE *yyin;
extern int yyparse(void);

int msl_buf_start = 0;
int msl_buf_position = 0;
char msl_buffer[8192];

%}

%x MULTIPLE_STRING

%%
#[^\n]*                                         { printf("ONE STRING COMMENT: %s\n", yytext); }
begin                                           { printf("KEYWORD begin: %s\n", yytext); return BEGIN_KW;}
end                                             { printf("KEYWORD end: %s\n", yytext); return END;}
if                                              { printf("KEYWORD if: %s\n", yytext); return IF;}
else                                            { printf("KEYWORD else: %s\n", yytext); return ELSE;}
in                                              { printf("KEYWORD in: %s\n", yytext); return IN;}
to                                              { printf("KEYWORD in: %s\n", yytext); return TO;}
for                                             { printf("KEYWORD for: %s\n", yytext); return FOR;}
while                                           { printf("KEYWORD while: %s\n", yytext); return WHILE;}
function                                        { printf("KEYWORD function: %s\n", yytext); return FUNCTION;}
procedure                                       { printf("KEYWORD procedure: %s\n", yytext); return PROCEDURE;}
var                                             { printf("KEYWORD var: %s\n", yytext); return VAR;}
return                                          { printf("KEYWORD return: %s\n", yytext); return RETURN;}

Rect                                            { printf("KEYWORD Rect: %s\n", yytext); return RECT;}
Oval                                            { printf("KEYWORD Oval: %s\n", yytext); return OVAL;}
Line                                            { printf("KEYWORD Line: %s\n", yytext); return LINE;}
Polyline                                        { printf("KEYWORD Polyline: %s\n", yytext); return POLYLINE;}
Text                                            { printf("KEYWORD Text: %s\n", yytext); return TEXT;}

E                                               { printf("CONSTANT E: %s\n", yytext); return E;}
PI                                              { printf("CONSTANT PI: %s\n", yytext); return PI;}
true                                            { printf("CONSTANT true: %s\n", yytext); yylval.boolean = strdup(yytext); return TRUE;}
false                                           { printf("CONSTANT false: %s\n", yytext); yylval.boolean = strdup(yytext); return FALSE;}

String                                          { printf("TYPE String: %s\n", yytext); yylval.string = strdup(yytext); return STRING;}
Integer                                         { printf("TYPE Integer: %s\n", yytext); yylval.string = strdup(yytext); return INTEGER;}
Double                                          { printf("TYPE Double: %s\n", yytext); yylval.string = strdup(yytext); return DOUBLE;}
Boolean                                         { printf("TYPE Boolean: %s\n", yytext); yylval.string = strdup(yytext); return BOOLEAN;}
Shape                                           { printf("TYPE Shape: %s\n", yytext); yylval.string = strdup(yytext); return SHAPE;}
Context                                         { printf("TYPE Context: %s\n", yytext); yylval.string = strdup(yytext); return CONTEXT;}

[,]                                             { printf("OPERATOR COMMA : %s\n", yytext); return COMMA;}
[:]                                             { printf("OPERATOR COLON : %s\n", yytext); return COLON;}
[(]                                             { printf("OPEN ROUND BRACKETS : %s\n", yytext); return OPEN_ROUND_BRACKETS;}
[)]                                             { printf("CLOSE ROUND BRACKETS : %s\n", yytext); return CLOSE_ROUND_BRACKETS;}
[[]                                             { printf("OPEN SQUARE BRACKETS : %s\n", yytext); return OPEN_SQUARE_BRACKETS;}
[]]                                             { printf("CLOSE SQUARE BRACKETS : %s\n", yytext); return CLOSE_SQUARE_BRACKETS;}
[:][=]                                          { printf("OPERATOR ASSIGN : %s\n", yytext); return ASSIGN;}

[&][&]                                          { printf("OPERATOR LOGICAL AND : %s\n", yytext); yylval.string = strdup(yytext); return AND;}
[|][|]                                          { printf("OPERATOR LOGICAL OR : %s\n", yytext); yylval.string = strdup(yytext); return OR;}
[!]                                             { printf("OPERATOR LOGICAL NOT : %s\n", yytext); yylval.string = strdup(yytext); return NOT;}

">="                                            { printf("OPERATOR BIGGER OR EQUALS : %s\n", yytext); yylval.string = strdup(yytext); return BIGGER_OR_EQUALS;}
"<="                                            { printf("OPERATOR LESS OR EQUALS : %s\n", yytext); yylval.string = strdup(yytext); return LESS_OR_EQUALS; }
[=]                                             { printf("OPERATOR EQUALS : %s\n", yytext); yylval.string = strdup(yytext); return EQUALS;}
[>]                                             { printf("OPERATOR BIGGER : %s\n", yytext); yylval.string = strdup(yytext); return BIGGER;}
[<]                                             { printf("OPERATOR LESS : %s\n", yytext); yylval.string = strdup(yytext); return LESS;}

[+]                                             { printf("OPERATOR PLUS : %s\n", yytext); yylval.string = strdup(yytext); return PLUS;}
[-]                                             { printf("OPERATOR MINUS : %s\n", yytext); yylval.string = strdup(yytext); return MINUS;}
mod                                             { printf("OPERATOR mod : %s\n", yytext); yylval.string = strdup(yytext); return MOD;}
[*]                                             { printf("OPERATOR MULT : %s\n", yytext); yylval.string = strdup(yytext); return MULT;}
[/]                                             { printf("OPERATOR DIV : %s\n", yytext); yylval.string = strdup(yytext); return DIV;}


"'''"                                           { BEGIN(MULTIPLE_STRING); }
<MULTIPLE_STRING>.                              {
    msl_buffer[msl_buf_position++] = yytext[0];
}

<MULTIPLE_STRING>"'''"                          {
    char* msl = (char*) malloc(sizeof(char) * (msl_buf_position - msl_buf_start + 1));
    for (int i = 0; i < msl_buf_position - msl_buf_start; i++) {
        msl[i] = msl_buffer[i + msl_buf_start];
    }
    msl[msl_buf_position - msl_buf_start] = '\0';
    msl_buf_start = msl_buf_position;
    yylval.string = msl;
    printf("MULTIPLE_STRING %s", msl);
    BEGIN(INITIAL);
    return STRING_LITERAL;
}

["][^"]*["]                                     { printf("STRING LITERAL: %s\n", yytext); yylval.string = strdup(yytext); return STRING_LITERAL;}
[0-9]+                                          { printf("INTEGER NUMBER: %s\n", yytext); yylval.num = atoi(yytext); return INTEGER_NUMBER;}
[0-9]+[.][0-9]+|[0-9]+[.]|[.][0-9]+             { printf("DOUBLE NUMBER: %s\n", yytext); yylval.frac = atof(yytext); return DOUBLE_NUMBER;}
[a-zA-Z_][a-zA-Z_0-9]*                          { printf("IDENTIFIER: %s\n", yytext); yylval.string = strdup(yytext); return IDENTIFIER;}

[ \t\n\v\f\r]                                   {  }
.                                               {     fprintf(stderr, "Error! Incorrect symbol: '%s' on line %d\n", yytext, yylineno); YY_FATAL_ERROR("Incorrect symbol"); }

%%


int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        perror("Error opening file");
        return 1;
    }

    yyparse();
    fclose(yyin);
    return 0;
}

int yywrap() { return 1; }