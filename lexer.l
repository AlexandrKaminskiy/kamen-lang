%{
typedef struct AstNode AstNode;
#include "parser.tab.hpp"
extern FILE *yyin;
extern int yyparse(void);

bool lex_dbg = false;

int msl_buf_start = 0;
int msl_buf_position = 0;
char msl_buffer[8192];

%}

%x MULTIPLE_STRING

%%
#[^\n]*                                         { if(lex_dbg) printf("ONE STRING COMMENT: %s\n", yytext); }
begin                                           { if(lex_dbg) printf("KEYWORD begin: %s\n", yytext); return BEGIN_KW;}
end                                             { if(lex_dbg) printf("KEYWORD end: %s\n", yytext); return END;}
if                                              { if(lex_dbg) printf("KEYWORD if: %s\n", yytext); return IF;}
else                                            { if(lex_dbg) printf("KEYWORD else: %s\n", yytext); return ELSE;}
in                                              { if(lex_dbg) printf("KEYWORD in: %s\n", yytext); return IN;}
to                                              { if(lex_dbg) printf("KEYWORD in: %s\n", yytext); return TO;}
for                                             { if(lex_dbg) printf("KEYWORD for: %s\n", yytext); return FOR;}
while                                           { if(lex_dbg) printf("KEYWORD while: %s\n", yytext); return WHILE;}
function                                        { if(lex_dbg) printf("KEYWORD function: %s\n", yytext); return FUNCTION;}
procedure                                       { if(lex_dbg) printf("KEYWORD procedure: %s\n", yytext); return PROCEDURE;}
var                                             { if(lex_dbg) printf("KEYWORD var: %s\n", yytext); return VAR;}
return                                          { if(lex_dbg) printf("KEYWORD return: %s\n", yytext); return RETURN;}

Rect                                            { if(lex_dbg) printf("KEYWORD Rect: %s\n", yytext); return RECT;}
Oval                                            { if(lex_dbg) printf("KEYWORD Oval: %s\n", yytext); return OVAL;}
Line                                            { if(lex_dbg) printf("KEYWORD Line: %s\n", yytext); return LINE;}
Polyline                                        { if(lex_dbg) printf("KEYWORD Polyline: %s\n", yytext); return POLYLINE;}
Text                                            { if(lex_dbg) printf("KEYWORD Text: %s\n", yytext); return TEXT;}

E                                               { if(lex_dbg) printf("CONSTANT E: %s\n", yytext); return E;}
PI                                              { if(lex_dbg) printf("CONSTANT PI: %s\n", yytext); return PI;}
true                                            { if(lex_dbg) printf("CONSTANT true: %s\n", yytext); yylval.boolean = strdup(yytext); return TRUE;}
false                                           { if(lex_dbg) printf("CONSTANT false: %s\n", yytext); yylval.boolean = strdup(yytext); return FALSE;}

String                                          { if(lex_dbg) printf("TYPE String: %s\n", yytext); yylval.string = strdup(yytext); return STRING;}
Integer                                         { if(lex_dbg) printf("TYPE Integer: %s\n", yytext); yylval.string = strdup(yytext); return INTEGER;}
Double                                          { if(lex_dbg) printf("TYPE Double: %s\n", yytext); yylval.string = strdup(yytext); return DOUBLE;}
Boolean                                         { if(lex_dbg) printf("TYPE Boolean: %s\n", yytext); yylval.string = strdup(yytext); return BOOLEAN;}
Shape                                           { if(lex_dbg) printf("TYPE Shape: %s\n", yytext); yylval.string = strdup(yytext); return SHAPE;}
Context                                         { if(lex_dbg) printf("TYPE Context: %s\n", yytext); yylval.string = strdup(yytext); return CONTEXT;}

[,]                                             { if(lex_dbg) printf("OPERATOR COMMA : %s\n", yytext); return COMMA;}
[:]                                             { if(lex_dbg) printf("OPERATOR COLON : %s\n", yytext); return COLON;}
[(]                                             { if(lex_dbg) printf("OPEN ROUND BRACKETS : %s\n", yytext); return OPEN_ROUND_BRACKETS;}
[)]                                             { if(lex_dbg) printf("CLOSE ROUND BRACKETS : %s\n", yytext); return CLOSE_ROUND_BRACKETS;}
[[]                                             { if(lex_dbg) printf("OPEN SQUARE BRACKETS : %s\n", yytext); return OPEN_SQUARE_BRACKETS;}
[]]                                             { if(lex_dbg) printf("CLOSE SQUARE BRACKETS : %s\n", yytext); return CLOSE_SQUARE_BRACKETS;}
[:][=]                                          { if(lex_dbg) printf("OPERATOR ASSIGN : %s\n", yytext); return ASSIGN;}

