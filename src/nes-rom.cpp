#include "nes-rom.hpp"

internal NesRomMapperType
nes_rom_get_rom_mapper_type_from_mapper_number(u8 mapper_number) {
    
    switch (mapper_number) {
        case NES_ROM_MAPPER_NROM:            return nrom;
        case NES_ROM_MAPPER_NINTENDO_MMC_1:  return nintendo_mmc_1;
        case NES_ROM_MAPPER_UNROM_SWITCH:    return unrom_switch;
        case NES_ROM_MAPPER_CNROM_SWITCH:    return cnrom_switch;
        case NES_ROM_MAPPER_NINTENDO_MMC_3:  return nintendo_mmc_3;
        case NES_ROM_MAPPER_NINTENDO_MMC_5:  return nintendo_mmc_5;
        case NES_ROM_MAPPER_FFE_F4XXX:       return ffe_f4xxx;
        case NES_ROM_MAPPER_AOROM_SWITCH:    return aorom_switch;
        case NES_ROM_MAPPER_FFE_F3XXX:       return ffe_f3xxx;
        case NES_ROM_MAPPER_NINTENDO_MMC_2:  return nintendo_mmc_2;
        case NES_ROM_MAPPER_NINTENDO_MMC_4:  return nintendo_mmc_4;
        //defasult assume no mapper / NROM
        default:                             return nrom;
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

    rom_header.count_8kb_ram_banks = rom_file_header_str[NES_ROM_HEADER_COUNT_RAM_BYTE_INDEX];

    return rom_header;
}


internal NesRomPrgRomBank* 
nes_rom_prg_rom_create_and_initialize(char* nes_rom_str, u32 count_prg_rom_banks) {

    //allocate memory for the prg rom
    NesRomPrgRomBank* prg_rom = (NesRomPrgRomBank*)malloc(count_prg_rom_banks * sizeof(NesRomPrgRomBank));

    for (u32 bank_index = 0; bank_index < count_prg_rom_banks; ++bank_index) {
        memmove(prg_rom[bank_index].memory, 
                &nes_rom_str[(bank_index * NES_ROM_SIZE_PRG_ROM_BANK) + NES_ROM_SIZE_HEADER], 
                NES_ROM_SIZE_PRG_ROM_BANK);
    }

    return prg_rom;
}

internal void
nes_rom_prg_rom_destroy(NesRomPrgRomBank* prg_rom, u32 count_prg_rom_banks) {
    
    for (u32 i; i < count_prg_rom_banks; ++i) {
        prg_rom[i] = {0};
    }
    free(nes_rom_prg_rom_destroy);
}

internal void
nes_rom_destroy(NesRom* rom) {

    nes_rom_prg_rom_destroy(rom->prg_rom, rom->header.count_16kb_prg_rom_banks);
}

internal NesRomChrRomBank*
nes_rom_chr_rom_create_and_initialize(char* nes_rom_str, u32 count_chr_rom_banks, u32 count_prg_rom_banks) {

    //allocate memory for the chr rom
    NesRomChrRomBank* chr_rom = (NesRomChrRomBank*)malloc(count_chr_rom_banks * sizeof(NesRomChrRomBank)); 

    for (u32 bank_index = 0; bank_index < count_chr_rom_banks; ++bank_index) {
        memmove(
            chr_rom[bank_index].memory,
            &nes_rom_str[(bank_index * NES_ROM_SIZE_VROM_BANK) + (NES_ROM_SIZE_PRG_ROM_BANK * count_prg_rom_banks) + NES_ROM_SIZE_HEADER],
            NES_ROM_SIZE_VROM_BANK
        );
    }

    return chr_rom;
}

internal NesRom
nes_rom_create_and_initialize(Buffer rom_buffer) {
    
    char rom_file_header_str[NES_ROM_SIZE_HEADER];
    memcpy(rom_file_header_str, rom_buffer.buffer_contents, NES_ROM_SIZE_HEADER);

    NesRom rom  = {0};
    rom.header  = nes_rom_parse_file_header(rom_file_header_str);
    rom.prg_rom = nes_rom_prg_rom_create_and_initialize(rom_buffer.buffer_contents, rom.header.count_16kb_prg_rom_banks);
    rom.chr_rom = nes_rom_chr_rom_create_and_initialize(rom_buffer.buffer_contents, rom.header.count_8kb_vrom_banks, rom.header.count_16kb_prg_rom_banks);
    
    return rom;
}