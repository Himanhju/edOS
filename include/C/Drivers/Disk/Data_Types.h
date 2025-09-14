#ifndef DISK_DATA_TYPES
#define DISK_DATA_TYPES

#include <Types/Common.h>
#include "Disk.h"

typedef struct Superblock {
    struct{
        dword magic;          // the magic number of the FS
        qword total_sectors;  // Disk size
        dword block_size;
        qword root_dir_start; // Sector number of root dir
        byte is_first_boot;  // whether its the first boot or not
    }info;
    char padding[SECTOR_SIZE - (sizeof(byte) + (sizeof(dword) * 2 + (sizeof(qword) * 2)))];
}Superblock;


#endif