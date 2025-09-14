#ifndef Disk_H
#define Disk_H

#include <Types/Common.h>

#include <Coms/IO/IO.h>
#include <Coms/PCI/PCI.h>

typedef enum Disk_ID {
    NO_DISK,
    ATA,
    SATA,
    NVME,
}Disk_ID;

#define INTEL_SATA_ID           0x80862922
#define REDHAT_NVME_ID          0x1B360010
#define INTEL_CHIPSET_ATA_ID    0x80867010

#define BOOT_SECTOR         0
#define FS_INFO_SECTOR      258
#define BK_BOOT_SECTOR      257
#define SUPERBLOCK_SECTOR   263
#define FIRST_FREE_SECTOR   264

#define SECTOR_SIZE         512

#include "Data_Types.h"

extern volatile struct Superblock superblock;

// detects and uses the first compatable drive returns 1 if no drive is found
enum Disk_ID Disk_Init(void); // highs are the highest numbers +1 that have devices in order to speed up the process

// reads in the superblock
byte Read_Superblock(void);

// writes info to the superblock
byte Write_Superblock(void);

// initializes the superblock with useful info
byte Superblock_Init(void);


// reads a sector and puts the contents in buffer (uses needed drivers)
byte Read_Sector(dword lba, void *buffer);

// writes the contents of buffer into a sector (uses needed drivers)
byte Write_Sector(dword lba, void *buffer);

// reads multiple contiguous sectors at once (uses needed drivers)
byte Bulk_Read_Sectors(dword Start_lba, void *buffer, word Sectors);

// writes multiple contiguous sectors at once (uses needed drivers)
byte Bulk_Write_Sectors(dword Start_lba, void *buffer, word Sectors);

// puts the device info into the buffer (size: 512)
byte Identify_Device(void *buffer);

#endif