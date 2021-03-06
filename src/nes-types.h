#ifndef NES_TYPES_H
#define NES_TYPES_H

#define ASSERT(x) if (!(x)){ (*(int*)(NULL)) = 1; }

#define Fatal() ASSERT(1 == 0)

//data types
typedef char  i8;  
typedef short i16; 
typedef int   i32;   
typedef long  i64;

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned long  u64;

typedef u8  nes_val;
typedef u16 nes_addr;

typedef i32 b32;

#define TRUE  1
#define FALSE -1

#define global   static
#define internal static
#define local    static

#define SetBitInByte(val_bit_index, val_byte)   val_byte |= 1 << val_bit_index
#define ClearBitInByte(val_bit_index, val_byte) val_byte &= ~(1 << val_bit_index)
#define ReadBitInByte(val_bit_index, val_byte)  (val_byte >> val_bit_index) & 1

#define BUFFER_DEFAULT_SIZE 256

struct Buffer {
    u64 buffer_size;
    char* buffer_contents;
};

#endif //NES_TYPES_H
