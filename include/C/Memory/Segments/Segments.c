#include "Segments.h"

byte Read_Byte_From_Segment(void *Addr, word Segment){
    byte Value;
    asm(
        "movw %1, %%fs\n"                       // set fs
        "movb %%fs:(%2), %0"                    // read from fs at an offset 0faddress and put it in value
        : "=r"(Value)                           // return value
        : "r"(Segment), "r"(Addr)               // inputs
        : "memory"                              // if fs is used somewhere else (idk) it might affect memory
    );
    return Value;
}

void Write_Byte_To_Segment(void *Addr, word Segment, byte Value){
    asm(
        "movw %0, %%fs\n"                       // set fs
        "movb %2, %%fs:(%1)"                    // write value to fs at offset of address
        :                                       // no outputs
        : "r"(Segment), "r"(Addr), "r"(Value)   // inputs
        : "memory"                              // if fs is used somewhere else (idk) it might affect memory
    );
}

word Read_Word_From_Segment(void *Addr, word Segment){
    word Value;
    asm(
        "movw %1, %%fs\n"                       // set fs
        "movw %%fs:(%2), %0"                    // read from fs at an offset 0faddress and put it in value
        : "=r"(Value)                           // return value
        : "r"(Segment), "r"(Addr)               // inputs
        : "memory"                              // if fs is used somewhere else (idk) it might affect memory
    );
    return Value;
}

void Write_Word_To_Segment(void *Addr, word Segment, word Value){
    asm(
        "movw %0, %%fs\n"                       // set fs
        "movw %2, %%fs:(%1)"                    // write value to fs at offset of address
        :                                       // no outputs
        : "r"(Segment), "r"(Addr), "r"(Value)   // inputs
        : "memory"                              // if fs is used somewhere else (idk) it might affect memory
    );
}

dword Read_Dword_From_Segment(void *Addr, word Segment){
    dword Value;
    asm(
        "movw %1, %%fs\n"                       // set fs
        "movl %%fs:(%2), %0"                    // read from fs at an offset 0faddress and put it in value
        : "=r"(Value)                           // return value
        : "r"(Segment), "r"(Addr)               // inputs
        : "memory"                              // if fs is used somewhere else (idk) it might affect memory
    );
    return Value;
}

void Write_Dword_To_Segment(void *Addr, word Segment, dword Value){
    asm(
        "movw %0, %%fs\n"                       // set fs
        "movl %2, %%fs:(%1)"                    // write value to fs at offset of address
        :                                       // no outputs
        : "r"(Segment), "r"(Addr), "r"(Value)   // inputs
        : "memory"                              // if fs is used somewhere else (idk) it might affect memory
    );
}

void seg_memcpy(void* src, void* dest, word segment_in, word segment_out, unsigned int bytes){
    for(unsigned int i = 0; i < bytes; i++){
        Write_Dword_To_Segment(dest, segment_out, Read_Dword_From_Segment(src, segment_in));
    }
}

unsigned int seg_strlen(void* str, word segment){
    unsigned int i = 0;
    for(; Read_Byte_From_Segment(str + i, segment) != '\0'; i++);
    return i;
}

byte seg_memcmp(void* thing1, void* thing2, word segment_1, word segment_2, unsigned int bytes){
    for(unsigned int i = 0; i < bytes; i++){
        if(Read_Byte_From_Segment(thing1 + i, segment_1) != Read_Byte_From_Segment(thing2 + i, segment_2)){
            return 1;
        }
    }
    return 0;
}

void seg_memset(void *dest, word segment, byte value, unsigned int bytes){
    for(unsigned int i = 0; i < bytes; i++){
        Write_Byte_To_Segment(dest, segment, value);
    }
}

word Get_DS(void){
    word DS;
    asm volatile(
        "movw %%ds, %0"
        : "=r"(DS)
    );
    return DS;
}

word Get_ES(void){
    word ES;
    asm volatile(
        "movw %%es, %0"
        : "=r"(ES)
    );
    return ES;
}

word Get_FS(void){
    word FS;
    asm volatile(
        "movw %%fs, %0"
        : "=r"(FS)
    );
    return FS;
}

word Get_SS(void){
    word SS;
    asm volatile(
        "movw %%ss, %0"
        : "=r"(SS)
    );
    return SS;
}

word Get_GS(void){
    word GS;
    asm volatile(
        "movw %%gs, %0"
        : "=r"(GS)
    );
    return GS;
}

word Get_CS(void){
    word CS;
    asm volatile(
        "movw %%cs, %0"
        : "=r"(CS)
    );
    return CS;
}