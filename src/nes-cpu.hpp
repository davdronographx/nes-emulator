#ifndef NES_CPU_HPP
#define NES_CPU_HPP

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

#define NES_CPU_MEM_MAP_ZERO_PAGE_SIZE 0x0100
#define NES_CPU_MEM_MAP_STACK_SIZE 0x0100
#define NES_CPU_MEM_MAP_RAM_SIZE 0x0600
#define NES_CPU_MEM_MAP_RAM_MIRROR_SIZE 0x1800

//$0000 - $1FFF
struct NesCpuRam {
    //$0000 - $00FF
    u8 zero_page[NES_CPU_MEM_MAP_ZERO_PAGE_SIZE];
    //$0100 - $01FF
    u8 stack[NES_CPU_MEM_MAP_STACK_SIZE];
    //$0200 - $07FF
    u8 ram[NES_CPU_MEM_MAP_RAM_SIZE];
    //$0800 - $1FFF
    u8 mirrors[NES_CPU_MEM_MAP_RAM_MIRROR_SIZE];
};

#define NES_CPU_MEM_MAP_LOWER_IO_REG_SIZE 0x0008
#define NES_CPU_MEM_MAP_IO_REG_MIRROR_SIZE 0x1FF8
#define NES_CPU_MEM_MAP_UPPER_IO_REG_SIZE 0x0008

//$2000 - $401F
struct NesCpuIoRegisters {
    //$2000 - $2007
    u8 io_registers_lower[NES_CPU_MEM_MAP_LOWER_IO_REG_SIZE];
    //$2008 - $3FFF
    u8 mirrors[NES_CPU_MEM_MAP_IO_REG_MIRROR_SIZE];
    //$4000 - $401F
    u8 io_registers_upper[NES_CPU_MEM_MAP_UPPER_IO_REG_SIZE];
};

#define NES_CPU_MEM_MAP_LOWER_PRG_ROM_SIZE 0x4000
#define NES_CPU_MEM_MAP_UPPER_PRG_ROM_SIZE 0x4000

//$8000 - $10000
struct NesCpuPrgRom {
    //$8000 - $BFFF
    u8 lower_bank[NES_CPU_MEM_MAP_LOWER_PRG_ROM_SIZE];
    //$COOO - $10000
    u8 upper_bank[NES_CPU_MEM_MAP_UPPER_PRG_ROM_SIZE];
};

#define NES_CPU_MEM_MAP_EXPANSION_ROM_SIZE 0x1FE0
#define NES_CPU_MEM_MAP_SRAM_SIZE 0x2000 

struct NesCpuMemoryMap {
    //$0000 - $1FFF
    NesCpuRam ram;
    //$2000 - $401F
    NesCpuIoRegisters io_registers;
    //$4020 - $5FFF
    u8 expansion_rom[NES_CPU_MEM_MAP_EXPANSION_ROM_SIZE];
    //$6000 - $7FFF 
    u8 sram[NES_CPU_MEM_MAP_SRAM_SIZE];
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

#define NesCpuFlagSetC(ptr_NesCpu)   SetBitInByte(NES_CPU_FLAG_C,ptr_NesCpu->registers.p)
#define NesCpuFlagClearC(ptr_NesCpu) ClearBitInByte(NES_CPU_FLAG_C,ptr_NesCpu->registers.p)
#define NesCpuFlagReadC(ptr_NesCpu)  ReadBitInByte(NES_CPU_FLAG_C,ptr_NesCpu->registers.p)

#define NesCpuFlagSetZ(ptr_NesCpu)   SetBitInByte(NES_CPU_FLAG_Z,ptr_NesCpu->registers.p)
#define NesCpuFlagClearZ(ptr_NesCpu) ClearBitInByte(NES_CPU_FLAG_Z,ptr_NesCpu->registers.p)
#define NesCpuFlagReadZ(ptr_NesCpu)  ReadBitInByte(NES_CPU_FLAG_Z,ptr_NesCpu->registers.p)

#define NesCpuFlagSetI(ptr_NesCpu)   SetBitInByte(NES_CPU_FLAG_I,ptr_NesCpu->registers.p)
#define NesCpuFlagClearI(ptr_NesCpu) ClearBitInByte(NES_CPU_FLAG_I,ptr_NesCpu->registers.p)
#define NesCpuFlagReadI(ptr_NesCpu)  ReadBitInByte(NES_CPU_FLAG_I,ptr_NesCpu->registers.p)

#define NesCpuFlagSetD(ptr_NesCpu)   SetBitInByte(NES_CPU_FLAG_D,ptr_NesCpu->registers.p)
#define NesCpuFlagClearD(ptr_NesCpu) ClearBitInByte(NES_CPU_FLAG_D,ptr_NesCpu->registers.p)
#define NesCpuFlagReadD(ptr_NesCpu)  ReadBitInByte(NES_CPU_FLAG_D,ptr_NesCpu->registers.p)

#define NesCpuFlagSetB(ptr_NesCpu)   SetBitInByte(NES_CPU_FLAG_B,ptr_NesCpu->registers.p)
#define NesCpuFlagClearB(ptr_NesCpu) ClearBitInByte(NES_CPU_FLAG_B,ptr_NesCpu->registers.p)
#define NesCpuFlagReadB(ptr_NesCpu)  ReadBitInByte(NES_CPU_FLAG_B,ptr_NesCpu->registers.p)

#define NesCpuFlagSetV(ptr_NesCpu)   SetBitInByte(NES_CPU_FLAG_V,ptr_NesCpu->registers.p)
#define NesCpuFlagClearV(ptr_NesCpu) ClearBitInByte(NES_CPU_FLAG_V,ptr_NesCpu->registers.p)
#define NesCpuFlagReadV(ptr_NesCpu)  ReadBitInByte(NES_CPU_FLAG_V,ptr_NesCpu->registers.p)

#define NesCpuFlagSetN(ptr_NesCpu)   SetBitInByte(NES_CPU_FLAG_N,ptr_NesCpu->registers.p)
#define NesCpuFlagClearN(ptr_NesCpu) ClearBitInByte(NES_CPU_FLAG_N,ptr_NesCpu->registers.p)
#define NesCpuFlagReadN(ptr_NesCpu)  ReadBitInByte(NES_CPU_FLAG_N,ptr_NesCpu->registers.p)

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
    indexed_indirect,
    indirect_indexed
};

#define NesCpuMemoryWrite(ptr_NesMemoryMap, val_nes_addr, val_nes_val) { \
    ((nes_val*)(&(*ptr_NesMemoryMap)))[val_nes_addr] = val_nes_val \
}

#define NesCpuMemoryRead(ptr_NesMemoryMap, val_nes_addr) ((nes_val*)(&(*ptr_NesMemoryMap)))[val_nes_addr]


#define NesCpuMemoryReadAndIncrimentProgramCounter(ptr_NesCpu, val_nes_val)  { \
    val_nes_val = NesCpuMemoryRead(&ptr_NesCpu->mem_map, ptr_NesCpu->registers.pc); \
    ++ptr_NesCpu->registers.pc; \
}

#define NesCpuMemoryReadAndIncrimentProgramCounterIndexed(ptr_NesCpu, val_nes_val_offset, val_nes_val) { \
    val_nes_val = NesCpuMemoryRead(&ptr_NesCpu->mem_map, ptr_NesCpu->registers.pc); \
    val_nes_val += val_nes_val_offset; \
    ++ptr_NesCpu->registers.pc; \
}


#endif //NES_CPU_HPP
