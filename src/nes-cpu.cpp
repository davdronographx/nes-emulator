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

//$0000 - $1FFF
struct NesCpuRam {
    //$0000 - $00FF
    u8 zero_page[0x0100];
    //$0100 - $01FF
    u8 stack[0x0100];
    //$0200 - $07FF
    u8 ram[0x0600];
    //$0800 - $1FFF
    u8 mirrors[0x1800];
};

//$2000 - $401F
struct NesCpuIoRegisters {
    //$2000 - $2007
    u8 io_registers_lower[0x0008];
    //$2008 - $3FFF
    u8 mirrors[0x1FF8];
    //$4000 - $401F
    u8 io_registers_upper[0x0020];
};

//$8000 - $10000
struct NesCpuPrgRom {
    //$8000 - $BFFF
    u8 lower_bank[0x4000];
    //$COOO - $10000
    u8 upper_bank[0x4000];
};

struct NesCpuMemoryMap {
    //$0000 - $1FFF
    NesCpuRam ram;
    //$2000 - $401F
    NesCpuIoRegisters io_registers;
    //$4020 - $5FFF
    u8 expansion_rom[0x1FE0];
    //$6000 - $7FFF 
    u8 sram[0x2000];
    //$8000 - $10000
    NesCpuPrgRom prg_rom;
};

struct NesCpu {
    NesCpuMemoryMap mem_map;
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

internal void
nes_cpu_memory_write(NesCpuMemoryMap* mem_map, u16 address, u8 value) {
    ((u8*)(&(*mem_map)))[address] = value;
}

internal u8
nes_cpu_memory_read(NesCpuMemoryMap* mem_map, u16 address) {
    return ((u8*)(&(*mem_map)))[address];
}

internal NesCpu 
nes_cpu_initialize() {

    NesCpu nes_cpu = {0};

    return nes_cpu;
}

