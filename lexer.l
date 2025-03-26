%{
#include <stdio.h>
%}

%%
#[^\n]*                                         { printf("ONE STRING COMMENT: %s\n", yytext); }
begin                                           { printf("KEYWORD begin: %s\n", yytext); }
end                                             { printf("KEYWORD end: %s\n", yytext); }
if                                              { printf("KEYWORD if: %s\n", yytext); }
else                                            { printf("KEYWORD else: %s\n", yytext); }
for                                             { printf("KEYWORD for: %s\n", yytext); }
while                                           { printf("KEYWORD while: %s\n", yytext); }
function                                        { printf("KEYWORD function: %s\n", yytext); }
var                                             { printf("KEYWORD var: %s\n", yytext); }

E                                               { printf("CONSTANT E: %s\n", yytext); }
PI                                              { printf("CONSTANT PI: %s\n", yytext); }
true                                            { printf("CONSTANT true: %s\n", yytext); }
false                                           { printf("CONSTANT false: %s\n", yytext); }

String                                          { printf("TYPE String: %s\n", yytext); }
Integer                                         { printf("TYPE Integer: %s\n", yytext); }
Float                                           { printf("TYPE Float: %s\n", yytext); }
Double                                          { printf("TYPE Double: %s\n", yytext); }
Shape                                           { printf("TYPE Shape: %s\n", yytext); }
Context                                         { printf("TYPE Context: %s\n", yytext); }

[,]                                             { printf("OPERATOR COMMA : %s\n", yytext); }
[:]                                             { printf("OPERATOR COLON : %s\n", yytext); }
[(]                                             { printf("OPEN ROUND BRACKETS : %s\n", yytext); }
[)]                                             { printf("CLOSE ROUND BRACKETS : %s\n", yytext); }
[[]                                             { printf("OPEN SQUARE BRACKETS : %s\n", yytext); }
[]]                                             { printf("CLOSE SQUARE BRACKETS : %s\n", yytext); }
[&][&]                                          { printf("OPERATOR LOGICAL AND : %s\n", yytext); }
[|][|]                                          { printf("OPERATOR LOGICAL OR : %s\n", yytext); }
[!]                                             { printf("OPERATOR LOGICAL NOT : %s\n", yytext); }
mod                                             { printf("OPERATOR mod : %s\n", yytext); }
[:][=]                                          { printf("OPERATOR ASSIGN : %s\n", yytext); }
[*]                                             { printf("OPERATOR MULT : %s\n", yytext); }
[=]                                             { printf("OPERATOR EQUALS : %s\n", yytext); }
[-]                                             { printf("OPERATOR MINUS : %s\n", yytext); }
[/]                                             { printf("OPERATOR DIV : %s\n", yytext); }
[+]                                             { printf("OPERATOR PLUS : %s\n", yytext); }

["][^"]*["]                                     { printf("STRING LITERAL: %s\n", yytext); }
[0-9]+                                          { printf("INTEGER NUMBER: %s\n", yytext); }
[0-9]+[.][0-9]+|[0-9]+[.]|[.][0-9]+             { printf("DOUBLE NUMBER: %s\n", yytext); }
[a-zA-Z_][a-zA-Z_0-9]*                          { printf("IDENTIFIER: %s\n", yytext); }


.                                               {  }
%%

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Ошибка открытия файла");
            return 1;
        }
    }

    yylex();

    if (yyin) fclose(yyin);
    return 0;
}

int yywrap() { return 1; }