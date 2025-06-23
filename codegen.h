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
inline std::unordered_map<bool, std::string> bool_constant_map;

inline int constant_counter = 0;
inline const std::string CONSTANT_NAME = "__constant__";
inline const std::string DEFINE_BYTE = " db ";
inline const std::string DEFINE_WORD = " dw ";
inline const std::string DEFINE_DOUBLE_WORD = " dd ";
inline const std::string DEFINE_QUAD = " dq ";
inline const int STACK_BIT_DEPTH = 8;

inline std::string EXPRESSION_LISTING;

inline const std::string POP_OP = "POP";
inline const std::string MOV_OP = "MOV";
inline const std::string PUSH_OP = "PUSH";
inline const std::string ADD_OP = "ADD";
inline const std::string SUB_OP = "SUB";
inline const std::string IMUL_OP = "IMUL";
inline const std::string IDIV_OP = "IDIV";

inline const std::string XOR_OP = "XOR";

inline const std::string CQO_OP = "CQO"; // https://www.felixcloutier.com/x86/cwd:cdq:cqo CWD/CDQ/CQO — Convert Word to Doubleword/Convert Doubleword to Quadword

inline const std::string ADDSD_OP = "ADDSD";
inline const std::string SUBSD_OP = "SUBSD";
inline const std::string MULSD_OP = "MULSD";
inline const std::string DIVSD_OP = "DIVSD";
inline const std::string MOVSD_OP = "MOVSD";
inline const std::string CVTSD2SI_OP = "CVTSD2SI"; // https://www.felixcloutier.com/x86/cvtsd2si Convert Scalar Double Precision Floating-Point Value to Doubleword Integer
inline const std::string CVTSI2SD_OP = "CVTSI2SD"; // https://www.felixcloutier.com/x86/cvtsi2sd Convert Doubleword Integer to Scalar Double Precision Floating-Point Value


inline const std::string RAX_REG = "RAX";
inline const std::string RBX_REG = "RBX";
inline const std::string RDX_REG = "RDX";
inline const std::string RSP_REG = "RSP";
inline const std::string RBP_REG = "RBP";

inline const std::string XMM0_REG = "XMM0";
inline const std::string XMM1_REG = "XMM1";
inline const std::string XMM2_REG = "XMM2";

inline const std::string QWORD_SIZE = "QWORD";



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
