#ifndef NES_TYPES_H
#define NES_TYPES_H

#define ASSERT(x) if (!x) *((int*)(NULL)) = 1

//data types
typedef char i8;  
typedef short i16; 
typedef int i32;   
typedef long i64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long u64;

typedef u8 nes_val;
typedef u16 nes_addr;

typedef i32 b32;

#define TRUE 1
#define FALSE -1

#define global static
#define internal static
#define local static

#define SetBitInByte(val_bit_index, val_byte)   val_byte |= 1 << val_bit_index
#define ClearBitInByte(val_bit_index, val_byte) val_byte &= ~(1 << val_bit_index)
#define ReadBitInByte(val_bit_index, val_byte)  (val_byte >> val_bit_index) & 1

#define ADC_I     0x69
#define ADC_ZP    0x65
#define ADC_ZP_X  0x75
#define ADC_ABS   0x6D
#define ADC_ABS_X 0x7D
#define ADC_ABS_Y 0x79
#define ADC_IND_X 0x61
#define ADC_IND_Y 0x71        

#define AND_I     0x29
#define AND_ZP    0x25
#define AND_ZP_X  0x35
#define AND_ABS   0x2D
#define AND_ABS_X 0x3D
#define AND_ABS_Y 0x39
#define AND_IND_X 0x21
#define AND_IND_Y 0x31

#define ASL_ACC   0x0A
#define ASL_ZP    0x06
#define ASL_ZPX   0x16
#define ASL_ABS   0x0E
#define ASL_ABS_X 0x1E

#define BCC_REL 0x90
#define BCS_REL 0xB0
#define BEQ_REL 0xF0
#define BMI_REL 0x30
#define BNE_REL 0xD0
#define BPL_REL 0x10 


//TODO - bit
//#define BIT_ZP  0x24
//#define BIT_ABS 0x2C



#endif //NES_TYPES_H
