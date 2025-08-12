#!/bin/bash
set -e

nasm -f elf64 out.asm -o out.o
g++ -c extern.cpp -o extern.o $(pkg-config --cflags cairo)
echo "Linking"
g++ -o out out.o extern.o $(pkg-config --libs cairo)

./out