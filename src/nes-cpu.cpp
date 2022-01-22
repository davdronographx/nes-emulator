#include "nes-cpu.hpp"

internal void
nes_cpu_stack_push(NesCpu* cpu, nes_val value) {

    NesCpuMemoryWrite(cpu, cpu->registers.sp, value);
    
    //the stack goes in reverse, so if we are getting down to the beginning
    //of the stack's address space, we need to reset the stack pointer to the
    //top

    //otherwise, we just decrement the stack pointer
    if (cpu->registers.sp == (NES_CPU_MEM_MAP_ZERO_PAGE_ADDR)) {
        cpu->registers.sp += NES_CPU_MEM_MAP_ZERO_PAGE_SIZE;
    } else {
        --cpu->registers.sp;
    }
}

internal nes_val
nes_cpu_stack_pop(NesCpu* cpu) {

    nes_val stack_val = NesCpuMemoryRead(cpu, cpu->registers.sp);

    //update the stack pointer
    //the stack is reverse, so we incriment the pointer
    //if it gets to the top of the stack's address space,
    //it needs to be reset to the bottom 
    if (cpu->registers.sp == (NES_CPU_MEM_MAP_STACK_ADDR + NES_CPU_MEM_MAP_STACK_SIZE)) {
        cpu->registers.sp = NES_CPU_MEM_MAP_STACK_ADDR;
    } else {
        ++cpu->registers.sp;
    }

    return stack_val;
}

internal nes_val*
nes_cpu_decode_operand_address_from_address_mode(NesCpu* cpu, NesCpuAddressMode address_mode) {

    nes_addr address = 0;
    nes_addr addr_lower = 0; 
    nes_addr addr_upper = 0;

    nes_val* operand_address;

    switch(address_mode) {
        
        case NesCpuAddressMode::zero_page: {

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, address);

            operand_address = NesCpuMemoryAddress(cpu,address);

        } break;

        case NesCpuAddressMode::zero_page_indexed_x: {

            NesCpuMemoryReadAndIncrimentProgramCounterIndexed(cpu, cpu->registers.ir_x, address);

            operand_address = NesCpuMemoryAddress(cpu,address);

        } break;

        case NesCpuAddressMode::zero_page_indexed_y: {

            NesCpuMemoryReadAndIncrimentProgramCounterIndexed(cpu, cpu->registers.ir_y, address);

            operand_address = NesCpuMemoryAddress(cpu,address);
            
        } break;

        case NesCpuAddressMode::absolute: {
            
            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_lower);
            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_upper);

            address = (addr_upper << 8) + addr_lower;

            operand_address = NesCpuMemoryAddress(cpu,address);

        } break;

        case NesCpuAddressMode::absolute_indexed_x: {

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_lower);
            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_upper);

            address = (addr_upper << 8) + addr_lower + cpu->registers.ir_x;

            operand_address = NesCpuMemoryAddress(cpu,address);

        } break;

        case NesCpuAddressMode::absolute_indexed_y: {

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_lower);
            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_upper);

            address = (addr_upper << 8) + addr_lower + cpu->registers.ir_y;

            operand_address = NesCpuMemoryAddress(cpu,address);

        } break;

        case NesCpuAddressMode::indirect: {

            nes_addr addr_indirect_lower = 0;
            nes_addr addr_indirect_upper = 0;

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_indirect_lower);
            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_indirect_upper);

            nes_addr indirect_addr = (addr_indirect_upper << 8) + addr_indirect_lower;

            addr_lower = NesCpuMemoryRead(cpu, indirect_addr);
            addr_upper = NesCpuMemoryRead(cpu, indirect_addr + 1);

            address = (addr_upper << 8) + addr_lower;

            operand_address = NesCpuMemoryAddress(cpu,address);

        } break;

        //nothing to do, no memory used
        case NesCpuAddressMode::implied: break;
        
        //nothing to do, only uses the accumulator
        case NesCpuAddressMode::accumulator: break;

            operand_address = &cpu->registers.acc_a;

        case NesCpuAddressMode::immediate: {

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, *operand_address);

        } break;

        case NesCpuAddressMode::relative: {

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, *operand_address);

        } break;

        case NesCpuAddressMode::indexed_indirect_x: {

            nes_addr zero_page_addr = 0;

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, zero_page_addr);

            nes_addr addr_indirect_lower = NesCpuMemoryRead(cpu,zero_page_addr + cpu->registers.ir_x);
            nes_addr addr_indirect_upper = NesCpuMemoryRead(cpu,zero_page_addr + cpu->registers.ir_x + 1);

            address = (addr_upper << 8) + addr_lower;

            operand_address = NesCpuMemoryAddress(cpu,address);

        } break;

        case NesCpuAddressMode::indirect_indexed_y: {

            nes_addr zero_page_addr = 0;

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, zero_page_addr);

            nes_addr addr_indirect_lower = NesCpuMemoryRead(cpu,zero_page_addr);
            nes_addr addr_indirect_upper = NesCpuMemoryRead(cpu,zero_page_addr + 1);

            address = (addr_upper << 8) + addr_lower + cpu->registers.ir_y;

            operand_address = NesCpuMemoryAddress(cpu,address);

        } break;

        default: break;
    }

    return operand_address;
}

