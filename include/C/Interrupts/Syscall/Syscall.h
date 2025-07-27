#ifndef SYSCALL_H
#define SYSCALL_H

#define FNCTL_ADD_FILE 0
#define FNCTL_ADD_DIR  1

//syscall function called when int 0x80 is called
int syscall(int EAX, int EBX, int ECX, int EDX, int EDI, int  ESI);

// read syscall
byte sys_read(int fd, void *buf, unsigned int bytes, byte segment);

// write syscall
byte sys_write(int fd, void *buf, unsigned int bytes, byte segment);

// open syscall
byte sys_open(void *path, byte segment);

// close syscall
byte sys_close(int fd);

// file control syscall (ex: truncate, make new file/dir, etc.)
byte sys_fnctl(int Selector, void *PATH, int Flags, byte segment);

#endif