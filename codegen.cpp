//
// Created by Alexandr Kaminskiy on 21.06.2025.
//

#include "codegen.h"

void generate_code() {
    std::ofstream file("../out.asm");

    file << "global _start\n";
    file << "section .text\n";
    file << "_start:\n";


    file.close();
}