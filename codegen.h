//
// Created by Alexandr Kaminskiy on 21.06.2025.
//

#ifndef CODEGEN_H
#define CODEGEN_H

#include <iostream>
#include <fstream>
#include <stack>
#include <types.h>

inline std::unordered_map<std::string, std::string> string_constant_map;
inline std::unordered_map<float, std::string> float_constant_map;

inline int constant_counter = 0;
inline const std::string CONSTANT_NAME = "__constant__";
inline const std::string DEFINE_BYTE = " db ";
inline const std::string DEFINE_WORD = " dw ";
inline const std::string DEFINE_DOUBLE_WORD = " dd ";
inline const std::string DEFINE_QUAD = " dq ";
inline const int STACK_BIT_DEPTH = 8;

const std::array<std::string, 6> register_int_list = {
    "RDI",
    "RSI",
    "RDX",
    "RCX",
    "R8",
    "R9",
};

const std::array<std::string, 8> register_frac_list = {
    "XMM0",
    "XMM1",
    "XMM2",
    "XMM3",
    "XMM4",
    "XMM5",
    "XMM6",
    "XMM7"
};


std::string create_constants(AstNode *root);

void generate_code(AstNode *root);
#endif //CODEGEN_H
