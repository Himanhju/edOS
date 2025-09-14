#ifndef ATA_H
#define ATA_H

#include <Types/Common.h>

#include "Commands.h"

#define ATA_DEV_BUSY  0x80  // BSY - Device is busy
#define ATA_DEV_DRDY  0x40  // DRDY - Device is ready
#define ATA_DEV_DF    0x20  // Device Fault
#define ATA_DEV_DSC   0x10  // Device Seek Complete
#define ATA_DEV_DRQ   0x08  // DRQ - Data request ready
#define ATA_DEV_CORR  0x04  // Corrected data
#define ATA_DEV_IDX   0x02  // Index
#define ATA_DEV_ERR   0x01  // Error occurred

//prints the errors in the error register of the ata drive
void ata_print_err(byte error);

//write buffer into the lba-th sector
byte ata_write_sector(dword lba, void* buffer);

//writes buffer to multiple sectors starting at Start_lba and writing Sectors sectors
byte ata_bulk_write_sectors(dword Start_lba, void* buffer, word Sectors);

//read the lba-th sector into buffer
byte ata_read_sector(dword lba, void* buffer);

//reads multiple sectors into buffer starting at Start_lba and reading Sectors sectors
byte ata_bulk_read_sectors(dword Start_lba, void* buffer, word Sectors);

#endif