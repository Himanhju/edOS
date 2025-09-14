#include "ATA/ATA.h"
#include "SATA/SATA.h"
#include "NVMe/NVMe.h"

#include "Disk.h"

static enum Disk_ID Disk = NO_DISK;
volatile struct Superblock superblock = {0};

enum Disk_ID Disk_Init(void){
    for(word bus = 0; bus < Highs.bus; bus++){
        for(byte slot = 0; slot < Highs.slot; slot++){
            for(byte func = 0; func < Highs.func; func++){
                switch ((int)Get_Device(PCI_Index[bus][slot][func][VENDOR], PCI_Index[bus][slot][func][DEVICE])){
                    case ((int)INTEL_SATA_ID):
                        // intel

                        // 82801IR/IO/IH (ICH9R/DO/DH) 6 port SATA Controller [AHCI mode]
                        if(Disk >= SATA){
                            break; // already initialized a better drive
                        }
                        
                        // initialize sata and check if it worked
                        if(SATA_Init((byte)bus & 0xFF, slot, func) == 0){
                            Disk = SATA;
                        }

                        break;

                    case ((int)REDHAT_NVME_ID):
                        // Red Hat, inc.
                    
                        // QEMU NVM Express Controller
                        if(Disk >= NVME){
                            break; // already initialized a better drive
                        }
                        
                        //initialize nvme and check if it worked
                        if(NVMe_Init((byte)bus & 0xFF, slot, func) == 0){
                            Disk = NVME;
                        }

                        break;

                    case ((int)INTEL_CHIPSET_ATA_ID):
                        Disk = ATA;
                        break;

                    default:

                        break;
                }
            }
        }
    }
    
    return Disk;
}

byte Read_Superblock(void){
    return Read_Sector(SUPERBLOCK_SECTOR, (void *)&superblock);
}

byte Write_Superblock(void){
    return Write_Sector(SUPERBLOCK_SECTOR, (void *)&superblock);
}

byte Superblock_Init(void){
    superblock.info.block_size = SECTOR_SIZE;
    superblock.info.total_sectors = 0xFFFFFFFF;//(((qword)Device_info.Max48BitLBA[1]) << 32) | Device_info.Max48BitLBA[0];
    superblock.info.is_first_boot = 1;

    Write_Superblock();

    return 0;
}

byte Read_Sector(dword lba, void *buffer){
    switch(Disk){
        case ATA:  return ata_read_sector(lba, buffer);
        case SATA: return sata_read_sector(lba, buffer);
        case NVME: return nvme_read_sector(lba, buffer);

        default:   return ata_read_sector(lba, buffer);
    }
}

byte Write_Sector(dword lba, void *buffer){
    switch(Disk){
        case ATA:  return ata_write_sector(lba, buffer);
        case SATA: return sata_write_sector(lba, buffer);

        default:   return ata_write_sector(lba, buffer);
    }
}

byte Bulk_Read_Sectors(dword Start_lba, void *buffer, word Sectors){
    switch(Disk){
        case ATA:  return ata_bulk_read_sectors(Start_lba, buffer, Sectors);
        case SATA: return sata_bulk_read_sectors(Start_lba, buffer, Sectors);

        default:   return ata_bulk_read_sectors(Start_lba, buffer, Sectors);
    }
}

byte Bulk_Write_Sectors(dword Start_lba, void *buffer, word Sectors){
    switch(Disk){
        case ATA:  return ata_bulk_write_sectors(Start_lba, buffer, Sectors);
        case SATA: return sata_bulk_write_sectors(Start_lba, buffer, Sectors);

        default:   return ata_bulk_read_sectors(Start_lba, buffer, Sectors);
    }
}

byte Identify_Device(void *buffer){
    switch(Disk){
//        case ATA:  return ata_bulk_write_sectors(Start_lba, buffer, Sectors);
        case SATA: return sata_identify_device(buffer);

        default:   return sata_identify_device(buffer);
    }
}