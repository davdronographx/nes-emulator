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

#endif //NES_TYPES_H