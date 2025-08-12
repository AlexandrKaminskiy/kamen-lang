#!/bin/sh
set -e 

SRC_DIR="$(pwd)"
BUILD_DIR="$SRC_DIR/build"

mkdir -p "$BUILD_DIR"

echo "Generating parser"
bison -d "$SRC_DIR/parser.y" -o "$BUILD_DIR/parser.tab.cpp"

echo "Generating lexer"
flex -o "$BUILD_DIR/lex.yy.cpp" "$SRC_DIR/lexer.l"

echo "Compilation"
g++ -std=c++17 -g \
    "$BUILD_DIR/parser.tab.cpp" \
    "$BUILD_DIR/lex.yy.cpp" \
    "$SRC_DIR/syntax.cpp" \
    "$SRC_DIR/semantic.cpp" \
    "$SRC_DIR/types.cpp" \
    "$SRC_DIR/codegen.cpp" \
    -I"$SRC_DIR" \
    -I"$BUILD_DIR" \
    -o "$BUILD_DIR/kl"

mv ./build/kl .
