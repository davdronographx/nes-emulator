#include "nes-emulator.hpp"

internal NesEmulator
nes_emulator_create_and_initialize(char* rom_path,
                                   NesEmulatorPlatformCallbacks platform_callbacks) {
    
    //open the ROM file
    NesEmulatorFileBuffer rom_buffer = {0};
    rom_buffer.file_name = rom_path;
    platform_callbacks.open_and_read_file(&rom_buffer);    

    NesEmulator nes_emulator        = {0};
    nes_emulator.cpu                = nes_cpu_create_and_initialize();
    nes_emulator.platform_callbacks = platform_callbacks;
    
    //initialize the rom
    nes_emulator.rom                = nes_rom_create_and_initialize(rom_buffer.file_buffer);
    platform_callbacks.close_and_free_file(&rom_buffer);
    ASSERT(nes_emulator.rom.header.valid);

    return nes_emulator;
}

internal b32
nes_emulator_update_and_render(NesEmulator* emulator) {

    NesEmulatorFileBuffer log_file_buffer = {0};
    log_file_buffer.file_name = "nes-emulator-log.txt";
    log_file_buffer.file_buffer.buffer_size = 256;
    log_file_buffer.file_buffer.buffer_contents = "HELLO WORLD!!!";

    emulator->platform_callbacks.open_and_write_to_file(&log_file_buffer);

    //we need to read from the rom and update the CPU prg rom banks
    NesRomPrgRomBankRead rom_read = nes_rom_prg_rom_read(&emulator->rom);
    nes_cpu_update_prg_rom(&emulator->cpu, rom_read.low_bank.memory, rom_read.high_bank.memory);

    //reset and we are ready to go
    nes_cpu_reset(&emulator->cpu);

    //hold on to your butts...
    nes_cpu_tick(&emulator->cpu);

    return TRUE;
}