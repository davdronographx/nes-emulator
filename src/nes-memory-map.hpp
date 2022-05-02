#ifndef NES_MEMORY_MAP_HPP
#define NES_MEMORY_MAP_HPP

#include "nes-types.h"

#define NES_MEM_MAP_ZERO_PAGE_ADDR  0x0000
#define NES_MEM_MAP_ZERO_PAGE_SIZE  0x0100

#define NES_MEM_MAP_STACK_ADDR      0x0100
#define NES_MEM_MAP_STACK_SIZE      0x0100
#define NES_MEM_MAP_STACK_PTR_BEGIN 0xFF
#define NES_MEM_MAP_STACK_PTR_END   0x00

#define NES_MEM_MAP_RAM_BEGIN       0x0000
#define NES_MEM_MAP_RAM_END         0x1FFF
#define NES_MEM_MAP_RAM_ADDR        0x0200
#define NES_MEM_MAP_RAM_SIZE        0x0600

#define NES_MEM_MAP_RAM_MIRROR_ADDR 0x0800
#define NES_MEM_MAP_RAM_MIRROR_SIZE 0x1800

//$0000 - $1FFF
struct NesMemoryMapRam {
    //$0000 - $00FF
    u8 zero_page[NES_MEM_MAP_ZERO_PAGE_SIZE];
    //$0100 - $01FF
    u8 stack[NES_MEM_MAP_STACK_SIZE];
    //$0200 - $07FF
    u8 ram[NES_MEM_MAP_RAM_SIZE];
};

#define NES_MEM_MAP_IO_REG_BEGIN       0x2000
#define NES_MEM_MAP_IO_REG_END         0x401F

#define NES_MEM_MAP_LOWER_IO_REG_ADDR  0x2000
#define NES_MEM_MAP_LOWER_IO_REG_SIZE  0x0008

#define NES_MEM_MAP_IO_REG_MIRROR_ADDR 0x2008
#define NES_MEM_MAP_IO_REG_MIRROR_SIZE 0x1FF8

#define NES_MEM_MAP_UPPER_IO_REG_ADDR  0x4000
#define NES_MEM_MAP_UPPER_IO_REG_SIZE  0x0008

//$2000 - $401F
struct NesMemoryMapIoRegisters {
    //$2000 - $2007
    u8 io_registers_lower[NES_MEM_MAP_LOWER_IO_REG_SIZE];
    //$2008 - $3FFF
    u8 mirrors[NES_MEM_MAP_IO_REG_MIRROR_SIZE];
    //$4000 - $401F
    u8 io_registers_upper[NES_MEM_MAP_UPPER_IO_REG_SIZE];
};

#define NES_MEM_MAP_EXPANSION_ROM_ADDR 0x4020
#define NES_MEM_MAP_EXPANSION_ROM_SIZE 0x1FE0

#define NES_MEM_MAP_SRAM_ADDR          0x6000 
#define NES_MEM_MAP_SRAM_SIZE          0x2000 

#define NES_MEM_MAP_LOWER_PRG_ROM_ADDR 0x8000
#define NES_MEM_MAP_LOWER_PRG_ROM_SIZE 0x4000

#define NES_MEM_MAP_UPPER_PRG_ROM_ADDR 0xC000
#define NES_MEM_MAP_UPPER_PRG_ROM_SIZE 0x4000

//$8000 - $10000
struct NesMemoryMapPrgRom {
    //$8000 - $BFFF
    u8 lower_bank[NES_MEM_MAP_LOWER_PRG_ROM_SIZE];
    //$COOO - $10000
    u8 upper_bank[NES_MEM_MAP_UPPER_PRG_ROM_SIZE];
};

struct NesMemoryMap {
    //$0000 - $1FFF
    NesMemoryMapRam ram;
    //$2000 - $401F
    NesMemoryMapIoRegisters io_registers;
    //$4020 - $5FFF
    u8 expansion_rom[NES_MEM_MAP_EXPANSION_ROM_SIZE];
    //$6000 - $7FFF 
    u8 sram[NES_MEM_MAP_SRAM_SIZE];
    //$8000 - $10000
    NesMemoryMapPrgRom prg_rom;
};

#endif //NES_MEMORY_MAP_HPP
