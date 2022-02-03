#include "nes-types.h"
#include <stdio.h>
#include <string.h>

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
    b32 battery_backed_ram_present;
    b32 trainer_present_512_bytes;
    NesRomMirroringType mirroring_type;
    NesRomMapperType mapper_type;
};

struct NesRom {
    NesRomFileHeader header;
};

#define NES_ROM_HEADER_SIZE 16
#define NES_ROM_HEADER_STR "NES"
#define NES_ROM_HEADER_FORMAT_BYTE 0x1A

#define NES_ROM_HEADER_FORMAT_BYTE_INDEX        3
#define NES_ROM_HEADER_COUNT_PRG_ROM_BYTE_INDEX 4
#define NES_ROM_HEADER_COUNT_VROM__BYTE_INDEX   5
#define NES_ROM_HEADER_CTRL_1_BYTE_INDEX        6
#define NES_ROM_HEADER_CTRL_2_BYTE_INDEX        7

internal NesRomMapperType
nes_rom_get_rom_mapper_type_from_mapper_number(u8 mapper_number) {
    
    switch (mapper_number) {
        case 0:  return nrom;
        case 1:  return nintendo_mmc_1;
        case 2:  return unrom_switch;
        case 3:  return cnrom_switch;
        case 4:  return nintendo_mmc_3;
        case 5:  return nintendo_mmc_5;
        case 6:  return ffe_f4xxx;
        case 7:  return aorom_switch;
        case 8:  return ffe_f3xxx;
        case 9:  return nintendo_mmc_2;
        case 10: return nintendo_mmc_4;
    }
}

internal NesRomFileHeader
nes_rom_parse_file_header(char* rom_file_header_str) {

    NesRomFileHeader rom_header = {0};

    //make sure we have a valid ROM header
    char header[3];
    memccpy(header, rom_file_header_str, 0, 3);    
    if (strcmp(header, NES_ROM_HEADER_STR) != 0) {
        return rom_header;
    }

    //verify format byte
    if (rom_file_header_str[NES_ROM_HEADER_FORMAT_BYTE_INDEX] != NES_ROM_HEADER_FORMAT_BYTE) {
        return rom_header;
    }

    //if we are at this point, we have a valid ROM header
    rom_header.valid = TRUE;

    //get rom bank counts
    rom_header.count_16kb_prg_rom_banks = (u32)rom_file_header_str[NES_ROM_HEADER_COUNT_PRG_ROM_BYTE_INDEX];
    rom_header.count_8kb_vrom_banks     = (u32)rom_file_header_str[NES_ROM_HEADER_COUNT_VROM__BYTE_INDEX];

    u8 rom_ctrl_byte_1 = rom_file_header_str[NES_ROM_HEADER_CTRL_1_BYTE_INDEX];
    u8 rom_ctrl_byte_2 = rom_file_header_str[NES_ROM_HEADER_CTRL_2_BYTE_INDEX];

    //get the mirroring type
    if (ReadBitInByte(3, rom_ctrl_byte_1) == 1) {
        rom_header.mirroring_type = NesRomMirroringType::four_screen;
    } 
    else {
        rom_header.mirroring_type = (ReadBitInByte(0, rom_ctrl_byte_1) == 1)
            ? NesRomMirroringType::vertical
            : NesRomMirroringType::horizontal;
    }

    rom_header.battery_backed_ram_present = (ReadBitInByte(1, rom_ctrl_byte_1) == 1)
        ? TRUE
        : FALSE;
    rom_header.trainer_present_512_bytes = (ReadBitInByte(2, rom_ctrl_byte_1) == 1)
        ? TRUE
        : FALSE;

    u8 mapper_number_lsb = rom_ctrl_byte_1 & 0xF0;

    u8 mapper_number_msb = rom_ctrl_byte_2 & 0xF0;

    u8 mapper_number = mapper_number_msb | (mapper_number_lsb >> 4);

    return rom_header;
}

internal NesRom
nes_rom_create_and_initialize(Buffer rom_buffer) {
    
    char rom_file_header_str[NES_ROM_HEADER_SIZE];
    memcpy(rom_file_header_str, rom_buffer.buffer_contents, NES_ROM_HEADER_SIZE);

    NesRom rom = {0};
    rom.header = nes_rom_parse_file_header(rom_file_header_str);    

    return rom;
}