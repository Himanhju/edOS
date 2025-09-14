#ifndef SYSCALL_H
#define SYSCALL_H


//syscall function called when int 0x80 is called
int syscall(int EAX, int EBX, int ECX, int EDX, int EDI, int  ESI);

// read syscall
byte sys_read(int fd, void *buf, unsigned int bytes);

// write syscall
byte sys_write(int fd, void *buf, unsigned int bytes);

// open syscall
byte sys_open(void *path);

// close syscall
byte sys_close(int fd);

// file control syscall (ex: truncate, make new file/dir, etc.)
byte sys_fnctl(int Selector, void *PATH, int Flags);

#endif