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
nes_cpu_stack_pull(NesCpu* cpu) {

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

internal void
nes_cpu_interrupt(NesCpu* cpu, NesCpuInterruptType interrupt_type) {

    if (interrupt_type == NesCpuInterruptType::IRQ) {
        //this is a software interrupt, we start by pushing PC+2 to the stack as the return address
        //PC+1 is spacing for a break mark and is not needed
        nes_cpu_stack_push(cpu, NesCpuMemoryRead(cpu, cpu->registers.pc+2));
    } 
    else {
        //push the program counter onto the stack
        nes_cpu_stack_push(cpu, cpu->registers.pc);
    }

    //push the status register onto the stack
    nes_cpu_stack_push(cpu, cpu->registers.p);    

    //set the address for the interrupt routine
    nes_val interrupt_routine_addr_lsb = 0;
    nes_val interrupt_routine_addr_msb = 0;
    switch (interrupt_type) {
        case NesCpuInterruptType::IRQ: {
            interrupt_routine_addr_lsb = NES_CPU_IRQ_ADDR_LSB;
            interrupt_routine_addr_msb = NES_CPU_IRQ_ADDR_MSB;
        } break;
        case NesCpuInterruptType::NMI: {
            interrupt_routine_addr_lsb = NES_CPU_NMI_ADDR_LSB;
            interrupt_routine_addr_msb = NES_CPU_NMI_ADDR_MSB;
        } break;
        case NesCpuInterruptType::RST: {
            interrupt_routine_addr_lsb = NES_CPU_RST_ADDR_LSB;
            interrupt_routine_addr_msb = NES_CPU_RST_ADDR_MSB;
        } break;
        default: {
            //by default hardware reset
            interrupt_routine_addr_lsb = NES_CPU_RST_ADDR_LSB;
            interrupt_routine_addr_msb = NES_CPU_RST_ADDR_MSB;
        } break;
    }

    cpu->registers.pc = interrupt_routine_addr_msb;
    cpu->registers.pc <<= 8;
    cpu->registers.pc += interrupt_routine_addr_lsb;

    //clear the other status values
    NesCpuFlagClearC(cpu);
    NesCpuFlagClearZ(cpu);
    NesCpuFlagClearI(cpu);
    NesCpuFlagClearD(cpu);
    NesCpuFlagClearB(cpu);
    NesCpuFlagClearV(cpu);
    NesCpuFlagClearN(cpu);

    //set the result interrupt disable flag to true
    NesCpuFlagSetI(cpu);
}

internal void
nes_cpu_decode_operand_address_from_address_mode(NesCpu* cpu) {

    nes_addr address = 0;
    nes_addr addr_lower = 0; 
    nes_addr addr_upper = 0;

    switch(cpu->current_instr.addr_mode) {
        
        case NesCpuAddressMode::zero_page: {

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, address);

            cpu->current_instr.operand_addr = NesCpuMemoryAddress(cpu,address);

        } break;

        case NesCpuAddressMode::zero_page_indexed_x: {

            NesCpuMemoryReadAndIncrimentProgramCounterIndexed(cpu, cpu->registers.ir_x, address);

            cpu->current_instr.operand_addr = NesCpuMemoryAddress(cpu,address);

        } break;

        case NesCpuAddressMode::zero_page_indexed_y: {

            NesCpuMemoryReadAndIncrimentProgramCounterIndexed(cpu, cpu->registers.ir_y, address);

            cpu->current_instr.operand_addr = NesCpuMemoryAddress(cpu,address);
            
        } break;

        case NesCpuAddressMode::absolute: {
            
            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_lower);
            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_upper);

            address = (addr_upper << 8) + addr_lower;

            cpu->current_instr.operand_addr = NesCpuMemoryAddress(cpu,address);

        } break;

        case NesCpuAddressMode::absolute_indexed_x: {

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_lower);
            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_upper);

            address = (addr_upper << 8) + addr_lower + cpu->registers.ir_x;

            cpu->current_instr.operand_addr = NesCpuMemoryAddress(cpu,address);

            cpu->current_instr.result.page_boundary_crossed = 
                (((addr_upper << 8) + addr_lower) & 0xFF00) != (address * 0xFF00);

        } break;

        case NesCpuAddressMode::absolute_indexed_y: {

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_lower);
            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, addr_upper);

            address = (addr_upper << 8) + addr_lower + cpu->registers.ir_y;

            cpu->current_instr.operand_addr = NesCpuMemoryAddress(cpu,address);

            cpu->current_instr.result.page_boundary_crossed = 
                (((addr_upper << 8) + addr_lower) & 0xFF00) != (address * 0xFF00);

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

            cpu->current_instr.operand_addr = NesCpuMemoryAddress(cpu,address);

        } break;

        //nothing to do, no memory used
        case NesCpuAddressMode::implied: break;
        
        case NesCpuAddressMode::accumulator: break;

            cpu->current_instr.operand_addr = &cpu->registers.acc_a;

        case NesCpuAddressMode::immediate: {

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, *cpu->current_instr.operand_addr);

        } break;

        case NesCpuAddressMode::relative: {

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, *cpu->current_instr.operand_addr);

        } break;

        case NesCpuAddressMode::indexed_indirect_x: {

            nes_addr zero_page_addr = 0;

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, zero_page_addr);

            nes_addr addr_indirect_lower = NesCpuMemoryRead(cpu,zero_page_addr + cpu->registers.ir_x);
            nes_addr addr_indirect_upper = NesCpuMemoryRead(cpu,zero_page_addr + cpu->registers.ir_x + 1);

            address = (addr_indirect_upper << 8) + addr_indirect_lower;

            cpu->current_instr.operand_addr = NesCpuMemoryAddress(cpu,address);

        } break;

        case NesCpuAddressMode::indirect_indexed_y: {

            nes_addr zero_page_addr = 0;

            NesCpuMemoryReadAndIncrimentProgramCounter(cpu, zero_page_addr);

            nes_addr addr_indirect_lower = NesCpuMemoryRead(cpu,zero_page_addr);
            nes_addr addr_indirect_upper = NesCpuMemoryRead(cpu,zero_page_addr + 1);

            address = (addr_indirect_upper << 8) + addr_indirect_lower + cpu->registers.ir_y;

            cpu->current_instr.operand_addr = NesCpuMemoryAddress(cpu,address);

            cpu->current_instr.result.page_boundary_crossed = 
                (((addr_indirect_upper << 8) + addr_indirect_lower) & 0xFF00) != (address * 0xFF00);

        } break;

        default: break;
    }
}

