#ifndef NES_CPU_HPP
#define NES_CPU_HPP

#include "nes-types.h"
#include "nes-memory-map.hpp"
#include "nes-memory-map.cpp"
#include "nes-cpu-instr.hpp"
#include <stdio.h>
#include <stdlib.h>

#define NES_CPU_DEBUG_LOG 1


struct NesCpuRegisters {
    //program counter
    u16 pc;
    //stack pointer
    u8 sp;
    //accumulator a
    u8 acc_a;
    //index register x
    u8 ir_x;
    //index register y;
    u8 ir_y;
    //processor status
    u8 p;
};

//carry flag
#define NES_CPU_FLAG_C 0
//zero flag
#define NES_CPU_FLAG_Z 1
//interrupt disable flag
#define NES_CPU_FLAG_I 2
//decimal mode flag
#define NES_CPU_FLAG_D 3
//break command flag
#define NES_CPU_FLAG_B 4
//overflow flag
#define NES_CPU_FLAG_V 6
//negative flag
#define NES_CPU_FLAG_N 7

enum NesCpuAddressMode {
    zero_page,
    zero_page_indexed_x,
    zero_page_indexed_y,
    absolute,
    absolute_indexed_x,
    absolute_indexed_y,
    indirect,
    implied,
    accumulator,
    immediate,
    relative,
    indexed_indirect_x,
    indirect_indexed_y
};

struct NesCpuInstrResult {
    u32 value;
    u32 cycles;
    u32 branch_cycles;
    b32 flag_c;
    b32 flag_z;
    b32 flag_i;
    b32 flag_d;
    b32 flag_b;
    b32 flag_v;
    b32 flag_n;
    b32 page_boundary_crossed;
};

struct NesCpuInstruction {
    nes_val op_code;
    nes_val* operand_addr;
    NesCpuAddressMode addr_mode;
    NesCpuInstrResult result;
};

struct NesCpuDebugInfo {
    char* pc_str;
    char* pc_p0_val_str;
    char* pc_p1_val_str;
    char* pc_p2_val_str;
    char* op_code_str;
    char* addr_mode_str;
    char* cyc_str;
    char* acc_str;
    char* ind_x_str;
    char* ind_y_str;
    char* sp_str;
    char* debug_str;
};

struct NesCpu {
    NesMemoryMap mem_map;
    NesCpuRegisters registers;    
    NesCpuInstruction current_instr;
    NesCpuInstruction previous_instr;
    NesCpuDebugInfo debug_info;
};

enum NesCpuInterruptType {
    IRQ,
    NMI,
    RST
};

#define NES_CPU_INTERRUPT_VECTOR_NMI 0xFFFA
#define NES_CPU_INTERRUPT_VECTOR_RST 0xFFFC
#define NES_CPU_INTERRUPT_VECTOR_BRK 0xFFFE

#endif //NES_CPU_HPP
