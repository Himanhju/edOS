#include <Drivers/Disk/Disk.h>
#include <Graphics/VESA/VESA.h>
#include <Input/Keyboard/Keyboard.h>
#include <Memory/Operations/Operations.h>

#include "FS.h"


// prompt the user for which filesystem they want and use that as the filesystem (if the input is unexpected then use vdFS)
void FS_Init(void){
    
    char buf[32] = {0};

ask:

    pstr_8x8("\n\nvdFS (experimental): 1\nCFS (not done): 2\nSectors (no files/directories): 3\nPick a FS: ", WHITE);
    Get_Str(sizeof(buf), buf, WHITE);
    newline();

    if(strncmp("1", buf, strlen(buf))){

    }else if(strncmp("2", buf, strlen(buf))){

    }else if(strncmp("3", buf, strlen(buf))){

    }else{
        goto ask;
    }

    Superblock_Init();
}