#include <Graphics/VESA/VESA.h>
#include <Interrupts/IDT/IDT.h>
#include <Memory/Paging/Paging.h>
#include <Drivers/ROM/ROM.h>
#include <Coms/PCI/PCI.h>
#include <Coms/IO/IO.h>
#include <Memory/Operations/Operations.h>
#include <Scheduler/Scheduler.h>
#include <General/Info/Info.h>
#include <Drivers/Disk/Disk.h>
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

    Scancode = 0;

    if(Check_First_Boot() == 0){
        superblock.info.is_first_boot = 1;
        Superblock_Init();
    }else{
        Read_Superblock();
    }

    for(int i = 0; i < 400; i++){
        if((Scancode & ~0x80) == 1 && Key_Pressed == 1){ // if diagnostics is needed
            pstr_8x8("\n\nEntering Diagnostics Mode\n\n", WHITE);
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
            pstr_8x8(">>> ", WHITE);
            Get_Str(MAX_ARG_SIZE * 16, buffer, WHITE);
        }
    }
 
//========================TMP=========================

    #include <Memory/Opcodes/Opcodes.h>

    outw(0x604, 0x2000);

//====================================================
    
    OS_CTRL = 0;  // tell the scheduler that initialization is done and it can schedule threads

    pstr_8x8("\n\nreached the end\n", WHITE);

OS_LOOP:

    while(OS_CTRL == 0); // keep kernel active but wait for something to tell it to run
    

    goto OS_LOOP; // restart the loop
}