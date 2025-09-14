#ifndef SEGMENTS_H
#define SEGMENTS_H

#include <Types/Common.h>

#define KERNEL_CODE_SEG 0x08
#define KERNEL_DATA_SEG 0x10
#define USER_CODE_SEG   0x18
#define USER_DATA_SEG   0x20

// returns the byte at Addr in segment Segment
byte Read_Byte_From_Segment(void *Addr, word Segment);

// write Value into Addr at segment Segment
void Write_Byte_To_Segment(void *Addr, word Segment, byte Value);

// returns the word at Addr in segment Segment
word Read_Word_From_Segment(void *Addr, word Segment);

// write Value into Addr at segment Segment
void Write_Word_To_Segment(void *Addr, word Segment, word Value);

// returns the dword at Addr in segment Segment
dword Read_Dword_From_Segment(void *Addr, word Segment);

// write Value into Addr at segment Segment
void Write_Dword_To_Segment(void *Addr, word Segment, dword Value);

// like normal memcpy but across different segments
void seg_memcpy(void* src, void* dest, word segment_in, word segment_out, unsigned int bytes);

// like normal strlen but across different segments
unsigned int seg_strlen(void* str, word segment);

// like normal memcmp but across different segments
byte seg_memcmp(void* thing1, void* thing2, word segment_1, word segment_2, unsigned int bytes);

// like normal memset but across different segments
void seg_memset(void *dest, word segment, byte value, unsigned int bytes);

// returns the value of DS (Data Segment)
word Get_DS(void);

// returns the value of FS
word Get_FS(void);

// returns the value of ES (Extra Segment)
word Get_ES(void);

// returns the value of SS (Stack Segment)
word Get_SS(void);

// returns the value of GS
word Get_GS(void);

// returns the value of CS (Code Segment)
word Get_CS(void);

#endif