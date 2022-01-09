#include "nes-types.h"

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

struct NesCpu {
    NesCpuRegisters registers;    
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


internal void
nes_cpu_flag_set(NesCpu* cpu, u8 flag) {
    cpu->registers.p |= 1 << flag;
}

internal void
nes_cpu_flag_clear(NesCpu* cpu, u8 flag) {
    cpu->registers.p &= ~(1 << flag);
}

internal u8
nes_cpu_flag_read(NesCpu* cpu, u8 flag) {
    return (cpu->registers.p >> flag) & 1;
}

internal NesCpu 
nes_cpu_initialize() {

    NesCpu nes_cpu = {0};

    //set
    nes_cpu_flag_set(&nes_cpu, NES_CPU_FLAG_N);
    //read
    u8 value = nes_cpu_flag_read(&nes_cpu, NES_CPU_FLAG_N);
    //clear
    nes_cpu_flag_clear(&nes_cpu, NES_CPU_FLAG_N);
    //read
    value = nes_cpu_flag_read(&nes_cpu, NES_CPU_FLAG_N);

    return nes_cpu;
}