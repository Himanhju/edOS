#include "../PCI/PCI.h"
#include "../../vdFS/vdFS.h"

#include "IO.h"

volatile IO *IO_RAM = (volatile IO *)0x00000000; // "allocate" ram to store the io ports (in kernel data segment)

byte check_IO_type(byte bus, byte dev, byte func, byte bar_num){
    dword bar = read_bar(bus, dev, func, bar_num);
    if((bar & 0x01) == MMIO){
        //mmio
        return MMIO;
    }else{
        //ports
        return PORT;
    }
}

void IO_Init(void){
    for(word b = 0; b < BUSSES; b++){
        for(byte s = 0; s < SLOTS; s++){
            for(byte f = 0; f < FUNCS; f++){
                for(byte n = 0; n < MAX_BARS; n++){
                    
                }
            }
        }
    }
    ata_bulk_write_sectors(IO_STORAGE_BLOCK, (void *)IO_RAM, IO_STORAGE_BLOCKS);
}

void Read_IO(void){
    ata_bulk_read_sectors(IO_STORAGE_BLOCK, (void *)IO_RAM, IO_STORAGE_BLOCKS); // read
}