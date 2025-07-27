#include "../PCI/PCI.h"
#include "../../Graphics/VESA.h"
#include "../../Memory/Operations/Operations.h"
#include "../../vdFS/vdFS.h"

#include "../../Drivers/Inet/Inet.h"
#include "../../Drivers/ACPI/ACPI.h"

#include "IO.h"

IO *IO_RAM = (IO *)0x00000000; // "allocate" ram to store the io ports (in kernel data segment)

enum IO_Type check_IO_type(byte bus, byte slot, byte func, byte bar_num){
    dword bar = read_bar(bus, slot, func, bar_num);
    if((bar & 0x01) == MMIO){
        //mmio 
        return MMIO;
    }else{
        //ports
        return PORT;
    }
}

void IO_Init(void){
    RGB RED = Get_RGB(0xff0000);
    PCI_Device Dev;

    for(word b = 0; b < BUSSES; b++){
        for(byte s = 0; s < SLOTS; s++){
            for(byte f = 0; f < FUNCS; f++){

                if(PCI_Index[b][s][f][VENDOR] == 0xFFFF){ // not valid device
                    continue;
                }

                switch(PCI_Index[b][s][f][VENDOR]){
                    case 0x8086:
//                      Intel

                        switch(PCI_Index[b][s][f][DEVICE]){
                            case 0x100E:
//                              82540EM Gigabit Ethernet Controller

//                              0, 3, 0


                                Dev.bus  = (byte)(b & 0x00FF);
                                Dev.slot = s;
                                Dev.func = f;  
                                IO_RAM[INET].Device = Dev;
                                
                                //(void)INET_Init(&IO_RAM[INET]);

                                (void)INET_Init(IO_RAM[INET]);


                                //for(byte i = 0; i < MAX_BARS; i++){
                                //    IO_RAM[INET].Type[i] = check_IO_type((byte)b, s, f, i);
                                //    if(IO_RAM[INET].Type[i] == MMIO){
                                //        IO_RAM[INET].bar[i] = read_bar((byte)b, s, f, i);
                                //        continue;
                                //    }
                                //    IO_RAM[INET].bar[i] = read_bar((byte)b, s, f, i);
                                //} 
                                
                                break;

                            case 0x1237:

//                              0, 0, 0

//                              440FX - 82441FX PMC [Natoma]
//                              PCI and Memory Controller (PMC)

                                Dev.bus  = (byte)(b & 0x00FF);
                                Dev.slot = s;
                                Dev.func = f;  
                                IO_RAM[PMC].Device = Dev;

                                //PMC_Init(&IO_RAM[PMC]);

                                break;

                            case 0x7000:

//                              0, 1, 0

//                              82371SB PIIX3 ISA [Natoma/Triton II]
//                              USB Host/Hub, IDE Controller, PCI-to-ISA Bridge

//                              isa = Instruction Set Architecture (what the cpu can do)

                                Dev.bus  = (byte)(b & 0x00FF);
                                Dev.slot = s;
                                Dev.func = f;  
                                IO_RAM[ISA].Device = Dev;

                                //ISA_Init(&IO_RAM[ISA]);
                            
                                break;

                            case 0x7010:

//                              0, 1, 1

//                              82371SB PIIX3 IDE [Natoma/Triton II]
//                              PCI-to-ISA Bridge, IDE Controller, USB Host/Hub Controller

//                              ide = Integrated Drive Electronics (disc)

                                Dev.bus  = (byte)(b & 0x00FF);
                                Dev.slot = s;
                                Dev.func = f;  
                                IO_RAM[IDE].Device = Dev;

                                //IDE_Init(&IO_RAM[IDE]);
                            
                                break;

                            case 0x7113:

//                              0, 1, 3

//                              82371AB/EB/MB PIIX4 ACPI
//                              PCI-to-ISA Bridge, Enhanced IDE Controller, USB Host/Hub, Enhanced Power Management, RTC(Real-Time Clock), SMBus, CMOS

//                              acpi = Advanced Configuation and Power Interface (self explanatory)

                                Dev.bus  = (byte)(b & 0x00FF);
                                Dev.slot = s;
                                Dev.func = f;  
                                IO_RAM[ACPI].Device = Dev;

                                //ACPI_Init(&IO_RAM[ACPI]);

                                ACPI_Init(IO_RAM[ACPI]);
                            
                                break;
                            

                            default:
                                // not a known device

                                goto DEVICE_UNKNOWN;
                        }

                        break;

                    case 0x1234:
                        // bochs/technical corp?
                        switch(PCI_Index[b][s][f][DEVICE]){
                            case 0x1111:

//                              0, 2, 0

//                              Bochs Graphics Adaptor
//                              QEMU Virtual Video Controller ["-vga std"]

                                Dev.bus  = (byte)(b & 0x00FF);
                                Dev.slot = s;
                                Dev.func = f;  
                                IO_RAM[GRAPH].Device = Dev;

                                //GRAPH_Init(&IO_RAM[GRAPH]);

                                break;
                            default:
                                // not a known device

                                goto DEVICE_UNKNOWN;
                        }

                        break;

                    default:
                        // not a known vendor
//VENDOR_UNKNOWN:
                        pstr_8x8("MIN ERR: UNABLE TO IDENTIFY PCI VENDOR: 0x", RED);
                        phex_8x8(PCI_Index[b][s][f][VENDOR], RED);
                        newline();
                        break;
DEVICE_UNKNOWN:
                        pstr_8x8("MIN ERR: UNABLE TO IDENTIFY PCI DEVICE: 0x", RED);
                        phex_8x8(PCI_Index[b][s][f][DEVICE], RED);
                        newline();
                        break;
                }
            }
        }
    }
    
    //Write_IO();
}

void Read_IO(void){
    ata_bulk_read_sectors(IO_STORAGE_BLOCK, (void *)IO_RAM, IO_STORAGE_BLOCKS); // read io storage into IO_Memory
}

void Write_IO(void){
    ata_bulk_write_sectors(IO_STORAGE_BLOCK, (void *)IO_RAM, IO_STORAGE_BLOCKS); // write io storage from Memory (beware use sparingly)
}