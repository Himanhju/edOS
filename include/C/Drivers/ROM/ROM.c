#include "ROM.h"

dword EDBA_BASE;
dword EDBA_SIZE;

void ROM_Init(void){
    EDBA_BASE = (dword)(BDA_Read_Word(BDA_EBDA_BASE) << 4);
    EDBA_SIZE = (dword)((640 - BDA_Read_Word(BDA_KB_BEF_EBDA)) * 1024);
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

//return *((volatile dword *)(INET_Regs_Mem + Reg));



//*((volatile dword *)(INET_Regs_Mem + Reg)) = Data;

