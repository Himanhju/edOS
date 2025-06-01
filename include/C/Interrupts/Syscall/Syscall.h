#ifndef SYSCALL_H
#define SYSCALL_H

//syscall function called when int 0x80 is called
int syscall(int EAX, int EBX, int ECX, int EDX, int EDI, int  ESI);

#endif