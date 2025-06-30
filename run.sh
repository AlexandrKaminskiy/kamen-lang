cd build_
cp -r .. .

bison -d parser.y
flex lexer.l
g++ -g -o calc example.tab.c lex.yy.c codegen.cpp syntax.cpp types.cpp semantic.cpp -L/opt/homebrew/opt/flex/lib -lfl
ls

./calc ../test.txt