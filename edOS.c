#include "include/C/Interrupts/IDT/IDT.h"
#include "include/C/Input/Keyboard/Keyboard.h"
#include "include/C/General/Time/Time.h"
#include "include/C/Memory/Operations/Operations.h"
#include "include/C/General/shell/shell.h"
#include "include/C/vdFS/vdFS.h"
#include "include/C/Coms/PCI/PCI.h"
#include "include/C/General/Info/Info.h"
#include "include/C/Coms/IO/IO.h"

#include "include/C/edOS.h"

void _start(void){
    // Initialize the screen and VRAM
    VESA_Init();

    //set up IDT because if not it triple faults when interrupts are turned on
    IDT_Init();

    //initialize the PCI_Index array to say which pci slots and functions are enabled
    PCI_Init();

    //initialize the IO_RAM and IO ports for communication
    IO_Init();

    //initialize the FS, superblock, and directories
    if(Check_First_Boot() == 0){
        FS_Init();

        add_Dir_With_Path("/bin"); // the binary directory
        add_Dir_With_Path("/dev"); // devices and other things
        add_Dir_With_Path("/dev/fd"); // directory for opened files

        pstr_8x8("\nfs initialized\n", Get_RGB(0xffffff));
    }else{
        Read_Superblock();
    }

/*
    Dir_Metadata Meta;

    ata_read_sector(ROOT_START_BLOCK, &Meta);

    unsigned int bin_Sector = Hex_To_Dec_Short(Meta.Entries[0].Metadata_Block);

    ata_read_sector(bin_Sector, &Meta);

    pstr_8x8(Meta.Entries[0].Entname, Get_RGB(0xffffff));
*/

    

    //char HEX[9];
    //Dec_To_Hex((unsigned int)VRAM, HEX);
    //
    //pstr_8x8("vram addr: ", Get_RGB(0xffffff));
    //pstr_8x8(HEX, Get_RGB(0xffffff));

    //volatile RGB *test = ((volatile RGB*)Hex_To_Dec(HEX));

    //for(int i = 0; i < PIXEL_WIDTH * 4; i++){
    //    test[i] = Get_RGB(0xff00ff);
    //}

    newline();
    newline();
    pstr_8x8("reached the end\n", Get_RGB(0xffffff));

    while(1);
}

 