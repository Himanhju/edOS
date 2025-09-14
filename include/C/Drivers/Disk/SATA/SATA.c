#include <Drivers/Disk/Disk.h>
#include <Memory/Heap/Palloc/Palloc.h>
#include <Memory/Operations/Operations.h>
#include <Graphics/VESA/VESA.h>
#include <General/Time/Time.h>

#include "SATA.h"

static volatile HBA_MEM *SATA_Regs_Mem = NULL;
static byte Num_Ports = 0;
static volatile HBA_CMD_HEADER (*CMD_Headers[SATA_MAX_PORTS])[SATA_CMD_LIST_LEN] = {NULL};
static volatile HBA_FIS *FIS_arr[SATA_MAX_PORTS] = {NULL};

byte SATA_Init(byte bus, byte slot, byte func){
    RGB RED = Get_RGB(0xff0000);

    Enable_Bus_Mastering(bus, slot, func);

    dword bar_buf = read_bar(bus, slot, func, SATA_REGISTER_BAR);

    if(bar_buf == 0){// if no space is reserved for the bar
        SATA_Regs_Mem = (volatile HBA_MEM *)pmalloc(0x2000, 0x1000);// allocate 8 kb for the registers (rounded up for pages)
        write_bar(bus, slot, func, SATA_REGISTER_BAR, (dword)SATA_Regs_Mem);// tell the device this is the bar now
    }else if((bar_buf & 1) == 0){ // if it a valid mmio address
        SATA_Regs_Mem = (volatile HBA_MEM *)(bar_buf & (~((dword)0xF))); // get the mmio
    }else{// if its a pio bar
        pstr_8x8("could not initialize sata drive: unexpected bar5\n", RED); // print an error
        return 1;
    }
    psleep(10); // wait for the device to finish setting the registers 

    SATA_Regs_Mem->ghc |= GHC_AHCI_ENABLE;

    for(byte i = 0; i < sizeof(SATA_Regs_Mem->pi) * 8; i++){
        if((SATA_Regs_Mem->pi & (1 << i)) != 0 && check_type(SATA_Regs_Mem->ports[i]) != AHCI_DEV_NULL){
            Reset_Port(i);
            Num_Ports++;
        }
    }

    if(Num_Ports == 0){
        pstr_8x8("could not initialize sata drive: No Ports\n", RED); // print an error
        return 2;
    }

    for(byte p = 0; p < sizeof(SATA_Regs_Mem->pi) * 8; p++){
        if((SATA_Regs_Mem->pi & (1 << p)) != 0 && check_type(SATA_Regs_Mem->ports[p]) != AHCI_DEV_NULL){
            // port implemented

            CMD_Headers[p] = pcalloc(SATA_CMD_LIST_LEN, sizeof(HBA_CMD_HEADER), sizeof(HBA_CMD_HEADER) * SATA_CMD_LIST_LEN); // allocate things for the ith ports commands
            SATA_Regs_Mem->ports[p].clb     = (void *)CMD_Headers[p];
            SATA_Regs_Mem->ports[p].clbu    = 0;

        }
    }

    for(byte p = 0; p < sizeof(SATA_Regs_Mem->pi) * 8; p++){
        if((SATA_Regs_Mem->pi & (1 << p)) != 0 && check_type(SATA_Regs_Mem->ports[p]) != AHCI_DEV_NULL){
            // port implemented

            for(byte i = 0; i < SATA_CMD_LIST_LEN; i++){
    //            (*CMD_Headers[p])[i].prdtl  = SATA_PRDT_LEN;
                (*CMD_Headers[p])[i].ctba   = pmalloc(sizeof(HBA_CMD_TBL), 0x80);
                (*CMD_Headers[p])[i].ctbau  = 0;
            }

        }
    }

    for(byte p = 0; p < sizeof(SATA_Regs_Mem->pi) * 8; p++){
        if((SATA_Regs_Mem->pi & (1 << p)) != 0 && check_type(SATA_Regs_Mem->ports[p]) != AHCI_DEV_NULL){
            // port implemented

            FIS_arr[p] = pmalloc(sizeof(HBA_FIS), sizeof(HBA_FIS)); // allocate things for the ith ports returned things
            SATA_Regs_Mem->ports[p].fb      = (void *)FIS_arr[p];
            SATA_Regs_Mem->ports[p].fbu     = 0;

        }
    } // 3 for loops to save palloc's memory

    return 0;
}

