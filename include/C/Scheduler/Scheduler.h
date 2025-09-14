#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <Types/Common.h>

// amount of milliseconds each thread gets at a time
#define TIME_SLICE 10

typedef struct registers_t{
    // GPRs
    dword EAX, EBX, ECX, EDX, ESI, EDI, ESP, EBP;

    //flags
    dword EFLAGS;

    // Segment regs      
    word CS, SS, DS, ES, FS, GS;

    // floating point regs
    //long double ST[8];
    //long double XMM[8]; // simd
}registers_t;

typedef struct thread_t{
    registers_t Regs;

    dword EIP; // the thread's Intruction pointer
}thread_t;

extern volatile thread_t cur_thread;
extern byte OS_CTRL;

void Scheduler_tick(void);

#endif