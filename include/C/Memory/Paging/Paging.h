#ifndef PAGING_H
#define PAGING_H

#include <Types/Common.h>

#define PAGE_PRESENT        0x1
#define PAGE_RW             0x2
#define PAGE_USER           0x4

#define STACK_TOP           0x0007C000
#define MEM_TOP             0xFFFFFFFF


#define PAGE_DIR_SIZE       0x00001000
#define PAGE_TABLES_SIZE    0x00400000


#define PAGE_MEM_BASE       0x00100000
#define PAGE_DIR_BASE       0x00100000
#define PAGE_TABLES_BASE    0x00102000

#define PAGE_MEM_TOP        0x00502000


void Page_Init(void);

void Remap_Page(dword virtual_addr, dword physical_addr, dword flags);

#endif