[&][&]                                          { if(lex_dbg) printf("OPERATOR LOGICAL AND : %s\n", yytext); yylval.string = strdup(yytext); return AND;}
[|][|]                                          { if(lex_dbg) printf("OPERATOR LOGICAL OR : %s\n", yytext); yylval.string = strdup(yytext); return OR;}
[!]                                             { if(lex_dbg) printf("OPERATOR LOGICAL NOT : %s\n", yytext); yylval.string = strdup(yytext); return NOT;}

">="                                            { if(lex_dbg) printf("OPERATOR BIGGER OR EQUALS : %s\n", yytext); yylval.string = strdup(yytext); return BIGGER_OR_EQUALS;}
"<="                                            { if(lex_dbg) printf("OPERATOR LESS OR EQUALS : %s\n", yytext); yylval.string = strdup(yytext); return LESS_OR_EQUALS; }
[=]                                             { if(lex_dbg) printf("OPERATOR EQUALS : %s\n", yytext); yylval.string = strdup(yytext); return EQUALS;}
[>]                                             { if(lex_dbg) printf("OPERATOR BIGGER : %s\n", yytext); yylval.string = strdup(yytext); return BIGGER;}
[<]                                             { if(lex_dbg) printf("OPERATOR LESS : %s\n", yytext); yylval.string = strdup(yytext); return LESS;}

[+]                                             { if(lex_dbg) printf("OPERATOR PLUS : %s\n", yytext); yylval.string = strdup(yytext); return PLUS;}
[-]                                             { if(lex_dbg) printf("OPERATOR MINUS : %s\n", yytext); yylval.string = strdup(yytext); return MINUS;}
mod                                             { if(lex_dbg) printf("OPERATOR mod : %s\n", yytext); yylval.string = strdup(yytext); return MOD;}
[*]                                             { if(lex_dbg) printf("OPERATOR MULT : %s\n", yytext); yylval.string = strdup(yytext); return MULT;}
[/]                                             { if(lex_dbg) printf("OPERATOR DIV : %s\n", yytext); yylval.string = strdup(yytext); return DIV;}


"'''"                                           { msl_buffer[msl_buf_position++] = '\"'; BEGIN(MULTIPLE_STRING); }
<MULTIPLE_STRING>.                              {
    msl_buffer[msl_buf_position++] = yytext[0];
}

<MULTIPLE_STRING>"'''"                          {
    char* msl = (char*) malloc(sizeof(char) * (msl_buf_position - msl_buf_start + 2));
    for (int i = 0; i < msl_buf_position - msl_buf_start; i++) {
        msl[i] = msl_buffer[i + msl_buf_start];
    }
    msl[msl_buf_position - msl_buf_start - 1] = '\"';
    msl[msl_buf_position - msl_buf_start] = '\0';
    msl_buf_start = msl_buf_position;
    yylval.string = msl;
    printf("MULTIPLE_STRING %s", msl);
    BEGIN(INITIAL);
    return STRING_LITERAL;
}

["][^"]*["]                                     { if(lex_dbg) printf("STRING LITERAL: %s\n", yytext); yylval.string = strdup(yytext); return STRING_LITERAL;}
[0-9]+                                          { if(lex_dbg) printf("INTEGER NUMBER: %s\n", yytext); yylval.num = atoi(yytext); return INTEGER_NUMBER;}
[0-9]+[.][0-9]+|[0-9]+[.]|[.][0-9]+             { if(lex_dbg) printf("DOUBLE NUMBER: %s\n", yytext); yylval.frac = atof(yytext); return DOUBLE_NUMBER;}
[a-zA-Z_][a-zA-Z_0-9]*                          { if(lex_dbg) printf("IDENTIFIER: %s\n", yytext); yylval.string = strdup(yytext); return IDENTIFIER;}

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