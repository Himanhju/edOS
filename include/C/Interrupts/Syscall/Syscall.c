#include "../../vdFS/vdFS.h"

#include "Syscall.h"

//syscall function called when int 0x80 is called
int syscall(int EAX, int EBX, int ECX, int EDX, int EDI, int ESI){
    switch(EAX){
        case 0: 
            EAX = read( // ECX is the pointer to the start of the buf then we copy at an offset to a buffer each time we write
                (char*)EBX, // ebx should hold the address of the buffer in user memory
                (char*)ECX, // ecx should hold the address of the buffer in user memory
                EDX         // edx should hold the number of byte to read
            );
            break;
        case 1: 
            break;
        case 2: 
            break;
        case 3: 
            break;
    }
    return EAX;
}

int sys_read(char* Path, void *buf, bytes){

}