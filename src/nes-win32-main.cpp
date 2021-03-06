#include <Windows.h>
#include "nes-win32-io.cpp"
#include "nes-emulator.cpp"


internal void 
nes_win32_main_open_file_for_emulator(NesEmulatorFileBuffer* file_buffer) {

    file_buffer->file_buffer = nes_win32_io_open_and_read_file(file_buffer->file_name);
}

internal void
nes_win32_main_open_and_write_buffer_to_file(NesEmulatorFileBuffer* file_buffer) {

    nes_win32_io_open_and_write_file(file_buffer->file_name, file_buffer->file_buffer.buffer_contents, file_buffer->file_buffer.buffer_size);
}

internal void
nes_win32_main_close_and_free_file_for_nes_emulator(NesEmulatorFileBuffer* file_buffer) {

    nes_win32_io_destroy_file_info(file_buffer->file_buffer);
}

internal void
nes_win32_main_loop(NesEmulator* nes_emulator) {

    //todo - we need to loop this, but we can wait until we have a way for the user to escape
    nes_emulator_update_and_render(nes_emulator);
}

i32 WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance,
    PSTR cmd_line, i32 cmd_show)
{
    NesEmulatorPlatformCallbacks platform_callbacks = {0};
    platform_callbacks.open_and_read_file     = nes_win32_main_open_file_for_emulator;
    platform_callbacks.close_and_free_file    = nes_win32_main_close_and_free_file_for_nes_emulator;
    platform_callbacks.open_and_write_to_file = nes_win32_main_open_and_write_buffer_to_file;

    //TODO - we should probably tokenize the cmd line, but for now we are only passing in one argument
    NesEmulator nes_emulator = nes_emulator_create_and_initialize(cmd_line, platform_callbacks);
    nes_win32_main_loop(&nes_emulator);
    
    return 0;
}