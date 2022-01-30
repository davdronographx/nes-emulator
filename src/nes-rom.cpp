#include "nes-types.h"

struct NesRom {
    b32 valid;
};

#define NES_ROM_HEADER_STR "NES"

internal NesRom
nes_rom_create_and_initialize(Buffer rom_buffer) {
    NesRom rom = {0};

    return rom;
}