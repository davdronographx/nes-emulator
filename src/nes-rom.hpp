#ifndef NES_ROM_HPP
#define NES_ROM_HPP

#include "nes-types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define NES_ROM_MAPPER_NROM           0
#define NES_ROM_MAPPER_NINTENDO_MMC_1 1
#define NES_ROM_MAPPER_UNROM_SWITCH   2
#define NES_ROM_MAPPER_CNROM_SWITCH   3
#define NES_ROM_MAPPER_NINTENDO_MMC_3 4
#define NES_ROM_MAPPER_NINTENDO_MMC_5 5
#define NES_ROM_MAPPER_FFE_F4XXX      6
#define NES_ROM_MAPPER_AOROM_SWITCH   7
#define NES_ROM_MAPPER_FFE_F3XXX      8
#define NES_ROM_MAPPER_NINTENDO_MMC_2 9
#define NES_ROM_MAPPER_NINTENDO_MMC_4 10

#define NES_ROM_SIZE_PRG_ROM_BANK 0x4000
#define NES_ROM_SIZE_VROM_BANK    0x2000

struct NesRomPrgRomBank {
    nes_val memory[NES_ROM_SIZE_PRG_ROM_BANK];
};

struct NesRomChrRomBank {
    nes_val memory[NES_ROM_SIZE_VROM_BANK];
};

enum NesRomMapperType {
    nrom,
    nintendo_mmc_1,
    unrom_switch,
    cnrom_switch,
    nintendo_mmc_3,
    nintendo_mmc_5,
    ffe_f4xxx,
    aorom_switch,
    ffe_f3xxx,
    nintendo_mmc_2,
    nintendo_mmc_4
};

enum NesRomMirroringType {
    horizontal,
    vertical,
    four_screen
};

struct NesRomFileHeader {
    b32 valid;
    u32 count_16kb_prg_rom_banks;
    u32 count_8kb_vrom_banks;
    u32 count_8kb_ram_banks;
    b32 battery_backed_ram_present;
    b32 trainer_present_512_bytes;
    NesRomMirroringType mirroring_type;
    NesRomMapperType mapper_type;
};

struct NesRom {
    NesRomFileHeader header;
    NesRomPrgRomBank* prg_rom;
    NesRomChrRomBank* chr_rom;
};

#define NES_ROM_SIZE_HEADER 16
#define NES_ROM_HEADER_STR "NES"
#define NES_ROM_HEADER_FORMAT_BYTE 0x1A

#define NES_ROM_HEADER_FORMAT_BYTE_INDEX        3
#define NES_ROM_HEADER_COUNT_PRG_ROM_BYTE_INDEX 4
#define NES_ROM_HEADER_COUNT_VROM__BYTE_INDEX   5
#define NES_ROM_HEADER_CTRL_1_BYTE_INDEX        6
#define NES_ROM_HEADER_CTRL_2_BYTE_INDEX        7
#define NES_ROM_HEADER_COUNT_RAM_BYTE_INDEX     8

#endif //NES_ROM_HPP