#include "include/C/Interrupts/IDT/IDT.h"
#include "include/C/Input/Keyboard/Keyboard.h"
#include "include/C/General/Time/Time.h"
#include "include/C/Memory/Operations/Operations.h"
#include "include/C/General/shell/shell.h"
#include "include/C/vdFS/vdFS.h"
#include "include/C/General/Info/Info.h"
#include "include/C/Coms/PCI/PCI.h"

void _start(void) {
    // Initialize the screen and VRAM
    VESA_Init();

    //set up IDT because if not it triple faults when interrupts are turned on
    IDT_Init();

    //initialize the PCI_Index array to say which pci slots and functions are enabled
    PCI_Init();

    //initialize the FS and superblock
    if(!Check_First_Boot()){
        FS_Init();
    }

    pint_8x8(get_Dir_Sector_From_Path_Absolute("/"), Get_RGB(0xffffff));
    newline();
    psleep(999);
    pint_8x8(ROOT_START_BLOCK, Get_RGB(0xffffff));

    while(1);
}

 