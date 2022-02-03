#include "nes-emulator.hpp"

internal NesEmulator
nes_emulator_create_and_initialize(char* rom_path,
                                   NesEmulatorPlatformCallbacks platform_callbacks) {
    
    //open the ROM file
    NesEmulatorFileBuffer rom_buffer = {0};
    rom_buffer.file_name = rom_path;
    platform_callbacks.open_and_read_file(&rom_buffer);    

    NesEmulator nes_emulator = {0};
    nes_emulator.cpu                = nes_cpu_create_and_initialize();
    nes_emulator.platform_callbacks = platform_callbacks;
    nes_emulator.rom                = nes_rom_create_and_initialize(rom_buffer.file_buffer);
    ASSERT(nes_emulator.rom.header.valid);


    return nes_emulator;
}

internal b32
nes_emulator_update_and_render(NesEmulator emulator) {

}