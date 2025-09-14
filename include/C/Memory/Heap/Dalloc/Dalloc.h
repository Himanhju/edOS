#ifndef DALLOC_H
#define DALLOC_H

#include <Types/Common.h>

#define HEAP_BOTTOM 0x10000000 // 256 mb
#define HEAP_TOP    0x20000000 // 512 mb

// returns the place where the next allocation will get added
const void *Dalloc_Get_Heap_Pos(void);

// returns an address to an allocated region of memory (not initialized) aligned to alignment
void *dmalloc(dword bytes);

// returns an address to an allocated region of memory (initialized to 0) aligned to alignment
void *dcalloc(dword Entries, dword Entry_Size);

// frees space from the heap by removing an allocated thing
void dfree(void *ptr);

#endif