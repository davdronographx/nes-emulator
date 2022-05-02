#include "nes-memory-map.hpp"

internal nes_val
nes_memory_map_read(NesMemoryMap* map, nes_addr address) {

    //read from RAM
    if ((address >= NES_MEM_MAP_RAM_BEGIN) && (address <= NES_MEM_MAP_RAM_END)) {
        //we can do a bitwise mask here because this address is mirrored between 0x0800 and 0x2000
        return ((nes_val*)(&(map->ram)))[address & 0x07FF];
    }
    //read from io registers
    if ((address >= NES_MEM_MAP_IO_REG_BEGIN) && (address <= NES_MEM_MAP_IO_REG_END)) {
        //TODO - io registers have not been defined yet
    }

}

internal void
nes_memory_map_write(NesMemoryMap* map, nes_addr address, nes_val value) {
    
    //read from RAM
    if ((address >= NES_MEM_MAP_RAM_BEGIN) && (address <= NES_MEM_MAP_RAM_END)) {
        //we can do a bitwise mask here because this address is mirrored between 0x0800 and 0x2000
        ((nes_val*)(&(map->ram)))[address & 0x07FF] = value;
    }
}

internal nes_val*
nes_memory_map_address(NesMemoryMap* map, nes_addr address) {

    return &(((nes_val*)(map))[address]);
}