#include <Memory/Segments/Segments.h>
#include <Memory/Operations/Operations.h>
#include <Graphics/VESA/VESA.h>

#include "Syscall.h"

//syscall function called when int 0x80 is called
int syscall(int EAX, int EBX, int ECX, int EDX, int EDI, int ESI){
    switch(EAX){
        case 0: 
            // ECX is the pointer to the start of the buf then we copy at an offset to a buffer each time we write
            // ebx should hold the address of the buffer in user memory
            // ecx should hold the address of the buffer in user memory
            // edx should hold the number of byte to read
            EAX = sys_read(EBX, (void *)ECX, (unsigned int)EDX); // fix eax and add segment reading
            break;
        case 1: 
            EAX = sys_write(EBX, (void *)ECX, (unsigned int)EDX);
            break;
        case 2: 
            EAX = sys_open((void *)EBX);
            break;
        case 3: 
            EAX = sys_close(EBX);
            break;
        case 4:
            EAX = sys_fnctl(EBX, (void *)ECX, EDX);
            break;
        case 5:
            EBX &= 0xFF; //only get ascii chars
            pchar_8x8((char)EBX, Get_RGB(ECX & 0x00FFFFFF)); // print ascii with the color in ECX
            return EBX; // return the character printed
        default:
            EAX = -1;
            break;
    }

    return EAX;
}

//read from a file
byte sys_read(int fd, void *buf, unsigned int bytes){
    unsigned int sectors = bytes / 0x200; // auto-floors
    return 0;
}

//write to a file
byte sys_write(int fd, void *buf, unsigned int bytes){
    return 0;
}

// open a file
byte sys_open(void *path){
    return 0;
}

// closes a file
byte sys_close(int fd){
    return 0;
}