// Check device type
dword check_type(HBA_PORT port){
    dword ssts = port.ssts;

    byte ipm = (ssts >> 8) & 0x0F;
    byte det = ssts & 0x0F;

    if (det != HBA_PORT_DET_PRESENT)    // Check drive status
        return AHCI_DEV_NULL;
    if (ipm != HBA_PORT_IPM_ACTIVE)
        return AHCI_DEV_NULL;

    switch (port.sig)
    {
    case SATA_SIG_ATAPI:
        return AHCI_DEV_SATAPI;
    case SATA_SIG_SEMB:
        return AHCI_DEV_SEMB;
    case SATA_SIG_PM:
        return AHCI_DEV_PM;
    default:
        return AHCI_DEV_SATA;
    }
}

void Reset_Port(byte Port_num){
    stop_cmd(&(SATA_Regs_Mem->ports[Port_num]));

    // Set PxSCTL.DET to 1 (initiate COMRESET)
    SATA_Regs_Mem->ports[Port_num].sctl = (SATA_Regs_Mem->ports[Port_num].sctl & (~((dword)0xF))) | HBA_PORT_DET_INIT; // HBA_PORT_DET_INIT = 1
    psleep(1); // Short delay

    // Set PxSCTL.DET to 0 (return to normal)
    SATA_Regs_Mem->ports[Port_num].sctl = (SATA_Regs_Mem->ports[Port_num].sctl & (~((dword)0xF))) | HBA_PORT_DET_NOP; // HBA_PORT_DET_NOP = 0
    psleep(1); // Short delay again

    #define TIMEOUT_MS 2000
    dword ssts;
    for (int i = 0; i < TIMEOUT_MS; i++) {
        ssts = SATA_Regs_Mem->ports[Port_num].ssts;
        byte det = ssts & 0xF;
        byte ipm = (ssts >> 8) & 0xF;

        if (det == HBA_PORT_DET_PRESENT && ipm == HBA_PORT_IPM_ACTIVE)
            break;

        psleep(1); // wait 1ms for the time
    }


    start_cmd(&(SATA_Regs_Mem->ports[Port_num]));
}

// Start command engine
void start_cmd(HBA_PORT *port){
    // Wait until CR (bit15) is cleared
    while (port->cmd & HBA_PxCMD_CR);

    // Set FRE (bit4) and ST (bit0)
    port->cmd |= HBA_PxCMD_FRE;
    port->cmd |= HBA_PxCMD_ST; 
}

// Stop command engine
void stop_cmd(HBA_PORT *port){
    // Clear ST (bit0)
    port->cmd &= ~((dword)HBA_PxCMD_ST);

    // Clear FRE (bit4)
    port->cmd &= ~((dword)HBA_PxCMD_FRE);

    // Wait until FR (bit14), CR (bit15) are cleared
    while(1){
        if (port->cmd & HBA_PxCMD_FR)
            continue;
        if (port->cmd & HBA_PxCMD_CR)
            continue;
        break;
    }

}

byte AHCI_find_device(dword Device){

    for(byte i = 0; i < sizeof(SATA_Regs_Mem->pi) * 8; i++){
        if(check_type(SATA_Regs_Mem->ports[i]) == Device){
            return i;
        }
    }

    return 32;
}

