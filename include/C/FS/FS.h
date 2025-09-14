#ifndef FS_H
#define FS_H

typedef enum FS_TYPE{
    VDFS    = 0xDEADBEEF % 0x7FFFFFFF,
    CFS     = 0xA20FAC52 % 0x7FFFFFFF,
    SECTORS = 0xFFFFFFFF % 0x7FFFFFFF
}FS_TYPE;

void FS_Init(void);

#endif