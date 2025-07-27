#ifndef PALLOC_H
#define PALLOC_H

#include "../../Types/Common.h"

#define HEAP_BOTTOM 0x04000000 // 64 mb
#define HEAP_TOP    0x10000000 //256 mb

#define ALIGN4(x) (((x) + 3) & ~3)

// returns the place where the next allocation will get added
const void *Get_Heap_Pos(void);

// returns an address to an allocated region of memory (not initialized) aligned to alignment
void *pmalloc(dword bytes, dword alignment);

// returns an address to an allocated region of memory (initialized to 0) aligned to alignment
void *pcalloc(dword Entries, dword Entry_Size, dword alignment);

// returns the address aligned to bytes
void *align(void *addr, dword bytes);

#endif