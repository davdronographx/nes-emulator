#ifndef NES_CPU_HPP
#define NES_CPU_HPP

#include "nes-types.h"
#include "stdio.h"

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
#define NesCpuFlagCheckV(ptr_NesCpu, result) if (result > 0xFF) NesCpuFlagSetV(ptr_NesCpu) 

#define NesCpuFlagSetN(ptr_NesCpu)   SetBitInByte(NES_CPU_FLAG_N,ptr_NesCpu->registers.p)
#define NesCpuFlagClearN(ptr_NesCpu) ClearBitInByte(NES_CPU_FLAG_N,ptr_NesCpu->registers.p)
#define NesCpuFlagReadN(ptr_NesCpu)  ReadBitInByte(NES_CPU_FLAG_N,ptr_NesCpu->registers.p)
#define NesCpuFlagCheckN(ptr_NesCpu, result) if (result > 127) NesCpuFlagSetN(ptr_NesCpu) 


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

struct NesCpu {
    NesCpuMemoryMap mem_map;
    NesCpuRegisters registers;    
    NesCpuInstruction current_instr;
    NesCpuInstruction previous_instr;
    char* debug_str;
};

enum NesCpuInterruptType {
    IRQ,
    NMI,
    RST
};

#define NES_CPU_IRQ_ADDR_MSB 0xFFFF
#define NES_CPU_IRQ_ADDR_LSB 0xFFFE

#define NES_CPU_NMI_ADDR_MSB 0xFFFB
#define NES_CPU_NMI_ADDR_LSB 0xFFFA

#define NES_CPU_RST_ADDR_LSB 0xFFFD
#define NES_CPU_RST_ADDR_MSB 0xFFFC 

////////////////////////////////////
// NES CPU INSTRUCTIONS
////////////////////////////////////

//accumulator
#define NES_CPU_INSTR_ASL_ACC   0x0A
#define NES_CPU_INSTR_LSR_ACC   0x4A
#define NES_CPU_INSTR_ROL_ACC   0x2A
#define NES_CPU_INSTR_ROR_ACC   0x6A

//absolute
#define NES_CPU_INSTR_ADC_ABS   0x6D
#define NES_CPU_INSTR_AND_ABS   0x2D
#define NES_CPU_INSTR_ASL_ABS   0x0E
#define NES_CPU_INSTR_BIT_ABS   0x2C
#define NES_CPU_INSTR_CMP_ABS   0xCD
#define NES_CPU_INSTR_CPX_ABS   0xEC
#define NES_CPU_INSTR_CPY_ABS   0xCC
#define NES_CPU_INSTR_DEC_ABS   0xCE
#define NES_CPU_INSTR_EOR_ABS   0x4D
#define NES_CPU_INSTR_INC_ABS   0xEE
#define NES_CPU_INSTR_JMP_ABS   0x4C
#define NES_CPU_INSTR_JSR_ABS   0x20
#define NES_CPU_INSTR_LDA_ABS   0xAD
#define NES_CPU_INSTR_LDX_ABS   0xAE
#define NES_CPU_INSTR_LDY_ABS   0xAC
#define NES_CPU_INSTR_LSR_ABS   0x4E
#define NES_CPU_INSTR_ORA_ABS   0x0D
#define NES_CPU_INSTR_ROL_ABS   0x2E
#define NES_CPU_INSTR_ROR_ABS   0x6E
#define NES_CPU_INSTR_SBC_ABS   0xED
#define NES_CPU_INSTR_STA_ABS   0x8D
#define NES_CPU_INSTR_STX_ABS   0x8E
#define NES_CPU_INSTR_STY_ABS   0x8C

