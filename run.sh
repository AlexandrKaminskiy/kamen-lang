rm ./lex.yy.c
rm ./lexer
flex lexer.l
gcc lex.yy.c -o lexer -ll
./lexer test.kamen