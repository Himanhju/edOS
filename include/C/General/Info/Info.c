#include "../../vdFS/vdFS.h"
#include "../../Memory/Operations/Operations.h"

#include "Info.h"

byte Check_First_Boot(void){
    char contents[512];
    char Zeros[512];
    memset(Zeros, 0, 512);

    ata_read_sector(SUPERBLOCK_BLOCK, contents);

    return memcmp(contents, Zeros, 512);
}
