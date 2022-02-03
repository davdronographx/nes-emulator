#ifndef NES_EMULATOR_HPP
#define NES_EMULATOR_HPP

#include "nes-types.h"
#include "nes-cpu.cpp"
#include "nes-rom.cpp"

struct NesEmulatorFileBuffer {
    char* file_name;
    Buffer file_buffer;
};

typedef void (*file_read_callback)(NesEmulatorFileBuffer* file_buffer);
typedef void (*file_close_callback)(NesEmulatorFileBuffer* file_buffer);

struct NesEmulatorPlatformCallbacks {
    file_read_callback open_and_read_file;
    file_close_callback close_and_free_file;
};

struct NesEmulator {
    NesCpu cpu;
    NesRom rom;
    NesEmulatorPlatformCallbacks platform_callbacks;
};



#endif //NES_EMULATOR_HPP