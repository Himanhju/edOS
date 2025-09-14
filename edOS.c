#include <Graphics/VESA/VESA.h>
#include <Interrupts/IDT/IDT.h>
#include <Memory/Paging/Paging.h>
#include <Drivers/ROM/ROM.h>
#include <Coms/PCI/PCI.h>
#include <Coms/IO/IO.h>
#include <Drivers/Disk/Disk.h>
#include <Memory/Operations/Operations.h>
#include <Scheduler/Scheduler.h>
#include <General/Info/Info.h>
#include <FS/FAT32/FAT32.h>
#include <General/shell/shell.h>
#include <Input/Keyboard/Keyboard.h>
#include <General/Time/Time.h>

#include <edOS.h>

__attribute__((section(".text"), used, noinline, noreturn))
void _start(void){
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

    //initialize the FS, superblock, and directories
    if(Check_First_Boot() == 0){
        pstr_8x8("Initializing FS\n", Get_RGB(0xffffff));

        Format_FAT32();
    
        superblock.info.is_first_boot = 1;
        Superblock_Init();

        pstr_8x8("\nFS Initialized\n", Get_RGB(0xffffff));
    }else{
        Read_Superblock();
    }

    pstr_8x8("\nMounting FS\n", Get_RGB(0xffffff));

    switch(Mount_FAT32()){
    default:
        pstr_8x8("Failed To Mount FS: Unknown error\n\n", Get_RGB(0xff0033));
        break;

    case 1:
        pstr_8x8("Failed To Mount FS: Couldn't read boot sector\n\n", Get_RGB(0xff0033));
        break;

    case 2:
        pstr_8x8("Failed To Mount FS: Couldn't read FS Info sector\n\n", Get_RGB(0xff0033));
        break;

    case 3:
        pstr_8x8("Failed To Mount FS: Invalid FS ID\n\n", Get_RGB(0xff0033));
        break;

    case 0:
        pstr_8x8("FS Mounted\n\n", Get_RGB(0x7CFC00));
    }

    Scancode = 0;

    for(int i = 0; i < 400; i++){
        if((Scancode & ~0x80) == 1 && Key_Pressed == 1){ // if diagnostics is needed
            pstr_8x8("\n\nEntering Diagnostics Mode\n\n", Get_RGB(0xffffff));
            while((Scancode & ~0x80) != 28); // wait for user to continue
        }
        psleep(1);
    }

    for(byte i = 0; i < 32; i++){
        scroll(1);
        psleep(25);
    }
    
    clr_scrn();

    {
        char buffer[MAX_ARG_SIZE * 16];

        strcpy("echo Hello wecome to the EDOS tmp shell you can enter cont to continue booting and help to see the built in commands\n", buffer, 0);

        while(parse((char *)buffer) == 0){
            pstr_8x8(">>> ", Get_RGB(0xffffff));
            Get_Str(MAX_ARG_SIZE * 16, buffer, Get_RGB(0xFFFFFF));
        }
    }

    
    OS_CTRL = 0;  // tell the scheduler that initialization is done and it can schedule threads

    pstr_8x8("\n\nreached the end\n", Get_RGB(0xffffff));

OS_LOOP:

    while(OS_CTRL == 0); // keep kernel active but wait for something to tell it to run
    

    goto OS_LOOP; // restart the loop
}