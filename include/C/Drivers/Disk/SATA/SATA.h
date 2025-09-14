#ifndef SATA_H
#define SATA_H

#include <Types/Common.h>

#include <Coms/PCI/PCI.h>
#include <Coms/IO/IO.h>

#include "PCI_Info.h"
#include "Data_Types.h"

#include <Drivers/Disk/ATA/Commands.h> // based on ata so uses the same commands
#include <Drivers/Disk/ATA/ATA.h>

#define	SATA_SIG_ATA	0x00000101	// SATA drive
#define	SATA_SIG_ATAPI	0xEB140101	// SATAPI drive
#define	SATA_SIG_SEMB	0xC33C0101	// Enclosure management bridge
#define	SATA_SIG_PM	    0x96690101	// Port multiplier

#define HBA_PxIS_DHRS (1 << 0)  // Device to Host Register FIS received
#define HBA_PxIS_PSS  (1 << 1)  // PIO Setup FIS received
#define HBA_PxIS_DSS  (1 << 2)  // DMA Setup FIS received
#define HBA_PxIS_SDBS (1 << 3)  // Set Device Bits FIS received
#define HBA_PxIS_UFIS (1 << 4)  // Unknown FIS received
#define HBA_PxIS_PMIS (1 << 5)  // Port Multiplier FIS
#define HBA_PxIS_DIS  (1 << 6)  // Device Interface
#define HBA_PxIS_PRCS (1 << 7)  // Protocol Engine
#define HBA_PxIS_OFS  (1 << 8)  // Overflow
#define HBA_PxIS_INFS (1 << 9)  // Interface Non-fatal Error
#define HBA_PxIS_IFS  (1 << 10) // Interface Fatal Error
#define HBA_PxIS_HBDS (1 << 11) // Host Bus Data Error
#define HBA_PxIS_HBFS (1 << 12) // Host Bus Fatal Error
#define HBA_PxIS_TFES (1 << 30) // Task File Error (e.g. invalid command)
#define HBA_PxIS_CPDS (1 << 31) // Cold Port Detect

#define HBA_PxIS_ERRS (HBA_PxIS_TFES | HBA_PxIS_HBFS | HBA_PxIS_IFS)

#define AHCI_DEV_NULL 0
#define AHCI_DEV_SATA 1
#define AHCI_DEV_SEMB 2
#define AHCI_DEV_PM 3
#define AHCI_DEV_SATAPI 4

#define HBA_PORT_IPM_ACTIVE 1
#define HBA_PORT_DET_PRESENT 3

#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000

#define HBA_PORT_DET_INIT 0x01
#define HBA_PORT_DET_NOP  0x00

#define SATA_REGISTER_BAR 5
#define SATA_MAX_PORTS 32
#define SATA_CMD_LIST_LEN 32

#define SATA_ENTRY_SIZE 512

byte SATA_Init(byte bus, byte slot, byte func);

dword check_type(HBA_PORT port);

void Reset_Port(byte Port_num);

// Starts the command engine
void start_cmd(HBA_PORT *port);

// Stops the command engine
void stop_cmd(HBA_PORT *port);

byte AHCI_find_device(dword Device);

byte sata_read_sector(qword lba, void *buffer);

byte sata_write_sector(qword lba, void *buffer);

byte sata_bulk_read_sectors(qword Start_lba, void *buffer, word sectors);

byte sata_bulk_write_sectors(qword Start_lba, void *buffer, word sectors);

byte sata_identify_device(void *buffer);

#endif