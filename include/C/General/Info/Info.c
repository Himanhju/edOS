#include <Drivers/Disk/Disk.h>
#include <Memory/Operations/Operations.h>

#include "Info.h"

byte Check_First_Boot(void){
    Superblock superblock;
    byte status = Read_Sector(SUPERBLOCK_SECTOR, &superblock);
    if(status != 0){
        return 2;
    }
    return superblock.info.is_first_boot;
}