//absolute x indexed
#define NES_CPU_INSTR_ADC_ABS_X 0x7D
#define NES_CPU_INSTR_AND_ABS_X 0x3D
#define NES_CPU_INSTR_ASL_ABS_X 0x1E
#define NES_CPU_INSTR_CMP_ABS_X 0xDD
#define NES_CPU_INSTR_DEC_ABS_X 0xDE
#define NES_CPU_INSTR_EOR_ABS_X 0x5D
#define NES_CPU_INSTR_INC_ABS_X 0xFE
#define NES_CPU_INSTR_LDA_ABS_X 0xBD
#define NES_CPU_INSTR_LDY_ABS_X 0xBC
#define NES_CPU_INSTR_LSR_ABS_X 0x5E
#define NES_CPU_INSTR_ORA_ABS_X 0x1D
#define NES_CPU_INSTR_ROL_ABS_X 0x3E
#define NES_CPU_INSTR_ROR_ABS_X 0x7E
#define NES_CPU_INSTR_SBC_ABS_X 0xFD
#define NES_CPU_INSTR_STA_ABS_X 0x9D

//absolute y indexed
#define NES_CPU_INSTR_ADC_ABS_Y 0x79
#define NES_CPU_INSTR_AND_ABS_Y 0x39
#define NES_CPU_INSTR_CMP_ABS_Y 0xD9
#define NES_CPU_INSTR_EOR_ABS_Y 0x59
#define NES_CPU_INSTR_LDA_ABS_Y 0xB9
#define NES_CPU_INSTR_LDX_ABS_Y 0xBE
#define NES_CPU_INSTR_ORA_ABS_Y 0x19
#define NES_CPU_INSTR_SBC_ABS_Y 0xF9
#define NES_CPU_INSTR_STA_ABS_Y 0x99

//immediate
#define NES_CPU_INSTR_ADC_IMM   0x69
#define NES_CPU_INSTR_AND_IMM   0x29
#define NES_CPU_INSTR_CMP_IMM   0xC9
#define NES_CPU_INSTR_CPX_IMM   0xE0
#define NES_CPU_INSTR_CPY_IMM   0xC0
#define NES_CPU_INSTR_EOR_IMM   0x49
#define NES_CPU_INSTR_LDA_IMM   0xA9
#define NES_CPU_INSTR_LDX_IMM   0xA2
#define NES_CPU_INSTR_LDY_IMM   0xA0
#define NES_CPU_INSTR_ORA_IMM   0x09
#define NES_CPU_INSTR_SBC_IMM   0xE9

//implied
#define NES_CPU_INSTR_BRK_IMP   0x00
#define NES_CPU_INSTR_CLC_IMP   0x18
#define NES_CPU_INSTR_CLD_IMP   0xD8
#define NES_CPU_INSTR_CLI_IMP   0x58
#define NES_CPU_INSTR_CLV_IMP   0xB8
#define NES_CPU_INSTR_DEX_IMP   0xCA
#define NES_CPU_INSTR_DEY_IMP   0x88
#define NES_CPU_INSTR_INX_IMP   0xE8
#define NES_CPU_INSTR_INY_IMP   0xC8
#define NES_CPU_INSTR_NOP_IMP   0xEA
#define NES_CPU_INSTR_PHA_IMP   0x48
#define NES_CPU_INSTR_PHP_IMP   0x08
#define NES_CPU_INSTR_PLA_IMP   0x68
#define NES_CPU_INSTR_PLP_IMP   0x28
#define NES_CPU_INSTR_RTI_IMP   0x40
#define NES_CPU_INSTR_RTS_IMP   0x60
#define NES_CPU_INSTR_SEC_IMP	0x38
#define NES_CPU_INSTR_SED_IMP	0xF8
#define NES_CPU_INSTR_SEI_IMP	0x78
#define NES_CPU_INSTR_TAX_IMP	0xAA
#define NES_CPU_INSTR_TAY_IMP	0xA8
#define NES_CPU_INSTR_TSX_IMP	0xBA
#define NES_CPU_INSTR_TXA_IMP	0x8A
#define NES_CPU_INSTR_TXS_IMP	0x9A
#define NES_CPU_INSTR_TYA_IMP	0x98

//indirect
#define NES_CPU_INSTR_JMP_IND   0x6C

//indirect x indexed
#define NES_CPU_INSTR_ADC_IND_X 0x61
#define NES_CPU_INSTR_AND_IND_X 0x21
#define NES_CPU_INSTR_CMP_IND_X 0xC1
#define NES_CPU_INSTR_EOR_IND_X 0x41
#define NES_CPU_INSTR_LDA_IND_X 0xA1
#define NES_CPU_INSTR_ORA_IND_X 0x01
#define NES_CPU_INSTR_SBC_IND_X 0xE1
#define NES_CPU_INSTR_STA_IND_X 0x81

