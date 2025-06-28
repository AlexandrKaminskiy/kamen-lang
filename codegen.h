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
inline std::unordered_map<std::string, std::string> subprog_label_map;
inline std::unordered_map<float, std::string> float_constant_map;
inline std::unordered_map<bool, std::string> bool_constant_map;

inline int stack_shift = 0;
inline int constant_counter = 0;

inline const std::string CONSTANT_NAME = "__constant__";
inline const std::string DEFINE_BYTE = " db ";
inline const std::string DEFINE_WORD = " dw ";
inline const std::string DEFINE_DOUBLE_WORD = " dd ";
inline const std::string DEFINE_QUAD = " dq ";
inline const int STACK_BIT_DEPTH = 8;

inline std::string EXPRESSION_LISTING;

inline const std::string CALL_OP = "CALL";
inline const std::string RET_OP = "RET";
inline const std::string POP_OP = "POP";
inline const std::string MOV_OP = "MOV";
inline const std::string MOVZX_OP = "MOVZX";
inline const std::string CMP_OP = "CMP";
inline const std::string PUSH_OP = "PUSH";
inline const std::string ADD_OP = "ADD";
inline const std::string SUB_OP = "SUB";
inline const std::string IMUL_OP = "IMUL";
inline const std::string IDIV_OP = "IDIV";

inline const std::string XOR_OP = "XOR";
inline const std::string AND_OP = "AND";
inline const std::string OR_OP = "OR";

// mutate CF and ZF if above or below (unsigned + float)
inline const std::string SETE_OP = "SETE";
inline const std::string SETBE_OP = "SETBE";
inline const std::string SETB_OP = "SETB";
inline const std::string SETAE_OP = "SETAE";
inline const std::string SETA_OP = "SETA";

// mutate SF and ZF if greater or lower (signed)
inline const std::string SETLE_OP = "SETLE";
inline const std::string SETL_OP = "SETL";
inline const std::string SETGE_OP = "SETGE";
inline const std::string SETG_OP = "SETG";

inline const std::string CQO_OP = "CQO"; // https://www.felixcloutier.com/x86/cwd:cdq:cqo CWD/CDQ/CQO — Convert Word to Doubleword/Convert Doubleword to Quadword

inline const std::string ADDSD_OP = "ADDSD";
inline const std::string SUBSD_OP = "SUBSD";
inline const std::string MULSD_OP = "MULSD";
inline const std::string DIVSD_OP = "DIVSD";
inline const std::string MOVSD_OP = "MOVSD";
inline const std::string CVTSD2SI_OP = "CVTSD2SI"; // https://www.felixcloutier.com/x86/cvtsd2si Convert Scalar Double Precision Floating-Point Value to Doubleword Integer
inline const std::string CVTSI2SD_OP = "CVTSI2SD"; // https://www.felixcloutier.com/x86/cvtsi2sd Convert Doubleword Integer to Scalar Double Precision Floating-Point Value
inline const std::string UCOMISD_OP = "UCOMISD"; // https://c9x.me/x86/html/file_module_x86_id_316.html Unordered Compare Scalar Double-Precision Floating- Point Values and Set EFLAGS

// 8 bit
inline const std::string AL_REG = "AL";

// 64 bit
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

std::string handle_expression(AstNode *node, bool handle_next);
std::string handle_non_terminal_operation(AstNode *node, NonTerminal non_terminal, bool* handled);
std::string create_constants(AstNode *root);
std::string handle_operations(AstNode *root);
void generate_code(AstNode *root);
#endif //CODEGEN_H
