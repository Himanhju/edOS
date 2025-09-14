#ifndef ACPI_H
#define ACPI_H

#include <Types/Common.h>

#include <Coms/IO/IO.h>
#include <Drivers/ROM/ROM.h>

// ACPI V1.0 RSDP
struct RSDP_t {
 char Signature[8]; // "RSD PTR "
 byte Checksum;     // Checksum of first 20 bytes (ACPI 1.0)
 char OEMID[6];
 byte Revision;     // 0 = ACPI 1.0 (this one), 2 = ACPI 2.0+
 dword RsdtAddress; // 32-bit physical address of RSDT
} __attribute__ ((packed));

#define RSDP_ID 0

// ACPI V2.0 XSDP
struct XSDP_t {
 char Signature[8]; // "RSD PTR "
 byte Checksum;     // Checksum of first 20 bytes (ACPI 1.0)
 char OEMID[6];
 byte Revision;     // 0 = ACPI 1.0 (this one), 2 = ACPI 2.0+
 dword RsdtAddress; // 32-bit physical address of RSDT
 
 dword Length;
 qword XsdtAddress; // 64-bit physical address of XSDT
 byte ExtendedChecksum; // Checksum of entire structure (Length bytes)
 byte reserved[3];
} __attribute__ ((packed));

#define XSDP_ID 2

struct ACPISDTHeader {
  char Signature[4];
  dword Length;
  byte Revision;
  byte Checksum;
  char OEMID[6];
  char OEMTableID[8];
  dword OEMRevision;
  dword CreatorID;
  dword CreatorRevision;
}__attribute__ ((packed));


byte ACPI_Init(/*PCI_Device ACPI_IO*/void);

void *Get_SDP_addr(void);

byte Check_SDP(struct XSDP_t *SDP);

byte Check_SDT(struct ACPISDTHeader *SDT);

void *findFACP_RSDT(void *RSDT);

void *findFACP_XSDT(void *XSDT);

byte Check_FCAP(struct ACPISDTHeader *FCAP);

#endif