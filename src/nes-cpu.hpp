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

#define NES_CPU_MEM_MAP_ZERO_PAGE_ADDR  0x0000
#define NES_CPU_MEM_MAP_ZERO_PAGE_SIZE  0x0100

#define NES_CPU_MEM_MAP_STACK_ADDR      0x0100
#define NES_CPU_MEM_MAP_STACK_SIZE      0x0100

#define NES_CPU_MEM_MAP_RAM_ADDR        0x0200
#define NES_CPU_MEM_MAP_RAM_SIZE        0x0600

#define NES_CPU_MEM_MAP_RAM_MIRROR_ADDR 0x0800
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

#define NES_CPU_MEM_MAP_LOWER_IO_REG_ADDR  0x2000
#define NES_CPU_MEM_MAP_LOWER_IO_REG_SIZE  0x0008

#define NES_CPU_MEM_MAP_IO_REG_MIRROR_ADDR 0x2008
#define NES_CPU_MEM_MAP_IO_REG_MIRROR_SIZE 0x1FF8

#define NES_CPU_MEM_MAP_UPPER_IO_REG_ADDR  0x4000
#define NES_CPU_MEM_MAP_UPPER_IO_REG_SIZE  0x0008

//$2000 - $401F
struct NesCpuIoRegisters {
    //$2000 - $2007
    u8 io_registers_lower[NES_CPU_MEM_MAP_LOWER_IO_REG_SIZE];
    //$2008 - $3FFF
    u8 mirrors[NES_CPU_MEM_MAP_IO_REG_MIRROR_SIZE];
    //$4000 - $401F
    u8 io_registers_upper[NES_CPU_MEM_MAP_UPPER_IO_REG_SIZE];
};

#define NES_CPU_MEM_MAP_EXPANSION_ROM_ADDR 0x4020
#define NES_CPU_MEM_MAP_EXPANSION_ROM_SIZE 0x1FE0

#define NES_CPU_MEM_MAP_SRAM_ADDR          0x6000 
#define NES_CPU_MEM_MAP_SRAM_SIZE          0x2000 

#define NES_CPU_MEM_MAP_LOWER_PRG_ROM_ADDR 0x8000
#define NES_CPU_MEM_MAP_LOWER_PRG_ROM_SIZE 0x4000

#define NES_CPU_MEM_MAP_UPPER_PRG_ROM_ADDR 0xC000
#define NES_CPU_MEM_MAP_UPPER_PRG_ROM_SIZE 0x4000

//$8000 - $10000
struct NesCpuPrgRom {
    //$8000 - $BFFF
    u8 lower_bank[NES_CPU_MEM_MAP_LOWER_PRG_ROM_SIZE];
    //$COOO - $10000
    u8 upper_bank[NES_CPU_MEM_MAP_UPPER_PRG_ROM_SIZE];
};

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
#define NesCpuFlagCheckC(ptr_NesCpu,result) if (result > 255) NesCpuFlagSetC(ptr_NesCpu)   

#define NesCpuFlagSetZ(ptr_NesCpu)   SetBitInByte(NES_CPU_FLAG_Z,ptr_NesCpu->registers.p)
#define NesCpuFlagClearZ(ptr_NesCpu) ClearBitInByte(NES_CPU_FLAG_Z,ptr_NesCpu->registers.p)
#define NesCpuFlagReadZ(ptr_NesCpu)  ReadBitInByte(NES_CPU_FLAG_Z,ptr_NesCpu->registers.p)
#define NesCpuFlagCheckZ(ptr_NesCpu, result) if (result == 0) NesCpuFlagSetZ(ptr_NesCpu) 

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
#define NesCpuFlagCheckV(ptr_NesCpu, result) if (result > 127) NesCpuFlagSetV(ptr_NesCpu) 

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
    indexed_indirect_x,
    indirect_indexed_y
};

#define NesCpuMemoryWrite(ptr_NesCpu, val_nes_addr, val_nes_val) { ((nes_val*)(&ptr_NesCpu->mem_map))[val_nes_addr] = val_nes_val; }

#define NesCpuMemoryRead(ptr_NesCpu, val_nes_addr) ((nes_val*)(&ptr_NesCpu->mem_map))[val_nes_addr]

#define NesCpuMemoryAddress(ptr_NesCpu, val_nes_addr) &(((nes_val*)(&ptr_NesCpu->mem_map))[val_nes_addr])

#define NesCpuMemoryReadAndIncrimentProgramCounter(ptr_NesCpu, val_nes_val)  { \
    val_nes_val = NesCpuMemoryRead(ptr_NesCpu, ptr_NesCpu->registers.pc); \
    ++ptr_NesCpu->registers.pc; \
}

#define NesCpuMemoryReadAndIncrimentProgramCounterIndexed(ptr_NesCpu, val_nes_val_offset, val_nes_val) { \
    val_nes_val = NesCpuMemoryRead(ptr_NesCpu, ptr_NesCpu->registers.pc); \
    val_nes_val += val_nes_val_offset; \
    ++ptr_NesCpu->registers.pc; \
}

struct NesCpuInstrResult {
    u32 value;
    u32 cycles;
    b32 flag_c;
    b32 flag_z;
    b32 flag_i;
    b32 flag_d;
    b32 flag_b;
    b32 flag_v;
    b32 flag_n;
};

#define NES_CPU_IRQ_HARDWARE_ADDR_MSB 0xFFFF
#define NES_CPU_IRQ_HARDWARE_ADDR_LSB 0xFFFA

#define NES_CPU_IRQ_BRK_ADDR_MSB 0xFFFF
#define NES_CPU_IRQ_BRK_ADDR_LSB 0xFFFE

#define NES_CPU_IRQ_VBLANK_ADDR_MSB 0xFFFB
#define NES_CPU_IRQ_VBLANK_ADDR_LSB 0xFFFA

#define NES_CPU_IRQ_RESET_ADDR_MSB 
#define NES_CPU_IRQ_RESET_ADDR_LSB 

#define ADC_I     0x69
#define ADC_ZP    0x65
#define ADC_ZP_X  0x75
#define ADC_ABS   0x6D
#define ADC_ABS_X 0x7D
#define ADC_ABS_Y 0x79
#define ADC_IND_X 0x61
#define ADC_IND_Y 0x71        

#define AND_I     0x29
#define AND_ZP    0x25
#define AND_ZP_X  0x35
#define AND_ABS   0x2D
#define AND_ABS_X 0x3D
#define AND_ABS_Y 0x39
#define AND_IND_X 0x21
#define AND_IND_Y 0x31

#define ASL_ACC   0x0A
#define ASL_ZP    0x06
#define ASL_ZPX   0x16
#define ASL_ABS   0x0E
#define ASL_ABS_X 0x1E

#define BCC_REL   0x90
#define BCS_REL   0xB0
#define BEQ_REL   0xF0
#define BMI_REL   0x30
#define BNE_REL   0xD0
#define BPL_REL   0x10 


//TODO - bit
//#define BIT_ZP  0x24
//#define BIT_ABS 0x2C


#endif //NES_CPU_HPP
