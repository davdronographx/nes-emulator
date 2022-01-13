#include "nes-cpu.hpp"

internal nes_val
nes_cpu_decode_operand_from_address_mode(NesCpu* cpu, NesCpuAddressMode address_mode) {

    nes_val operand = 0;
    nes_addr address = 0;
    nes_addr addr_lower = 0; 
    nes_addr addr_upper = 0;

    switch(address_mode) {
        
        case NesCpuAddressMode::zero_page: {

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, address);

            operand = NesCpuMemoryRead(&cpu->mem_map,address);

        } break;

        case NesCpuAddressMode::zero_page_indexed_x: {

            NesCpuMemoryReadAndIncrimentProgramCounterIndexed(cpu, cpu->registers.ir_x, address);

            operand = NesCpuMemoryRead(&cpu->mem_map,address);

        } break;

        case NesCpuAddressMode::zero_page_indexed_y: {

            NesCpuMemoryReadAndIncrimentProgramCounterIndexed(cpu, cpu->registers.ir_y, address);

            operand = NesCpuMemoryRead(&cpu->mem_map,address);
            
        } break;

        case NesCpuAddressMode::absolute: {
            
            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_lower);
            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_upper);

            address = (addr_upper << 8) + addr_lower;

            operand = NesCpuMemoryRead(&cpu->mem_map,address);

        } break;

        case NesCpuAddressMode::absolute_indexed_x: {

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_lower);
            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_upper);

            address = (addr_upper << 8) + addr_lower + cpu->registers.ir_x;

            operand = NesCpuMemoryRead(&cpu->mem_map,address);

        } break;

        case NesCpuAddressMode::absolute_indexed_y: {

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_lower);
            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_upper);

            address = (addr_upper << 8) + addr_lower + cpu->registers.ir_y;

            operand = NesCpuMemoryRead(&cpu->mem_map,address);

        } break;

        case NesCpuAddressMode::indirect: {

            nes_addr addr_indirect_lower = 0;
            nes_addr addr_indirect_upper = 0;

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_indirect_lower);
            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_indirect_upper);

            nes_addr indirect_addr = (addr_indirect_upper << 8) + addr_indirect_lower;

            addr_lower = NesCpuMemoryRead(&cpu->mem_map, indirect_addr);
            addr_upper = NesCpuMemoryRead(&cpu->mem_map, indirect_addr + 1);

            address = (addr_upper << 8) + addr_lower;

            operand = NesCpuMemoryRead(&cpu->mem_map,address);

        } break;

        //nothing to do, no memory used
        case NesCpuAddressMode::implied: break;
        
        //nothing to do, only uses the accumulator
        case NesCpuAddressMode::accumulator: break;

        case NesCpuAddressMode::immediate: {

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, operand);

        } break;

        case NesCpuAddressMode::relative: {

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, operand);

        } break;

        case NesCpuAddressMode::indexed_indirect: {

            nes_addr zero_page_addr = 0;

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, zero_page_addr);

            nes_addr addr_indirect_lower = NesCpuMemoryRead(&cpu->mem_map,zero_page_addr + cpu->registers.ir_x);
            nes_addr addr_indirect_upper = NesCpuMemoryRead(&cpu->mem_map,zero_page_addr + cpu->registers.ir_x + 1);

            address = (addr_upper << 8) + addr_lower;

            operand = NesCpuMemoryRead(&cpu->mem_map,address);

        } break;

        case NesCpuAddressMode::indirect_indexed: {

            nes_addr zero_page_addr = 0;

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, zero_page_addr);

            nes_addr addr_indirect_lower = NesCpuMemoryRead(&cpu->mem_map,zero_page_addr);
            nes_addr addr_indirect_upper = NesCpuMemoryRead(&cpu->mem_map,zero_page_addr + 1);

            address = (addr_upper << 8) + addr_lower + cpu->registers.ir_y;

            operand = NesCpuMemoryRead(&cpu->mem_map,address);

        } break;

        default: break;
    }

    return operand;
}

internal NesCpu 
nes_cpu_initialize() {

    NesCpu nes_cpu = {0};

    return nes_cpu;
}

internal nes_val
nes_cpu_instr_adc(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_and(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_asl(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_bcc(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_bcs(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_beq(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_bit(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_bmi(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_bne(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_bpl(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_brk(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_bvc(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_bvs(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_clc(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_cld(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_cli(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_clv(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_cmp(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_cpx(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_cpy(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_dec(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_dex(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_dey(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_eor(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_inc(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_inx(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_iny(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_jmp(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_jsr(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_lda(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_ldx(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_ldy(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_lsr(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_nop(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_ora(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_pha(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_php(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_pla(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_plp(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_rol(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_ror(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_rti(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_rts(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_sbc(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_sec(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_sed(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_sei(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_sta(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_stx(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_sty(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_tax(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_tay(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_tsx(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_txa(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_txs(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_tya(NesCpu* cpu, nes_val operand) {
    //TODO
    return 0;
}