//indirect y indexed
#define NES_CPU_INSTR_ADC_IND_Y 0x71        
#define NES_CPU_INSTR_AND_IND_Y 0x31
#define NES_CPU_INSTR_CMP_IND_Y 0xD1
#define NES_CPU_INSTR_EOR_IND_Y 0x51
#define NES_CPU_INSTR_LDA_IND_Y 0xB1
#define NES_CPU_INSTR_ORA_IND_Y 0x11
#define NES_CPU_INSTR_SBC_IND_Y 0xF1
#define NES_CPU_INSTR_STA_IND_Y 0x91

//relative
#define NES_CPU_INSTR_BCC_REL   0x90
#define NES_CPU_INSTR_BCS_REL   0xB0
#define NES_CPU_INSTR_BEQ_REL   0xF0
#define NES_CPU_INSTR_BMI_REL   0x30
#define NES_CPU_INSTR_BNE_REL   0xD0
#define NES_CPU_INSTR_BPL_REL   0x10 
#define NES_CPU_INSTR_BVC_REL   0x50
#define NES_CPU_INSTR_BVS_REL   0x70

//zero paged
#define NES_CPU_INSTR_ADC_ZP    0x65
#define NES_CPU_INSTR_AND_ZP    0x25
#define NES_CPU_INSTR_ASL_ZP    0x06
#define NES_CPU_INSTR_BIT_ZP    0x24
#define NES_CPU_INSTR_CMP_ZP    0xC5
#define NES_CPU_INSTR_CPY_ZP    0xC4
#define NES_CPU_INSTR_DEC_ZP    0xC6
#define NES_CPU_INSTR_EOR_ZP    0x45
#define NES_CPU_INSTR_INC_ZP    0xE6
#define NES_CPU_INSTR_LDA_ZP    0xA5
#define NES_CPU_INSTR_LDX_ZP    0xA6
#define NES_CPU_INSTR_LDY_ZP    0xA4
#define NES_CPU_INSTR_LSR_ZP    0x46
#define NES_CPU_INSTR_ORA_ZP    0x05
#define NES_CPU_INSTR_ROL_ZP    0x26
#define NES_CPU_INSTR_ROR_ZP    0x66
#define NES_CPU_INSTR_SBC_ZP    0xE5
#define NES_CPU_INSTR_STA_ZP    0x85
#define NES_CPU_INSTR_STX_ZP    0x86
#define NES_CPU_INSTR_STY_ZP    0x84    
#define NES_CPU_INSTR_CPX_ZP    0xE4

//zero paged x indexed
#define NES_CPU_INSTR_ADC_ZP_X  0x75
#define NES_CPU_INSTR_AND_ZP_X  0x35
#define NES_CPU_INSTR_ASL_ZP_X  0x16
#define NES_CPU_INSTR_CMP_ZP_X  0xD5
#define NES_CPU_INSTR_DEC_ZP_X  0xD6
#define NES_CPU_INSTR_EOR_ZP_X  0x55
#define NES_CPU_INSTR_INC_ZP_X  0xF6
#define NES_CPU_INSTR_LDA_ZP_X  0xB5
#define NES_CPU_INSTR_LDY_ZP_X  0xB4
#define NES_CPU_INSTR_LSR_ZP_X  0x56
#define NES_CPU_INSTR_ORA_ZP_X  0x15
#define NES_CPU_INSTR_ROL_ZP_X  0x36
#define NES_CPU_INSTR_ROR_ZP_X  0x76
#define NES_CPU_INSTR_SBC_ZP_X  0xF5
#define NES_CPU_INSTR_STA_ZP_X  0x95
#define NES_CPU_INSTR_STY_ZP_X  0x94

//zero paged y indexed
#define NES_CPU_INSTR_LDX_ZP_Y  0xB6
#define NES_CPU_INSTR_STX_ZP_Y  0x96

#endif //NES_CPU_HPP
