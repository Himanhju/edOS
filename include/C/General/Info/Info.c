#include "../../vdFS/vdFS.h"
#include "../../Memory/Operations/Operations.h"

#include "../../Graphics/VESA.h"

#include "Info.h"

byte Check_First_Boot(void){
    Superblock superblock;
    byte status = ata_read_sector(SUPERBLOCK_BLOCK, &superblock);
    if(status != 0){
        return 2;
    }
    return superblock.is_first_boot != 0 ? 1 : 0;
}
