#ifndef OPCODES_H
#define OPCODES_H

#include <Types/Common.h>

//pusha opcode which saves all of the registers (note the values are stored but the registers arent overwrited also no variables can be defined after this or it will break)
void Pusha(void);

//popa opcode which restores all of the registers (note if variables were defined between this and the last pusha opcode this will restore from garbage / unwanted data)
void Popa(void);

//closes interrupts and doesnt let them occur
void cli(void);

//starts interrupts and lets them occur again if they were turned off
void sti(void);

//halts the cpu and prevents execution of all further functions
void halt(void);

//outb opcode which outputs a byte via an I/O port on the cpu
void outb(word port, byte val);

//inb opcode which takes a byte from and I/O port on the cpu
byte inb(word port);

//inw opcode which takes a word from and I/O port on the cpu
word inw(word port);

//outw opcode which outputs a word via an I/O port on the cpu
void outw(word port, word data);

//inl opcode which takes a dword from and I/O port on the cpu
dword inl(word port);

//outl opcode which outputs a dword via an I/O port on the cpu
void outl(word port, dword data);

#endif