internal void
nes_cpu_branch_and_update_cycles(NesCpu* cpu) {

        //add 1 cycle for branching to same page, add 2 cycles for branching to different page
        cpu->current_instr.result.branch_cycles = 
            (cpu->registers.pc & 0xFF00) == ((cpu->registers.pc + *cpu->current_instr.operand_addr) & 0xFF00)
            ? 1
            : 2;     

        //update program counter
        cpu->registers.pc += *cpu->current_instr.operand_addr;
}

internal void
nes_cpu_instr_adc(NesCpu* cpu) {

    cpu->current_instr.result.value = cpu->registers.acc_a + *cpu->current_instr.operand_addr + NesCpuFlagReadC(cpu);
    
    cpu->registers.acc_a = (nes_val)cpu->current_instr.result.value;

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
    cpu->current_instr.result.flag_c = TRUE;
    cpu->current_instr.result.flag_v = TRUE;
}

internal void
nes_cpu_instr_and(NesCpu* cpu) {

    cpu->current_instr.result.value = cpu->registers.acc_a & *cpu->current_instr.operand_addr;

    cpu->registers.acc_a = (nes_val)cpu->current_instr.result.value;

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_asl(NesCpu* cpu) {

    cpu->current_instr.result.value = *cpu->current_instr.operand_addr << 1;

    *cpu->current_instr.operand_addr = (nes_val)cpu->current_instr.result.value;
}

internal void
nes_cpu_instr_bcc(NesCpu* cpu) {

    if (NesCpuFlagReadC(cpu) == 0) {
        nes_cpu_branch_and_update_cycles(cpu);
    }
}

internal void
nes_cpu_instr_bcs(NesCpu* cpu) {

    if (NesCpuFlagReadC(cpu) == 1) {
        nes_cpu_branch_and_update_cycles(cpu);
    }
}

internal void
nes_cpu_instr_beq(NesCpu* cpu) {

    if (NesCpuFlagReadZ(cpu) == 1) {
        nes_cpu_branch_and_update_cycles(cpu);
    }
}

internal void
nes_cpu_instr_bit(NesCpu* cpu) {

    //transfer bit 6 of operand to V flag
    NesCpuFlagClearV(cpu);
    if (ReadBitInByte(6, *cpu->current_instr.operand_addr) == 1) {
        NesCpuFlagSetV(cpu);
    }

    //transfer bit 7 of operand to N flag
    NesCpuFlagClearN(cpu);
    if (ReadBitInByte(7, *cpu->current_instr.operand_addr) == 1) {
        NesCpuFlagSetN(cpu);
    }

    //the result of A and Operand will be used for the zero flag
    cpu->current_instr.result.value = (cpu->registers.acc_a & *cpu->current_instr.operand_addr);
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_bmi(NesCpu* cpu) {

    if (NesCpuFlagReadN(cpu) == 1) {
        nes_cpu_branch_and_update_cycles(cpu);
    }
}

internal void
nes_cpu_instr_bne(NesCpu* cpu) {

    if (NesCpuFlagReadZ(cpu) == 0) {
        nes_cpu_branch_and_update_cycles(cpu);
    }
}

internal void
nes_cpu_instr_bpl(NesCpu* cpu) {
    
    if (NesCpuFlagReadN(cpu) == 0) {
        nes_cpu_branch_and_update_cycles(cpu);
    }
}

internal void
nes_cpu_instr_brk(NesCpu* cpu) {

    cpu->current_instr.result.flag_b = TRUE;
}

internal void
nes_cpu_instr_bvc(NesCpu* cpu) {
    
    cpu->registers.pc += NesCpuFlagReadV(cpu) == 0 ? *cpu->current_instr.operand_addr : 0;
}

internal void
nes_cpu_instr_bvs(NesCpu* cpu) {
    
    cpu->registers.pc += NesCpuFlagReadV(cpu) == 1 ? *cpu->current_instr.operand_addr : 0;
}

internal void
nes_cpu_instr_clc(NesCpu* cpu) {
    
    NesCpuFlagClearC(cpu);
}

internal void
nes_cpu_instr_cld(NesCpu* cpu) {
    
    NesCpuFlagClearD(cpu);
}

internal void
nes_cpu_instr_cli(NesCpu* cpu) {

    NesCpuFlagClearI(cpu);
}

internal void
nes_cpu_instr_clv(NesCpu* cpu) {
    
    NesCpuFlagClearV(cpu);
}

internal void
nes_cpu_instr_cmp(NesCpu* cpu) {
    
    cpu->current_instr.result.value = (cpu->registers.acc_a - *cpu->current_instr.operand_addr);

    cpu->current_instr.result.flag_c = TRUE;
    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_cpx(NesCpu* cpu) {
    
    cpu->current_instr.result.value = (cpu->registers.ir_x - *cpu->current_instr.operand_addr);
    
    cpu->current_instr.result.flag_c = TRUE;
    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_cpy(NesCpu* cpu) {
    
    cpu->current_instr.result.value = (cpu->registers.ir_y - *cpu->current_instr.operand_addr);
    
    cpu->current_instr.result.flag_c = TRUE;
    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_dec(NesCpu* cpu) {

    --(*cpu->current_instr.operand_addr);

    cpu->current_instr.result.value = *cpu->current_instr.operand_addr;

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_dex(NesCpu* cpu) {
    
    --cpu->registers.ir_x;

    cpu->current_instr.result.value = cpu->registers.ir_x;

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_dey(NesCpu* cpu) {
    
    --cpu->registers.ir_y;

    cpu->current_instr.result.value = cpu->registers.ir_y;

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_eor(NesCpu* cpu) {
 
    cpu->current_instr.result.value = (cpu->registers.acc_a ^ *cpu->current_instr.operand_addr);

    cpu->registers.acc_a = (nes_val)cpu->current_instr.result.value;
 
    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_inc(NesCpu* cpu) {
    
    ++(*cpu->current_instr.operand_addr);

    cpu->current_instr.result.value = *cpu->current_instr.operand_addr;

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_inx(NesCpu* cpu) {

    ++cpu->registers.ir_x;

    cpu->current_instr.result.value = cpu->registers.ir_x;

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_iny(NesCpu* cpu) {
    
    ++cpu->registers.ir_y;

    cpu->current_instr.result.value = cpu->registers.ir_y;

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_jmp(NesCpu* cpu) {

    cpu->registers.pc = *cpu->current_instr.operand_addr;
}

internal void
nes_cpu_instr_jsr(NesCpu* cpu) {

    nes_cpu_stack_push(cpu, cpu->registers.sp);

    cpu->registers.pc = *cpu->current_instr.operand_addr;
}

internal void
nes_cpu_instr_lda(NesCpu* cpu) {

    cpu->registers.acc_a = *cpu->current_instr.operand_addr;

    cpu->current_instr.result.value = cpu->registers.acc_a; 

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal nes_val
nes_cpu_instr_ldx(NesCpu* cpu) {

    cpu->registers.ir_x = *cpu->current_instr.operand_addr;

    cpu->current_instr.result.value = cpu->registers.ir_x; 

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal nes_val
nes_cpu_instr_ldy(NesCpu* cpu) {

    cpu->registers.ir_y = *cpu->current_instr.operand_addr;

    cpu->current_instr.result.value = cpu->registers.ir_y; 

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_lsr(NesCpu* cpu) {
    
    cpu->current_instr.result.value = *cpu->current_instr.operand_addr;
    cpu->current_instr.result.value >>= 1;
    
    *cpu->current_instr.operand_addr = (nes_val)cpu->current_instr.result.value;

    cpu->current_instr.result.flag_c = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
    NesCpuFlagClearN(cpu);
}

internal void
nes_cpu_instr_nop(NesCpu* cpu) {
    
    //¯\_(ツ)_/¯
}

internal nes_val
nes_cpu_instr_ora(NesCpu* cpu) {
    
    cpu->current_instr.result.value = (cpu->registers.acc_a | *cpu->current_instr.operand_addr);

    cpu->registers.acc_a = (nes_val)cpu->current_instr.result.value;
 
    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_pha(NesCpu* cpu) {

    nes_cpu_stack_push(cpu, cpu->registers.acc_a);
}

internal void
nes_cpu_instr_php(NesCpu* cpu) {
    
    nes_cpu_stack_push(cpu, cpu->registers.p);
}

internal void 
nes_cpu_instr_pla(NesCpu* cpu) {
    
    cpu->registers.acc_a = nes_cpu_stack_pull(cpu);

    cpu->current_instr.result.value = cpu->registers.acc_a;

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_plp(NesCpu* cpu) {
    
    cpu->registers.p = nes_cpu_stack_pull(cpu);
}

internal nes_val
nes_cpu_instr_rol(NesCpu* cpu) {

    cpu->current_instr.result.value = *cpu->current_instr.operand_addr;
    cpu->current_instr.result.value <<= 1;
    
    *cpu->current_instr.operand_addr = (nes_val)cpu->current_instr.result.value;

    *cpu->current_instr.operand_addr += NesCpuFlagReadC(cpu);

    cpu->current_instr.result.flag_c = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
    cpu->current_instr.result.flag_n = TRUE;
}

internal nes_val
nes_cpu_instr_ror(NesCpu* cpu) {
    
    cpu->current_instr.result.value = *cpu->current_instr.operand_addr;
    cpu->current_instr.result.value >>= 1;
    cpu->current_instr.result.value += (NesCpuFlagReadC(cpu) << 8);

    cpu->current_instr.result.value = *cpu->current_instr.operand_addr;

    cpu->current_instr.result.flag_c = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
    cpu->current_instr.result.flag_n = TRUE;
    
    return 0;
}

internal void
nes_cpu_instr_rti(NesCpu* cpu) {
    
    cpu->registers.p = nes_cpu_stack_pull(cpu);
    cpu->registers.pc = nes_cpu_stack_pull(cpu);

    NesCpuFlagClearB(cpu);
}

internal void
nes_cpu_instr_rts(NesCpu* cpu) {

    cpu->registers.pc = nes_cpu_stack_pull(cpu) + 1;
}

internal void
nes_cpu_instr_sbc(NesCpu* cpu) {
    
    cpu->current_instr.result.value = cpu->registers.acc_a - *cpu->current_instr.operand_addr - NesCpuFlagReadC(cpu);
    
    cpu->registers.acc_a = (nes_val)cpu->current_instr.result.value;

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
    cpu->current_instr.result.flag_c = TRUE;
    cpu->current_instr.result.flag_v = TRUE;
    
}

internal void
nes_cpu_instr_sec(NesCpu* cpu) {
    
    NesCpuFlagSetC(cpu);
}

internal void
nes_cpu_instr_sed(NesCpu* cpu) {
    
    NesCpuFlagSetD(cpu);
}

internal void
nes_cpu_instr_sei(NesCpu* cpu) {
    
    NesCpuFlagSetI(cpu);
}

internal void
nes_cpu_instr_sta(NesCpu* cpu) {
    
    *cpu->current_instr.operand_addr = cpu->registers.acc_a;
}

internal void
nes_cpu_instr_stx(NesCpu* cpu) {
    
    *cpu->current_instr.operand_addr = cpu->registers.ir_x;
}

internal void
nes_cpu_instr_sty(NesCpu* cpu) {
    
    *cpu->current_instr.operand_addr = cpu->registers.ir_y;
}

internal void
nes_cpu_instr_tax(NesCpu* cpu) {
    
    cpu->registers.ir_x = cpu->registers.acc_a;

    cpu->current_instr.result.value = cpu->registers.ir_x;

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_tay(NesCpu* cpu) {

    cpu->registers.ir_y = cpu->registers.acc_a;

    cpu->current_instr.result.value = cpu->registers.ir_y;

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_tsx(NesCpu* cpu) {

    cpu->registers.ir_x = cpu->registers.sp;

    cpu->current_instr.result.value = cpu->registers.ir_x;

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_instr_txa(NesCpu* cpu) {
    
    cpu->registers.acc_a = cpu->registers.ir_x;

    cpu->current_instr.result.value = cpu->registers.acc_a;

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void 
nes_cpu_instr_txs(NesCpu* cpu) {
    
    cpu->registers.sp = cpu->registers.ir_x;
}

internal void
nes_cpu_instr_tya(NesCpu* cpu) {

    cpu->registers.acc_a = cpu->registers.ir_y;

    cpu->current_instr.result.value = cpu->registers.acc_a;

    cpu->current_instr.result.flag_n = TRUE;
    cpu->current_instr.result.flag_z = TRUE;
}

internal void
nes_cpu_addr_mode_decode_from_instr(NesCpu* cpu) {

    switch (cpu->current_instr.op_code) {

        //accumulator
        case NES_CPU_INSTR_ASL_ACC:
        case NES_CPU_INSTR_LSR_ACC:
        case NES_CPU_INSTR_ROL_ACC:
        case NES_CPU_INSTR_ROR_ACC: {
            cpu->current_instr.addr_mode = NesCpuAddressMode::accumulator;
        } break;

        //absolute
        case NES_CPU_INSTR_ADC_ABS:
        case NES_CPU_INSTR_AND_ABS:
        case NES_CPU_INSTR_ASL_ABS:
        case NES_CPU_INSTR_BIT_ABS:
        case NES_CPU_INSTR_CMP_ABS:
        case NES_CPU_INSTR_CPY_ABS:
        case NES_CPU_INSTR_DEC_ABS:
        case NES_CPU_INSTR_EOR_ABS:
        case NES_CPU_INSTR_INC_ABS:
        case NES_CPU_INSTR_JMP_ABS:
        case NES_CPU_INSTR_JSR_ABS:
        case NES_CPU_INSTR_LDA_ABS:
        case NES_CPU_INSTR_LDX_ABS:
        case NES_CPU_INSTR_LDY_ABS:
        case NES_CPU_INSTR_LSR_ABS:
        case NES_CPU_INSTR_ORA_ABS:
        case NES_CPU_INSTR_ROL_ABS:
        case NES_CPU_INSTR_ROR_ABS:
        case NES_CPU_INSTR_SBC_ABS:
        case NES_CPU_INSTR_STA_ABS:
        case NES_CPU_INSTR_STX_ABS:
        case NES_CPU_INSTR_STY_ABS: {
            cpu->current_instr.addr_mode = NesCpuAddressMode::absolute;
        } break;

        //absolute x indexed
        case NES_CPU_INSTR_ADC_ABS_X:
        case NES_CPU_INSTR_AND_ABS_X:
        case NES_CPU_INSTR_ASL_ABS_X:
        case NES_CPU_INSTR_CMP_ABS_X:
        case NES_CPU_INSTR_DEC_ABS_X:
        case NES_CPU_INSTR_EOR_ABS_X:
        case NES_CPU_INSTR_INC_ABS_X:
        case NES_CPU_INSTR_LDA_ABS_X:
        case NES_CPU_INSTR_LDY_ABS_X:
        case NES_CPU_INSTR_LSR_ABS_X:
        case NES_CPU_INSTR_ORA_ABS_X:
        case NES_CPU_INSTR_ROL_ABS_X:
        case NES_CPU_INSTR_ROR_ABS_X:
        case NES_CPU_INSTR_SBC_ABS_X:
        case NES_CPU_INSTR_STA_ABS_X: {
            cpu->current_instr.addr_mode = NesCpuAddressMode::absolute_indexed_x;
        } break;

        //absolute y indexed
        case NES_CPU_INSTR_ADC_ABS_Y:
        case NES_CPU_INSTR_AND_ABS_Y:
        case NES_CPU_INSTR_CMP_ABS_Y:
        case NES_CPU_INSTR_EOR_ABS_Y:
        case NES_CPU_INSTR_LDA_ABS_Y:
        case NES_CPU_INSTR_LDX_ABS_Y:
        case NES_CPU_INSTR_ORA_ABS_Y:
        case NES_CPU_INSTR_SBC_ABS_Y:
        case NES_CPU_INSTR_STA_ABS_Y: {
            cpu->current_instr.addr_mode = NesCpuAddressMode::absolute_indexed_y;
        } break;

        //immediate
        case NES_CPU_INSTR_ADC_IMM:
        case NES_CPU_INSTR_AND_IMM:
        case NES_CPU_INSTR_CMP_IMM:
        case NES_CPU_INSTR_CPY_IMM:
        case NES_CPU_INSTR_EOR_IMM:
        case NES_CPU_INSTR_LDA_IMM:
        case NES_CPU_INSTR_LDX_IMM:
        case NES_CPU_INSTR_LDY_IMM:
        case NES_CPU_INSTR_ORA_IMM:
        case NES_CPU_INSTR_SBC_IMM: {
            cpu->current_instr.addr_mode = NesCpuAddressMode::immediate;
        } break;

        //implied
        case NES_CPU_INSTR_BRK_IMP:
        case NES_CPU_INSTR_CLC_IMP:
        case NES_CPU_INSTR_CLD_IMP:
        case NES_CPU_INSTR_CLI_IMP:
        case NES_CPU_INSTR_CLV_IMP:
        case NES_CPU_INSTR_DEX_IMP:
        case NES_CPU_INSTR_DEY_IMP:
        case NES_CPU_INSTR_INX_IMP:
        case NES_CPU_INSTR_INY_IMP:
        case NES_CPU_INSTR_NOP_IMP:
        case NES_CPU_INSTR_PHA_IMP:
        case NES_CPU_INSTR_PHP_IMP:
        case NES_CPU_INSTR_PLA_IMP:
        case NES_CPU_INSTR_PLP_IMP:
        case NES_CPU_INSTR_RTI_IMP:
        case NES_CPU_INSTR_RTS_IMP:
        case NES_CPU_INSTR_SEC_IMP:
        case NES_CPU_INSTR_SED_IMP:
        case NES_CPU_INSTR_SEI_IMP:
        case NES_CPU_INSTR_TAX_IMP:
        case NES_CPU_INSTR_TAY_IMP:
        case NES_CPU_INSTR_TSX_IMP:
        case NES_CPU_INSTR_TXA_IMP:
        case NES_CPU_INSTR_TXS_IMP:
        case NES_CPU_INSTR_TYA_IMP: {
            cpu->current_instr.addr_mode = NesCpuAddressMode::implied;
        } break;

        //indirect
        case NES_CPU_INSTR_JMP_IND: {
            cpu->current_instr.addr_mode = NesCpuAddressMode::indirect;
        } break;

        //indirect x
        case NES_CPU_INSTR_ADC_IND_X:
        case NES_CPU_INSTR_AND_IND_X:
        case NES_CPU_INSTR_CMP_IND_X:
        case NES_CPU_INSTR_EOR_IND_X:
        case NES_CPU_INSTR_LDA_IND_X:
        case NES_CPU_INSTR_ORA_IND_X:
        case NES_CPU_INSTR_SBC_IND_X:
        case NES_CPU_INSTR_STA_IND_X: {
            cpu->current_instr.addr_mode = NesCpuAddressMode::indexed_indirect_x;
        } break;

        //indirect y
        case NES_CPU_INSTR_ADC_IND_Y:        
        case NES_CPU_INSTR_AND_IND_Y:
        case NES_CPU_INSTR_CMP_IND_Y:
        case NES_CPU_INSTR_EOR_IND_Y:
        case NES_CPU_INSTR_LDA_IND_Y:
        case NES_CPU_INSTR_ORA_IND_Y:
        case NES_CPU_INSTR_SBC_IND_Y:
        case NES_CPU_INSTR_STA_IND_Y: {
            cpu->current_instr.addr_mode = NesCpuAddressMode::indirect_indexed_y;
        } break;

        //relative
        case NES_CPU_INSTR_BCC_REL:
        case NES_CPU_INSTR_BCS_REL:
        case NES_CPU_INSTR_BEQ_REL:
        case NES_CPU_INSTR_BMI_REL:
        case NES_CPU_INSTR_BNE_REL:
        case NES_CPU_INSTR_BPL_REL: 
        case NES_CPU_INSTR_BVC_REL:
        case NES_CPU_INSTR_BVS_REL: {
            cpu->current_instr.addr_mode = NesCpuAddressMode::relative;
        } break;

        //zero paged
        case NES_CPU_INSTR_ADC_ZP:
        case NES_CPU_INSTR_AND_ZP:
        case NES_CPU_INSTR_ASL_ZP:
        case NES_CPU_INSTR_BIT_ZP:
        case NES_CPU_INSTR_CMP_ZP:
        case NES_CPU_INSTR_CPY_ZP:
        case NES_CPU_INSTR_DEC_ZP:
        case NES_CPU_INSTR_EOR_ZP:
        case NES_CPU_INSTR_INC_ZP:
        case NES_CPU_INSTR_LDA_ZP:
        case NES_CPU_INSTR_LDX_ZP:
        case NES_CPU_INSTR_LDY_ZP:
        case NES_CPU_INSTR_LSR_ZP:
        case NES_CPU_INSTR_ORA_ZP:
        case NES_CPU_INSTR_ROL_ZP:
        case NES_CPU_INSTR_ROR_ZP:
        case NES_CPU_INSTR_SBC_ZP:
        case NES_CPU_INSTR_STA_ZP:
        case NES_CPU_INSTR_STX_ZP:
        case NES_CPU_INSTR_STY_ZP: {
            cpu->current_instr.addr_mode = NesCpuAddressMode::zero_page;
        } break; 

        //zero paged x indexed
        case NES_CPU_INSTR_ADC_ZP_X:
        case NES_CPU_INSTR_AND_ZP_X:
        case NES_CPU_INSTR_ASL_ZP_X:
        case NES_CPU_INSTR_CMP_ZP_X:
        case NES_CPU_INSTR_DEC_ZP_X:
        case NES_CPU_INSTR_EOR_ZP_X:
        case NES_CPU_INSTR_INC_ZP_X:
        case NES_CPU_INSTR_LDA_ZP_X:
        case NES_CPU_INSTR_LDY_ZP_X:
        case NES_CPU_INSTR_LSR_ZP_X:
        case NES_CPU_INSTR_ORA_ZP_X:
        case NES_CPU_INSTR_ROL_ZP_X:
        case NES_CPU_INSTR_ROR_ZP_X:
        case NES_CPU_INSTR_SBC_ZP_X:
        case NES_CPU_INSTR_STA_ZP_X:
        case NES_CPU_INSTR_STY_ZP_X: {
            cpu->current_instr.addr_mode = NesCpuAddressMode::zero_page_indexed_x;
        } break;

        //zero paged y indexed
        case NES_CPU_INSTR_LDX_ZP_Y:
        case NES_CPU_INSTR_STX_ZP_Y: {
            cpu->current_instr.addr_mode = NesCpuAddressMode::zero_page_indexed_y;
        } break;
    }
}

internal void 
nes_cpu_instr_execute(NesCpu* cpu) {

    switch (cpu->current_instr.op_code) {

        case NES_CPU_INSTR_ADC_IMM:   nes_cpu_instr_adc(cpu); cpu->current_instr.result.cycles = 2; break;  
        case NES_CPU_INSTR_ADC_ZP:    nes_cpu_instr_adc(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_ADC_ZP_X:  nes_cpu_instr_adc(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_ADC_ABS:   nes_cpu_instr_adc(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_ADC_ABS_X: nes_cpu_instr_adc(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_ADC_ABS_Y: nes_cpu_instr_adc(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_ADC_IND_X: nes_cpu_instr_adc(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_ADC_IND_Y: nes_cpu_instr_adc(cpu); cpu->current_instr.result.cycles = 5; break;

        case NES_CPU_INSTR_AND_IMM:   nes_cpu_instr_and(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_AND_ZP:    nes_cpu_instr_and(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_AND_ZP_X:  nes_cpu_instr_and(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_AND_ABS:   nes_cpu_instr_and(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_AND_ABS_X: nes_cpu_instr_and(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_AND_ABS_Y: nes_cpu_instr_and(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_AND_IND_X: nes_cpu_instr_and(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_AND_IND_Y: nes_cpu_instr_and(cpu); cpu->current_instr.result.cycles = 5; break;

        case NES_CPU_INSTR_ASL_ACC:   nes_cpu_instr_asl(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_ASL_ZP:    nes_cpu_instr_asl(cpu); cpu->current_instr.result.cycles = 5; break;
        case NES_CPU_INSTR_ASL_ZP_X:  nes_cpu_instr_asl(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_ASL_ABS:   nes_cpu_instr_asl(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_ASL_ABS_X: nes_cpu_instr_asl(cpu); cpu->current_instr.result.cycles = 7; break;

        //branches
        case NES_CPU_INSTR_BCC_REL:   nes_cpu_instr_bcc(cpu); cpu->current_instr.result.cycles = 2; break; 
        case NES_CPU_INSTR_BCS_REL:   nes_cpu_instr_bcs(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_BEQ_REL:   nes_cpu_instr_beq(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_BMI_REL:   nes_cpu_instr_bmi(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_BNE_REL:   nes_cpu_instr_bne(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_BPL_REL:   nes_cpu_instr_bpl(cpu); cpu->current_instr.result.cycles = 2; break; 
        case NES_CPU_INSTR_BVC_REL:   nes_cpu_instr_bvc(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_BVS_REL:   nes_cpu_instr_bvs(cpu); cpu->current_instr.result.cycles = 2; break;

        case NES_CPU_INSTR_BRK_IMP:   nes_cpu_instr_brk(cpu); cpu->current_instr.result.cycles = 7; break;

        case NES_CPU_INSTR_CLC_IMP:   nes_cpu_instr_clc(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_CLD_IMP:   nes_cpu_instr_cld(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_CLI_IMP:   nes_cpu_instr_cli(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_CLV_IMP:   nes_cpu_instr_clv(cpu); cpu->current_instr.result.cycles = 2; break;

        case NES_CPU_INSTR_CMP_IMM:   nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 2; break;   
        case NES_CPU_INSTR_CMP_ZP:    nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_CMP_ZP_X:  nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_CMP_ABS:   nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_CMP_ABS_X: nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_CMP_ABS_Y: nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_CMP_IND_X: nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_CMP_IND_Y: nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 2; break;

        case NES_CPU_INSTR_CPX_IMM:   nes_cpu_instr_cpx(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_CPX_ZP:    nes_cpu_instr_cpx(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_CPX_ABS:   nes_cpu_instr_cpx(cpu); cpu->current_instr.result.cycles = 4; break;

        case NES_CPU_INSTR_CPY_IMM:   nes_cpu_instr_cpy(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_CPY_ZP:    nes_cpu_instr_cpy(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_CPY_ABS:   nes_cpu_instr_cpy(cpu); cpu->current_instr.result.cycles = 4; break;
        
        case NES_CPU_INSTR_DEC_ZP:    nes_cpu_instr_dec(cpu); cpu->current_instr.result.cycles = 5; break;
        case NES_CPU_INSTR_DEC_ZP_X:  nes_cpu_instr_dec(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_DEC_ABS:   nes_cpu_instr_dec(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_DEC_ABS_X: nes_cpu_instr_dec(cpu); cpu->current_instr.result.cycles = 7; break;

        case NES_CPU_INSTR_DEX_IMP:   nes_cpu_instr_dex(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_DEY_IMP:   nes_cpu_instr_dey(cpu); cpu->current_instr.result.cycles = 2; break;

        case NES_CPU_INSTR_EOR_IMM:   nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 2; break;   
        case NES_CPU_INSTR_EOR_ZP:    nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_EOR_ZP_X:  nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_EOR_ABS:   nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_EOR_ABS_X: nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_EOR_ABS_Y: nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_EOR_IND_X: nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_EOR_IND_Y: nes_cpu_instr_cmp(cpu); cpu->current_instr.result.cycles = 5; break;

        case NES_CPU_INSTR_INC_ZP:    nes_cpu_instr_inc(cpu); cpu->current_instr.result.cycles = 5; break;
        case NES_CPU_INSTR_INC_ZP_X:  nes_cpu_instr_inc(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_INC_ABS:   nes_cpu_instr_inc(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_INC_ABS_X: nes_cpu_instr_inc(cpu); cpu->current_instr.result.cycles = 7; break;

        case NES_CPU_INSTR_INX_IMP:   nes_cpu_instr_inx(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_INY_IMP:   nes_cpu_instr_iny(cpu); cpu->current_instr.result.cycles = 2; break;

        case NES_CPU_INSTR_JMP_ABS:   nes_cpu_instr_jmp(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_JMP_IND:   nes_cpu_instr_jmp(cpu); cpu->current_instr.result.cycles = 5; break;
        
        case NES_CPU_INSTR_JSR_ABS:   nes_cpu_instr_jsr(cpu); cpu->current_instr.result.cycles = 6; break;

        case NES_CPU_INSTR_LDA_IMM:   nes_cpu_instr_lda(cpu); cpu->current_instr.result.cycles = 2; break;   
        case NES_CPU_INSTR_LDA_ZP:    nes_cpu_instr_lda(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_LDA_ZP_X:  nes_cpu_instr_lda(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_LDA_ABS:   nes_cpu_instr_lda(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_LDA_ABS_X: nes_cpu_instr_lda(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_LDA_ABS_Y: nes_cpu_instr_lda(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_LDA_IND_X: nes_cpu_instr_lda(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_LDA_IND_Y: nes_cpu_instr_lda(cpu); cpu->current_instr.result.cycles = 5; break;

        case NES_CPU_INSTR_LDX_IMM:   nes_cpu_instr_ldx(cpu); cpu->current_instr.result.cycles = 2; break;   
        case NES_CPU_INSTR_LDX_ZP:    nes_cpu_instr_ldx(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_LDX_ZP_Y:  nes_cpu_instr_ldx(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_LDX_ABS:   nes_cpu_instr_ldx(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_LDX_ABS_Y: nes_cpu_instr_ldx(cpu); cpu->current_instr.result.cycles = 4; break;

        case NES_CPU_INSTR_LDY_IMM:   nes_cpu_instr_ldy(cpu); cpu->current_instr.result.cycles = 2; break;   
        case NES_CPU_INSTR_LDY_ZP:    nes_cpu_instr_ldy(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_LDY_ZP_X:  nes_cpu_instr_ldy(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_LDY_ABS:   nes_cpu_instr_ldy(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_LDY_ABS_X: nes_cpu_instr_ldy(cpu); cpu->current_instr.result.cycles = 4; break;

        case NES_CPU_INSTR_LSR_ACC:   nes_cpu_instr_lsr(cpu); cpu->current_instr.result.cycles = 2; break;   
        case NES_CPU_INSTR_LSR_ZP:    nes_cpu_instr_lsr(cpu); cpu->current_instr.result.cycles = 5; break;
        case NES_CPU_INSTR_LSR_ZP_X:  nes_cpu_instr_lsr(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_LSR_ABS:   nes_cpu_instr_lsr(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_LSR_ABS_X: nes_cpu_instr_lsr(cpu); cpu->current_instr.result.cycles = 7; break;

        case NES_CPU_INSTR_NOP_IMP:   nes_cpu_instr_nop(cpu); cpu->current_instr.result.cycles = 2; break;

        case NES_CPU_INSTR_ORA_IMM:   nes_cpu_instr_ora(cpu); cpu->current_instr.result.cycles = 2; break;   
        case NES_CPU_INSTR_ORA_ZP:    nes_cpu_instr_ora(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_ORA_ZP_X:  nes_cpu_instr_ora(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_ORA_ABS:   nes_cpu_instr_ora(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_ORA_ABS_X: nes_cpu_instr_ora(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_ORA_ABS_Y: nes_cpu_instr_ora(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_ORA_IND_X: nes_cpu_instr_ora(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_ORA_IND_Y: nes_cpu_instr_ora(cpu); cpu->current_instr.result.cycles = 5; break;

        case NES_CPU_INSTR_PHP_IMP:   nes_cpu_instr_php(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_PLA_IMP:   nes_cpu_instr_pla(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_PLP_IMP:   nes_cpu_instr_plp(cpu); cpu->current_instr.result.cycles = 4; break;
        
        case NES_CPU_INSTR_ROL_ACC:   nes_cpu_instr_rol(cpu); cpu->current_instr.result.cycles = 2; break;   
        case NES_CPU_INSTR_ROL_ZP:    nes_cpu_instr_rol(cpu); cpu->current_instr.result.cycles = 5; break;
        case NES_CPU_INSTR_ROL_ZP_X:  nes_cpu_instr_rol(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_ROL_ABS:   nes_cpu_instr_rol(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_ROL_ABS_X: nes_cpu_instr_rol(cpu); cpu->current_instr.result.cycles = 7; break;

        case NES_CPU_INSTR_ROR_ACC:   nes_cpu_instr_ror(cpu); cpu->current_instr.result.cycles = 2; break;   
        case NES_CPU_INSTR_ROR_ZP:    nes_cpu_instr_ror(cpu); cpu->current_instr.result.cycles = 5; break;
        case NES_CPU_INSTR_ROR_ZP_X:  nes_cpu_instr_ror(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_ROR_ABS:   nes_cpu_instr_ror(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_ROR_ABS_X: nes_cpu_instr_ror(cpu); cpu->current_instr.result.cycles = 7; break;

        case NES_CPU_INSTR_RTI_IMP:   nes_cpu_instr_rti(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_RTS_IMP:   nes_cpu_instr_rts(cpu); cpu->current_instr.result.cycles = 6; break;

        case NES_CPU_INSTR_SBC_IMM:   nes_cpu_instr_sbc(cpu); cpu->current_instr.result.cycles = 2; break;   
        case NES_CPU_INSTR_SBC_ZP:    nes_cpu_instr_sbc(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_SBC_ZP_X:  nes_cpu_instr_sbc(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_SBC_ABS:   nes_cpu_instr_sbc(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_SBC_ABS_X: nes_cpu_instr_sbc(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_SBC_ABS_Y: nes_cpu_instr_sbc(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_SBC_IND_X: nes_cpu_instr_sbc(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_SBC_IND_Y: nes_cpu_instr_sbc(cpu); cpu->current_instr.result.cycles = 5; break;

        case NES_CPU_INSTR_SEC_IMP:   nes_cpu_instr_sec(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_SED_IMP:   nes_cpu_instr_sed(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_SEI_IMP:   nes_cpu_instr_sei(cpu); cpu->current_instr.result.cycles = 2; break;

        case NES_CPU_INSTR_STA_ZP:    nes_cpu_instr_sta(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_STA_ZP_X:  nes_cpu_instr_sta(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_STA_ABS:   nes_cpu_instr_sta(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_STA_ABS_X: nes_cpu_instr_sta(cpu); cpu->current_instr.result.cycles = 5; break;
        case NES_CPU_INSTR_STA_ABS_Y: nes_cpu_instr_sta(cpu); cpu->current_instr.result.cycles = 5; break;
        case NES_CPU_INSTR_STA_IND_X: nes_cpu_instr_sta(cpu); cpu->current_instr.result.cycles = 6; break;
        case NES_CPU_INSTR_STA_IND_Y: nes_cpu_instr_sta(cpu); cpu->current_instr.result.cycles = 6; break;

        case NES_CPU_INSTR_STX_ZP:    nes_cpu_instr_stx(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_STX_ZP_Y:  nes_cpu_instr_stx(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_STX_ABS:   nes_cpu_instr_stx(cpu); cpu->current_instr.result.cycles = 4; break;

        case NES_CPU_INSTR_STY_ZP:    nes_cpu_instr_sty(cpu); cpu->current_instr.result.cycles = 3; break;
        case NES_CPU_INSTR_STY_ZP_X:  nes_cpu_instr_sty(cpu); cpu->current_instr.result.cycles = 4; break;
        case NES_CPU_INSTR_STY_ABS:   nes_cpu_instr_sty(cpu); cpu->current_instr.result.cycles = 4; break;

        case NES_CPU_INSTR_TAX_IMP:   nes_cpu_instr_tax(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_TAY_IMP:   nes_cpu_instr_tay(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_TSX_IMP:   nes_cpu_instr_tsx(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_TXA_IMP:   nes_cpu_instr_txa(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_TXS_IMP:   nes_cpu_instr_txs(cpu); cpu->current_instr.result.cycles = 2; break;
        case NES_CPU_INSTR_TYA_IMP:   nes_cpu_instr_tya(cpu); cpu->current_instr.result.cycles = 2; break;

        //default assume NOP
        default: nes_cpu_instr_nop(cpu); break;
    }

    //if we crossed a page boundary (aka an indexed operation toggled a bit in the MSB)
    //we add one cycle
    if (cpu->current_instr.result.page_boundary_crossed) {
        ++cpu->current_instr.result.cycles;
    }
}

internal void
nes_cpu_flag_check(NesCpu* cpu) {
    
    //check the flags
    if (cpu->current_instr.result.flag_c) {
        NesCpuFlagCheckC(cpu, cpu->current_instr.result.value);
    }
    if (cpu->current_instr.result.flag_z) {
        NesCpuFlagCheckZ(cpu, cpu->current_instr.result.value);
    }
    if (cpu->current_instr.result.flag_b && NesCpuFlagReadI(cpu) == 0) {
        nes_cpu_interrupt(cpu, NesCpuInterruptType::IRQ);
    }
    if (cpu->current_instr.result.flag_v) {
        NesCpuFlagCheckV(cpu, cpu->current_instr.result.value);
    }
    if (cpu->current_instr.result.flag_n) {
        NesCpuFlagCheckN(cpu, cpu->current_instr.result.value);
    }
}

internal void
nes_cpu_tick(NesCpu* cpu) {

    //set the previous instruction and clear current instruction
    cpu->previous_instr = cpu->current_instr;
    cpu->current_instr = {0};

    //get the instruction
    NesCpuMemoryReadAndIncrimentProgramCounter(cpu, cpu->current_instr.op_code);

    //decode the address mode
    nes_cpu_addr_mode_decode_from_instr(cpu);

    //decode the operand address from the address mode
    nes_cpu_decode_operand_address_from_address_mode(cpu);

    //execute the instruction
    nes_cpu_instr_execute(cpu);

    //check the flags affected by the result
    nes_cpu_flag_check(cpu);
}

internal NesCpu
nes_cpu_create_and_initialize() {

    NesCpu cpu = {0};
    cpu.registers.pc = NES_CPU_MEM_MAP_LOWER_PRG_ROM_ADDR;
    cpu.registers.sp = NES_CPU_MEM_MAP_STACK_ADDR + NES_CPU_MEM_MAP_STACK_SIZE;

    return cpu;
}