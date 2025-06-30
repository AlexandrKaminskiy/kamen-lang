set -e
nasm -f elf64 call.asm -o call.o
g++ -c extern.cpp -o extern.o $(pkg-config --cflags cairo)
g++ -o call call.o extern.o $(pkg-config --libs cairo)