byte sata_read_sector(qword lba, void *buffer){

    const byte port = AHCI_find_device(AHCI_DEV_SATA);
    
    if(port >= 32){
        return 2;
    }

    stop_cmd(&(SATA_Regs_Mem->ports[port]));

    HBA_PORT *hport = &SATA_Regs_Mem->ports[port];

    int slot = 0; // for simplicity, always use slot 0
    HBA_CMD_HEADER *cmdheader = &(*CMD_Headers[port])[slot];
    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL *)(cmdheader->ctba);

    // Zero command table
    memset((void *)cmdtbl, 0, sizeof(HBA_CMD_TBL));

    // Fill command header
    cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(dword); // Command FIS length
    cmdheader->w = 0; // Read
    cmdheader->prdtl = 1; // 1 PRDT entry

    // Set up PRDT
    cmdtbl->prdt_entry[0].dba = (void *)buffer;
    cmdtbl->prdt_entry[0].dbau = 0;
    cmdtbl->prdt_entry[0].dbc = 512 - 1;
    cmdtbl->prdt_entry[0].i = 1;

    // Set up command FIS
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D *)(&cmdtbl->cfis);
    memset((void *)cmdfis, 0, sizeof(FIS_REG_H2D));
    
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1; // command
    cmdfis->command = READ_DMA_EXT;
    cmdfis->device = 1 << 6; // LBA mode

    cmdfis->lba0 = (byte)(lba) & 0xFF;
    cmdfis->lba1 = (byte)(lba >> 8) & 0xFF;
    cmdfis->lba2 = (byte)(lba >> 16) & 0xFF;
    cmdfis->lba3 = (byte)(lba >> 24) & 0xFF;
    cmdfis->lba4 = (byte)(lba >> 32) & 0xFF;
    cmdfis->lba5 = (byte)(lba >> 40) & 0xFF;

    cmdfis->countl = 1; // 1 sector
    cmdfis->counth = 0;
    
    // Clear interrupts
    hport->is = (dword)-1;

    // Wait until not busy
    while ((hport->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)));
    
    start_cmd(&(SATA_Regs_Mem->ports[port]));

    //psleep(10);

    // Issue command
    hport->ci = 1 << slot;

    // Wait for completion
    while (hport->ci & (1 << slot)) {
        if (hport->is & HBA_PxIS_TFES)
            return 1; // error
    }

    return 0; // success
}

byte sata_write_sector(qword lba, void *buffer){

    byte port = AHCI_find_device(AHCI_DEV_SATA);
    
    if(port >= 32){
        return 2;
    }

    stop_cmd(&(SATA_Regs_Mem->ports[port]));

    HBA_PORT *hport = &SATA_Regs_Mem->ports[port];

    int slot = 0; // for simplicity, always use slot 0
    HBA_CMD_HEADER *cmdheader = &(*CMD_Headers[port])[slot];
    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL *)(cmdheader->ctba);

    // Zero command table
    memset((void *)cmdtbl, 0, sizeof(HBA_CMD_TBL));

    // Fill command header
    cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(dword); // Command FIS length
    cmdheader->w = 1; // Write
    cmdheader->prdtl = 1; // 1 PRDT entry

    // Set up PRDT
    cmdtbl->prdt_entry[0].dba = (void *)buffer;
    cmdtbl->prdt_entry[0].dbau = 0;
    cmdtbl->prdt_entry[0].dbc = 512 - 1;
    cmdtbl->prdt_entry[0].i = 1;

    // Set up command FIS
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D *)(&cmdtbl->cfis);
    memset((void *)cmdfis, 0, sizeof(FIS_REG_H2D));

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1; // command
    cmdfis->command = WRITE_DMA_EXT;
    cmdfis->device = 1 << 6; // LBA mode

    cmdfis->lba0 = (byte)(lba);
    cmdfis->lba1 = (byte)(lba >> 8);
    cmdfis->lba2 = (byte)(lba >> 16);
    cmdfis->lba3 = (byte)(lba >> 24);
    cmdfis->lba4 = (byte)(lba >> 32);
    cmdfis->lba5 = (byte)(lba >> 40);

    cmdfis->countl = 1; // 1 sector
    cmdfis->counth = 0;

    // Clear interrupts
    hport->is = (dword)-1;

    // Wait until not busy
    while ((hport->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)));

    start_cmd(&(SATA_Regs_Mem->ports[port]));

    //psleep(10);

    // Issue command
    hport->ci = 1 << slot;

    // Wait for completion
    while (hport->ci & (1 << slot)) {
        if (hport->is & HBA_PxIS_TFES)
            return 1; // error
    }

    return 0; // success
}

