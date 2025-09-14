#include <Graphics/VESA/VESA.h>
#include <Memory/Opcodes/Opcodes.h>

#include "ATA.h"

void ata_print_err(byte error){
    if((error & 0x1) != 0){
        pstr_8x8("Address Mark Not Found\n", Get_RGB(0xffffff));
    }
    if((error & 0x2) != 0){
        pstr_8x8("Track 0 Not Found\n", Get_RGB(0xffffff));
    }
    if((error & 0x4) != 0){
        pstr_8x8("Command Aborted\n", Get_RGB(0xffffff));
    }
    if((error & 0x8) != 0){
        pstr_8x8("Media Change Request\n", Get_RGB(0xffffff));
    }
    if((error & 0x10) != 0){
        pstr_8x8("ID Not Found\n", Get_RGB(0xffffff));
    }
    if((error & 0x20) != 0){
        pstr_8x8("Media Changed\n", Get_RGB(0xffffff));
    }
    if((error & 0x40) != 0){
        pstr_8x8("Uncorrectable data Error\n", Get_RGB(0xffffff));
    }
    if((error & 0x80) != 0){
        pstr_8x8("Bad Block Detected\n", Get_RGB(0xffffff));
    }else if(error == 0){
        pstr_8x8("No err\n", Get_RGB(0xffffff));
    }
}

byte ata_write_sector(dword lba, void *buffer) {
    // Wait for BSY to clear
    while(inb(0x1F7) & 0x80);

    //wait for it to be read for a command
    while(!(inb(0x1F7) & 0x40));

    // Send drive/head LBA info, and command to write
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);                      // Sector count
    outb(0x1F3, (byte)lba);              // LBA low
    outb(0x1F4, (byte)(lba >> 8));       // LBA mid
    outb(0x1F5, (byte)(lba >> 16));      // LBA high
    outb(0x1F7, WRITE_SECTORS);          // Command: WRITE SECTOR

    // Check for ERR
    byte status = inb(0x1F7);

    if (status & 0x01) {
        byte error = inb(0x1F1);
        // error now contains the error code bits
        ata_print_err(error);
        return error;
        
    }
    // Wait for the drive to be ready for data
    while (!(inb(0x1F7) & 0x08));

    const word *words = (word *)buffer;
    // Read 256 words (512 bytes)
    for (int i = 0; i < 256; i++) {
        outw(0x1F0, words[i]);
    }
    return 0;
}

byte ata_bulk_write_sectors(dword Start_lba, void *buffer, word Sectors) {
    byte *Buffer_bytes = (byte *)buffer;
    for(word i = 0; i < Sectors; i++){
        byte status = ata_write_sector(Start_lba + i, Buffer_bytes + (0x200 * i));
        if(status != 0){
            return status;
        }
    }
    return 0;
}

byte ata_read_sector(dword lba, void *buffer) {
    // Wait for BSY to clear
    while (inb(0x1F7) & 0x80);

    //wait for it to be read for a command
    while(!(inb(0x1F7) & 0x40));

    // Send drive/head, LBA info then tell it to execute the read 
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);                      // Sector count
    outb(0x1F3, (byte)lba);              // LBA low
    outb(0x1F4, (byte)(lba >> 8));       // LBA mid
    outb(0x1F5, (byte)(lba >> 16));      // LBA high
    outb(0x1F7, READ_SECTORS);           // Command: READ SECTOR
    
    // Check for ERR
    byte status = inb(0x1F7);
    
    if (status & 0x01) {
        byte error = inb(0x1F1);
        // error now contains the error code bits
        ata_print_err(error);
        return error;
        
    }

    // Wait for the drive to be ready for the data
    while (!(inb(0x1F7) & 0x08));

    word data;
    byte* buffer_bytes = (byte*)buffer;

    // Read 256 words (512 bytes)
    for (int i = 0; i < 256; i++) {
        data = inw(0x1F0);
        buffer_bytes[i * 2]     = (byte)(data & 0xFF);
        buffer_bytes[i * 2 + 1] = (byte)((data >> 8) & 0xFF);
    }
    return 0;
}

byte ata_bulk_read_sectors(dword Start_lba, void *buffer, word Sectors) {
    byte *Buffer_bytes = (byte *)buffer;
    for(word i = 0; i < Sectors; i++){
        byte status = ata_read_sector(Start_lba + i, Buffer_bytes + (0x200 * i));
        if(status != 0){
            return status;
        }
    }
    return 0;
}