internal NesCpu 
nes_cpu_initialize() {

    NesCpu nes_cpu = {0};

    return nes_cpu;
}

internal void
nes_cpu_instr_adc(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {

    nes_val* operand_address = nes_cpu_decode_operand_address_from_address_mode(cpu, addr_mode);

    result->value = cpu->registers.acc_a + *operand_address + NesCpuFlagReadC(cpu);
    
    cpu->registers.acc_a = (nes_val)result->value;

    result->flag_n = TRUE;
    result->flag_z = TRUE;
    result->flag_c = TRUE;
    result->flag_v = TRUE;

    switch (addr_mode)
    {
        case NesCpuAddressMode::immediate:           result->cycles = 2; break;
        case NesCpuAddressMode::zero_page:           result->cycles = 3; break;
        case NesCpuAddressMode::zero_page_indexed_x: result->cycles = 4; break;
        case NesCpuAddressMode::absolute:            result->cycles = 4; break;
        case NesCpuAddressMode::absolute_indexed_x:  result->cycles = 4; break;
        case NesCpuAddressMode::absolute_indexed_y:  result->cycles = 4; break;
        case NesCpuAddressMode::indexed_indirect_x:  result->cycles = 6; break;
        case NesCpuAddressMode::indirect_indexed_y:  result->cycles = 5; break;
        default: result->cycles = 2; break;
    }

}

internal void
nes_cpu_instr_and(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    
    nes_val* operand_address = nes_cpu_decode_operand_address_from_address_mode(cpu, addr_mode);

    result->value = cpu->registers.acc_a & *operand_address;

    cpu->registers.acc_a = (nes_val)result->value;

    result->flag_n = TRUE;
    result->flag_z = TRUE;

    switch (addr_mode)
    {
        case NesCpuAddressMode::immediate:           result->cycles = 2; break;
        case NesCpuAddressMode::zero_page:           result->cycles = 3; break;
        case NesCpuAddressMode::zero_page_indexed_x: result->cycles = 4; break;
        case NesCpuAddressMode::absolute:            result->cycles = 4; break;
        case NesCpuAddressMode::absolute_indexed_x:  result->cycles = 4; break;
        case NesCpuAddressMode::absolute_indexed_y:  result->cycles = 4; break;
        case NesCpuAddressMode::indexed_indirect_x:  result->cycles = 6; break;
        case NesCpuAddressMode::indirect_indexed_y:  result->cycles = 5; break;
        default: result->cycles = 2; break;
    }
}

internal void
nes_cpu_instr_asl(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {

    nes_val* operand_address = nes_cpu_decode_operand_address_from_address_mode(cpu, addr_mode);

    result->value = *operand_address << 1;

    *operand_address = (nes_val)result->value;

    switch (addr_mode) {
        case NesCpuAddressMode::accumulator:         result->cycles = 2; break;
        case NesCpuAddressMode::zero_page:           result->cycles = 5; break;
        case NesCpuAddressMode::zero_page_indexed_x: result->cycles = 6; break;
        case NesCpuAddressMode::absolute:            result->cycles = 6; break;
        case NesCpuAddressMode::absolute_indexed_x:  result->cycles = 7; break;
        default: result->cycles = 2; break;
    }
}

internal void
nes_cpu_instr_bcc(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    
    nes_val* operand_address = nes_cpu_decode_operand_address_from_address_mode(cpu, addr_mode);

    cpu->registers.pc += NesCpuFlagReadC(cpu) == 0 ? *operand_address : 0;

    result->cycles = 2;
}

internal void
nes_cpu_instr_bcs(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    
    nes_val* operand_address = nes_cpu_decode_operand_address_from_address_mode(cpu, addr_mode);

    cpu->registers.pc += NesCpuFlagReadC(cpu) == 1 ? *operand_address : 0;

    result->cycles = 2;
}

internal void
nes_cpu_instr_beq(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    
    nes_val* operand_address = nes_cpu_decode_operand_address_from_address_mode(cpu, addr_mode);

    cpu->registers.pc += NesCpuFlagReadZ(cpu) == 1 ? *operand_address : 0;

    result->cycles = 2;
}

internal nes_val
nes_cpu_instr_bit(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal void
nes_cpu_instr_bmi(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    
    nes_val* operand_address = nes_cpu_decode_operand_address_from_address_mode(cpu, addr_mode);

    cpu->registers.pc += NesCpuFlagReadN(cpu) == 1 ? *operand_address : 0;

    result->cycles = 2;
}

internal void
nes_cpu_instr_bne(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    
    nes_val* operand_address = nes_cpu_decode_operand_address_from_address_mode(cpu, addr_mode);

    cpu->registers.pc += NesCpuFlagReadZ(cpu) == 0 ? *operand_address : 0;

    result->cycles = 2;
}

internal void
nes_cpu_instr_bpl(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    
    nes_val* operand_address = nes_cpu_decode_operand_address_from_address_mode(cpu, addr_mode);

    cpu->registers.pc += NesCpuFlagReadN(cpu) == 0 ? *operand_address : 0;

    result->cycles = 2;
}

internal nes_val
nes_cpu_instr_brk(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_bvc(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_bvs(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_clc(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_cld(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_cli(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_clv(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_cmp(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_cpx(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_cpy(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_dec(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_dex(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_dey(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_eor(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_inc(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_inx(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_iny(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_jmp(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_jsr(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_lda(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_ldx(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_ldy(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_lsr(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_nop(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_ora(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_pha(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_php(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_pla(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_plp(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_rol(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_ror(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_rti(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_rts(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_sbc(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_sec(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_sed(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_sei(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_sta(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_stx(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_sty(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_tax(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_tay(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_tsx(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_txa(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_txs(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal nes_val
nes_cpu_instr_tya(NesCpu* cpu, NesCpuAddressMode addr_mode, NesCpuInstrResult* result) {
    //TODO
    return 0;
}

internal void
nes_cpu_instr_execute(NesCpu* cpu, nes_val instruction) {

    NesCpuInstrResult result = {0};

    switch (instruction) {

        case ADC_I:     nes_cpu_instr_adc(cpu, NesCpuAddressMode::immediate,&result); break;
        case ADC_ZP:    nes_cpu_instr_adc(cpu, NesCpuAddressMode::zero_page,&result); break;
        case ADC_ZP_X:  nes_cpu_instr_adc(cpu, NesCpuAddressMode::zero_page_indexed_x,&result); break;
        case ADC_ABS:   nes_cpu_instr_adc(cpu, NesCpuAddressMode::absolute,&result); break;
        case ADC_ABS_X: nes_cpu_instr_adc(cpu, NesCpuAddressMode::absolute_indexed_x,&result); break;
        case ADC_ABS_Y: nes_cpu_instr_adc(cpu, NesCpuAddressMode::absolute_indexed_y,&result); break;
        case ADC_IND_X: nes_cpu_instr_adc(cpu, NesCpuAddressMode::indexed_indirect_x,&result); break;
        case ADC_IND_Y: nes_cpu_instr_adc(cpu, NesCpuAddressMode::indirect_indexed_y,&result); break;

        case AND_I:     nes_cpu_instr_and(cpu, NesCpuAddressMode::immediate,&result); break;
        case AND_ZP:    nes_cpu_instr_and(cpu, NesCpuAddressMode::zero_page,&result); break;
        case AND_ZP_X:  nes_cpu_instr_and(cpu, NesCpuAddressMode::zero_page_indexed_x,&result); break;
        case AND_ABS:   nes_cpu_instr_and(cpu, NesCpuAddressMode::absolute,&result); break;
        case AND_ABS_X: nes_cpu_instr_and(cpu, NesCpuAddressMode::absolute_indexed_x,&result); break;
        case AND_ABS_Y: nes_cpu_instr_and(cpu, NesCpuAddressMode::absolute_indexed_y,&result); break;
        case AND_IND_X: nes_cpu_instr_and(cpu, NesCpuAddressMode::indexed_indirect_x,&result); break;
        case AND_IND_Y: nes_cpu_instr_and(cpu, NesCpuAddressMode::indirect_indexed_y,&result); break;

        case ASL_ACC:   nes_cpu_instr_asl(cpu, NesCpuAddressMode::accumulator, &result); break;
        case ASL_ZP:    nes_cpu_instr_asl(cpu, NesCpuAddressMode::zero_page, &result); break;
        case ASL_ZPX:   nes_cpu_instr_asl(cpu, NesCpuAddressMode::zero_page_indexed_x, &result); break;
        case ASL_ABS:   nes_cpu_instr_asl(cpu, NesCpuAddressMode::absolute, &result); break;
        case ASL_ABS_X: nes_cpu_instr_asl(cpu, NesCpuAddressMode::absolute_indexed_x, &result); break;

        //branches
        case BCC_REL:   nes_cpu_instr_bcc(cpu, NesCpuAddressMode::relative, &result); break;
        case BCS_REL:   nes_cpu_instr_bcs(cpu, NesCpuAddressMode::relative, &result); break;
        case BEQ_REL:   nes_cpu_instr_beq(cpu, NesCpuAddressMode::relative, &result); break;
        case BMI_REL:   nes_cpu_instr_bmi(cpu, NesCpuAddressMode::relative, &result); break;
        case BNE_REL:   nes_cpu_instr_bne(cpu, NesCpuAddressMode::relative, &result); break;
        case BPL_REL:   nes_cpu_instr_bpl(cpu, NesCpuAddressMode::relative, &result); break;




        //default assume NOP
        default: nes_cpu_instr_nop(cpu, NesCpuAddressMode::immediate, &result); break;
    }
}