byte sata_bulk_read_sectors(qword Start_lba, void *buffer, word sectors){
    byte port = AHCI_find_device(AHCI_DEV_SATA);
    
    if(port >= 32){
        return 2;
    }

    stop_cmd(&(SATA_Regs_Mem->ports[port]));

    HBA_PORT *hport = &SATA_Regs_Mem->ports[port];

    int slot = 0; // for simplicity, always use slot 0
    HBA_CMD_HEADER *cmdheader = &(*CMD_Headers[port])[slot];
    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL *)(cmdheader->ctba);

    // Zero command table
    memset((void *)cmdtbl, 0, sizeof(HBA_CMD_TBL));

    // Fill command header
    cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(dword); // Command FIS length
    cmdheader->w = 0; // Write
    cmdheader->prdtl = sectors; // number of PRDT entries for the sectors

    // Set up PRDT
    for(dword i = 0; i < sectors; i++){
        cmdtbl->prdt_entry[i].dba = (void *)((dword)buffer + (dword)(0x200 * i));
        cmdtbl->prdt_entry[i].dbau = 0;
        cmdtbl->prdt_entry[i].dbc = 512 - 1;
        cmdtbl->prdt_entry[i].i = 1;
    }
    

    // Set up command FIS
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D *)(&cmdtbl->cfis);
    memset((void *)cmdfis, 0, sizeof(FIS_REG_H2D));

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1; // command
    cmdfis->command = READ_DMA_EXT;
    cmdfis->device = 1 << 6; // LBA mode

    cmdfis->lba0 = (byte)((Start_lba) & 0xff);
    cmdfis->lba1 = (byte)((Start_lba >> 8) & 0xff);
    cmdfis->lba2 = (byte)((Start_lba >> 16) & 0xff);
    cmdfis->lba3 = (byte)((Start_lba >> 24) & 0xff);
    cmdfis->lba4 = (byte)((Start_lba >> 32) & 0xff);
    cmdfis->lba5 = (byte)((Start_lba >> 40) & 0xff);

    cmdfis->countl = (byte)(sectors & 0xff); // 1 sector
    cmdfis->counth = (byte)((sectors >> 8) & 0xff);

    // Clear interrupts
    hport->is = (dword)-1;

    // Wait until not busy
    while ((hport->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)));

    start_cmd(&(SATA_Regs_Mem->ports[port]));

    //psleep(10);

    // Issue command
    hport->ci = 1 << slot;

    // Wait for completion
    while (hport->ci & (1 << slot)) {
        if (hport->is & HBA_PxIS_TFES)
            return 1; // error
    }

    return 0; // success
}

