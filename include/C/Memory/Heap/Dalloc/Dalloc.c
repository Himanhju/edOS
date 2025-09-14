#include <Memory/Operations/Operations.h>

#include "Dalloc.h"

// Dalloc: Dynamic allocation
//  used for non-system purposes
//  only works in lifo order

static byte *Heap_Pos = (byte *)HEAP_BOTTOM;

const void *Dalloc_Get_Heap_Pos(void){
    return Heap_Pos;
}

void *dmalloc(dword bytes){
    if((dword)(Heap_Pos + (sizeof(dword) + bytes)) >= HEAP_TOP){
        return NULL;
    }
    void *ret = Heap_Pos + sizeof(dword);

    *((dword *)Heap_Pos) = bytes;

    Heap_Pos += sizeof(dword) + bytes;
    
    return ret;
}

void *dcalloc(dword Entries, dword Entry_Size){
    if(Entries == 0){
        return NULL;
    }

    void *returned = dmalloc(Entry_Size * Entries);

    if(returned != NULL){
        memset(returned, 0, Entries * Entry_Size);
    }

    return returned;
}

void dfree(void *ptr){
    if(ptr == NULL){
        return;
    }
    Heap_Pos -= ((*(((dword *)ptr) - 1)) + sizeof(dword));
}