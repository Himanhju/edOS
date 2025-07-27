#include "include/C/Interrupts/IDT/IDT.h"
#include "include/C/Input/Keyboard/Keyboard.h"
#include "include/C/General/Time/Time.h"
#include "include/C/Memory/Operations/Operations.h"
#include "include/C/Memory/Segments/Segments.h"
#include "include/C/General/shell/shell.h"
#include "include/C/vdFS/vdFS.h"
#include "include/C/Coms/PCI/PCI.h"
#include "include/C/General/Info/Info.h"
#include "include/C/Coms/IO/IO.h"
#include "include/C/Memory/Paging/Paging.h"
#include "include/C/Memory/Palloc/Palloc.h"
#include "include/C/Drivers/ROM/ROM.h"

#include "include/C/edOS.h"

void _start(void){
    // load the rest of the kernel because BIOS is annoying
    ata_bulk_read_sectors(0x81, (void *)0x17C00, 0x7F);

    // Initialize the screen and VRAM
    VESA_Init();

    //set up IDT because if not it triple faults when interrupts are turned on
    IDT_Init();

    //Set up Paging for MMIO and memory safety
    Page_Init();

    //Set Global Variables for BIOS info
    ROM_Init();

    //initialize the PCI_Index array to say which pci slots and functions are enabled
    PCI_Init();

    //initialize the IO_RAM and IO ports for communication
    IO_Init();

    newline();

    


    newline();
    newline();
    pstr_8x8("reached the end\n", Get_RGB(0xffffff));

    while(1);
}
/*
    //initialize the FS, superblock, and directories
    if(Check_First_Boot() == 0){
        pstr_8x8("Initializing vdFS\n", Get_RGB(0xffffff));
        FS_Init();
    
        // OS directories 
        add_Dir_With_Path("/bin"); // the binary directory
        add_Dir_With_Path("/dev"); // devices and other things
        add_Dir_With_Path("/dev/fd"); // directory for opened files
    
        pstr_8x8("vdFS Initialized\n", Get_RGB(0xffffff));
        newline();
    }else{
        Read_Superblock();
    }
    
    */