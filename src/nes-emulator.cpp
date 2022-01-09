#include <Windows.h>
#include "nes-types.h"
#include "nes-cpu.cpp"

struct NesEmulator {
    NesCpu cpu;
};

i32 WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance,
    PSTR cmd_line, i32 cmd_show)
{
    NesEmulator nes_emulator = {0};
    nes_emulator.cpu = nes_cpu_initialize();
    return 0;
}