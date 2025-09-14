#include "Palloc.h"

#include <Memory/Operations/Operations.h>

// Palloc: Permanent allocation
//  used for system things that are never freed
//  use Dalloc for non-system things that should be freed

static byte *Heap_Pos = (byte *)HEAP_BOTTOM;

// returns the address aligned to bytes
static void *align(void *addr, dword bytes){
    return (void *)(((dword)((byte *)addr) + (bytes - 1)) & (dword)(~(bytes - 1)));
}
/*
better writen version:

void *align(void *addr, dword bytes){
    dword p = (dword)addr;
    dword aligned = (p + (bytes - 1)) & ~(bytes - 1);
    return (void *)aligned;
}
 */

const void *Palloc_Get_Heap_Pos(void){
    return Heap_Pos;
}

void *pmalloc(dword bytes, dword alignment){

    Heap_Pos = (byte *)align(Heap_Pos, alignment);

    void *returned = Heap_Pos;

    Heap_Pos += bytes;
    if(Heap_Pos >= (byte *)HEAP_TOP){
        Heap_Pos -= bytes;

        return NULL;
    }

    return returned;
}

void *pcalloc(dword Entries, dword Entry_Size, dword alignment){
    if(Entries == 0){
        return NULL;
    }

    void *returned = pmalloc(Entry_Size * Entries, alignment);

    if(returned != NULL){
        memset(returned, 0, Entries * Entry_Size);
    }

    return returned;
}

