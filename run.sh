sh clean.sh
cd build
bison -d ../example.y
flex ../example.l
g++ -g -o calc example.tab.c lex.yy.c ../functions.cpp -L/opt/homebrew/opt/flex/lib -lfl
ls

./calc ../test.txt