byte sata_bulk_write_sectors(qword Start_lba, void *buffer, word sectors){
    byte port = AHCI_find_device(AHCI_DEV_SATA);
    
    if(port >= 32){
        return 2;
    }

    stop_cmd(&(SATA_Regs_Mem->ports[port]));

    HBA_PORT *hport = &SATA_Regs_Mem->ports[port];

    int slot = 0; // for simplicity, always use slot 0
    HBA_CMD_HEADER *cmdheader = &(*CMD_Headers[port])[slot];
    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL *)(cmdheader->ctba);

    // Zero command table
    memset((void *)cmdtbl, 0, sizeof(HBA_CMD_TBL));

    // Fill command header
    cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(dword); // Command FIS length
    cmdheader->w = 1; // Write
    cmdheader->prdtl = sectors; // number of PRDT entries for the sectors

    // Set up PRDT
    for(dword i = 0; i < sectors; i++){
        cmdtbl->prdt_entry[i].dba = (void *)((dword)buffer + (dword)(0x200 * i));
        cmdtbl->prdt_entry[i].dbau = 0;
        cmdtbl->prdt_entry[i].dbc = 512 - 1;
        cmdtbl->prdt_entry[i].i = 1;
    }
    

    // Set up command FIS
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D *)(&cmdtbl->cfis);
    memset((void *)cmdfis, 0, sizeof(FIS_REG_H2D));

    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1; // command
    cmdfis->command = WRITE_DMA_EXT;
    cmdfis->device = 1 << 6; // LBA mode

    cmdfis->lba0 = (byte)((Start_lba) & 0xff);
    cmdfis->lba1 = (byte)((Start_lba >> 8) & 0xff);
    cmdfis->lba2 = (byte)((Start_lba >> 16) & 0xff);
    cmdfis->lba3 = (byte)((Start_lba >> 24) & 0xff);
    cmdfis->lba4 = (byte)((Start_lba >> 32) & 0xff);
    cmdfis->lba5 = (byte)((Start_lba >> 40) & 0xff);

    cmdfis->countl = (byte)(sectors & 0xff); // 1 sector
    cmdfis->counth = (byte)((sectors >> 8) & 0xff);

    // Clear interrupts
    hport->is = (dword)-1;

    // Wait until not busy
    while ((hport->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)));

    start_cmd(&(SATA_Regs_Mem->ports[port]));

    //psleep(10);

    // Issue command
    hport->ci = 1 << slot;

    // Wait for completion
    while (hport->ci & (1 << slot)) {
        if (hport->is & HBA_PxIS_TFES)
            return 1; // error
    }

    return 0; // success
}

byte sata_identify_device(void *buffer){

    const byte port = AHCI_find_device(AHCI_DEV_SATA);
    
    if(port >= 32){
        return 2;
    }

    stop_cmd(&(SATA_Regs_Mem->ports[port]));

    HBA_PORT *hport = &SATA_Regs_Mem->ports[port];

    int slot = 0; // for simplicity, always use slot 0
    HBA_CMD_HEADER *cmdheader = &(*CMD_Headers[port])[slot];
    HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL *)(cmdheader->ctba);

    // Zero command table
    memset((void *)cmdtbl, 0, sizeof(HBA_CMD_TBL));

    // Fill command header
    cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(dword); // Command FIS length
    cmdheader->w = 0; // Read
    cmdheader->prdtl = 1; // 1 PRDT entry

    // Set up PRDT
    cmdtbl->prdt_entry[0].dba = (void *)buffer;
    cmdtbl->prdt_entry[0].dbau = 0;
    cmdtbl->prdt_entry[0].dbc = 512 - 1;
    cmdtbl->prdt_entry[0].i = 1;

    // Set up command FIS
    FIS_REG_H2D *cmdfis = (FIS_REG_H2D *)(&cmdtbl->cfis);
    memset((void *)cmdfis, 0, sizeof(FIS_REG_H2D));
    
    cmdfis->fis_type = FIS_TYPE_REG_H2D;
    cmdfis->c = 1; // command
    cmdfis->command = IDENTIFY_DEVICE;
    cmdfis->device = 1 << 6; // LBA mode

    cmdfis->lba0 = (byte)0;
    cmdfis->lba1 = (byte)0;
    cmdfis->lba2 = (byte)0;
    cmdfis->lba3 = (byte)0;
    cmdfis->lba4 = (byte)0;
    cmdfis->lba5 = (byte)0;

    cmdfis->countl = 0; // 1 sector
    cmdfis->counth = 0;
    
    // Clear interrupts
    hport->is = (dword)-1;

    // Wait until not busy
    while ((hport->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)));
    
    start_cmd(&(SATA_Regs_Mem->ports[port]));

    //psleep(10);

    // Issue command
    hport->ci = 1 << slot;

    // Wait for completion
    while (hport->ci & (1 << slot)) {
        if (hport->is & HBA_PxIS_TFES)
            return 1; // error
    }

    return 0; // success
}