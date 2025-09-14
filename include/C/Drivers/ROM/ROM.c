#include <Graphics/VESA/VESA.h>

#include "ROM.h"

dword EDBA_BASE;
dword EDBA_SIZE;

void ROM_Init(void){
    pstr_8x8("Initializing BIOS Data\n", Get_RGB(0xFFFFFF));
    EDBA_BASE = (dword)(BDA_Read_Word(BDA_EBDA_BASE) << 4);
    EDBA_SIZE = (dword)((640 - BDA_Read_Word(BDA_KB_BEF_EBDA)) * 1024);
    pstr_8x8("BIOS Data Initialized Successfully\n\n", Get_RGB(0x7CFC00));
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"

byte BDA_Read_Byte(byte addr){
    return *((volatile byte *)((dword)BDA_BASE + (dword)addr));
}

word BDA_Read_Word(byte addr){
    return *((volatile word *)((dword)BDA_BASE + (dword)addr));
}

dword BDA_Read_Dword(byte addr){
    return *((volatile dword *)((dword)BDA_BASE + (dword)addr));
}

byte EDBA_Read_Byte(byte addr){
    return *((volatile byte *)((dword)EDBA_BASE + (dword)addr));
}

word EDBA_Read_Word(byte addr){
    return *((volatile word *)((dword)EDBA_BASE + (dword)addr));
}

dword EDBA_Read_Dword(byte addr){
    return *((volatile dword *)(dword)((dword)EDBA_BASE + (dword)addr));
}

#pragma GCC diagnostic pop