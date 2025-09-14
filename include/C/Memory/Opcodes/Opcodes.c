#include <Types/Common.h>

#include "Opcodes.h"

void Pusha(void){
    asm("pusha");
}

void Popa(void){
    asm("popa");
}

void outb(word port, byte val) {
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(val), "Nd"(port)
    );
}

void cli(void){
    asm("cli");
}

void sti(void){
    asm("sti");
}

byte inb(word port){
    byte data;
    asm volatile (
        "inb %1, %0"
        : "=a"(data)         // output: AL register -> data
        : "d"(port)          // input: port number in DX
    );
    return data;
}

void halt(void){
    asm("cli\n" "hlt");
}

word inw(word port){
    word data;
    asm volatile (
        "inw %1, %0"
        : "=a"(data)         // output: AL register -> data
        : "d"(port)          // input: port number in DX
    );
    return data;
}

void outw(word port, word data){
    asm volatile (
        "outw %0, %1"
        :
        : "a"(data), "Nd"(port)  // input: port number in DX
    );
}

dword inl(word port){
    dword data;
    asm volatile (
        "inl %1, %0"
        : "=a"(data)         // output: AL register -> data
        : "d"(port)          // input: port number in DX
    );
    return data;
}

void outl(word port, dword data){
    asm volatile (
        "outl %0, %1"
        :
        : "a"(data), "Nd"(port)  // input: port number in DX
    );
}