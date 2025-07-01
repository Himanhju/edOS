/*int syscall(int EAX, int EBX, int ECX, int EDX, int EDI, int ESI){
    asm(
        "movl %1, %%eax\n"
        "movl %2, %%ebx\n"
        "movl %3, %%ecx\n"
        "movl %4, %%edx\n"
        "movl %5, %%edi\n"
        "movl %6, %%esi\n"
        "int $0x80\n"
        "movl %%eax, %0"
        : "=m"(EAX)
        : "r"(EAX), "r"(EBX), "r"(ECX), "r"(EDX), "r"(EDI), "r"(ESI)
        : "eax", "ebx", "ecx", "edx", "edi", "esi"
    );
    return EAX;
}*/

#include "../include/unistd.h"


int syscall(int eax_in, int ebx_in, int ecx_in, int edx_in, int edi_in, int esi_in){
    int result;
    asm volatile (
        "int $0x80"
        : "=a" (result)
        : "a" (eax_in), "b" (ebx_in), "c" (ecx_in),
          "d" (edx_in), "D" (edi_in), "S" (esi_in)
        : "memory"
    );
    return result;
}