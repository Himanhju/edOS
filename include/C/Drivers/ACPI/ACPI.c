#include <Memory/Operations/Operations.h>
#include <Graphics/VESA/VESA.h>

#include "ACPI.h"

static struct XSDP_t *SDP;
static struct ACPISDTHeader *SDT;
static struct ACPISDTHeader *FCAP; // really called FADT but i liked FCAP after i mispelled the signiture so much i changed it

byte ACPI_Init(void){


    SDP = Get_SDP_addr();

    if(SDP == NULL){
        return 1;
    }

    if(Check_SDP(SDP) != 0){
        //pstr_8x8("could not initialize acpi: Invalid SDP Checksum\n", white);
        return 2;
    }

    if(SDP->Revision == RSDP_ID){
        SDT = (struct ACPISDTHeader *)SDP->RsdtAddress; // they use the same struct

        if(strncmp("RSDT", SDT->Signature, 4)){
            //pstr_8x8("could not initialize acpi: Wrong signature\n", white);
            return 4;
        }
    }else{
        /*
        SDT = (struct ACPISDTHeader *)SDP->XsdtAddress; // they use the same struct 

        if(strncmp("XSDT", SDT->Signature, 4)){
            pstr_8x8("could not initialize acpi: Wrong signature\n", white);
            return 4;
        }
        */ // gives error because a pointer is 32 bit while the address is 64 bit  
    }

    if(Check_SDT(SDT) != 0){
        //pstr_8x8("could not initialize acpi: Invalid SDT Checksum\n", white);
        return 3;
    }

    if(SDP->Revision == RSDP_ID)
    {
        //dword (*PointerToOtherSDT)[(SDT->Length - sizeof(struct ACPISDTHeader)) / 4];
        //PointerToOtherSDT = ((byte *)SDT + sizeof(struct ACPISDTHeader));
        FCAP = findFACP_RSDT(SDT);
    }
    else
    {
        //qword (*PointerToOtherSDT)[(SDT->Length - sizeof(struct ACPISDTHeader)) / 4];
        //PointerToOtherSDT = ((byte *)SDT + sizeof(struct ACPISDTHeader));
        FCAP = findFACP_XSDT(SDT);
    }

    if(Check_FCAP(FCAP) != 0){
        //pstr_8x8("could not initialize acpi: Invalid FCAP Checksum\n", white);
        return 5;
    }


    return 0;
}

void *Get_SDP_addr(void){

    for(dword i = EDBA_BASE; i < EDBA_BASE + EDBA_SIZE; i++){
        if(strncmp("RSD PTR ", (void *)i, 8) == 0){
            return (void *)i;
        }
    }

    for(dword i = BIOS_EXP_BASE; i < BIOS_BASE + BIOS_SIZE; i++){
        if(strncmp("RSD PTR ", (void *)i, 8) == 0){
            return (void *)i;
        }
    }
    
    return NULL;
}

byte Check_SDP(struct XSDP_t *sdp){
    byte *SDP_Byte = (byte *)sdp;
    dword checksum = 0;

    if(sdp->Revision == RSDP_ID){
        for(byte i = 0; i < sdp->Length; i++){
            checksum += SDP_Byte[i];
        }
    }else{
        for(byte i = 0; i < sdp->Length; i++){
            checksum += SDP_Byte[i];
        }
    }

    return checksum & 0xff;
}

byte Check_SDT(struct ACPISDTHeader *sdt){
    byte *SDT_Byte = (byte *)sdt;
    dword checksum = 0;

    for(word i = 0; i < sdt->Length; i++){
        checksum += SDT_Byte[i];
    }

    return (byte)(checksum & 0xFF);
}
/*
    RSDT *rsdt = (RSDT *) RootSDT;
    int entries = (rsdt->h.Length - sizeof(rsdt->h)) / 4;

    for (int i = 0; i < entries; i++)
    {
        ACPISDTHeader *h = (ACPISDTHeader *) rsdt->PointerToOtherSDT[i];
        if (!strncmp(h->Signature, "FACP", 4)){
            return (void *) h;
        }
    }

    // No FACP found
    return NULL;
*/

void *findFACP_RSDT(void *RSDT){

    struct ACPISDTHeader *rsdt = RSDT;
    dword**PointerToOtherSDT;
    PointerToOtherSDT = (void *)((byte *)rsdt + sizeof(struct ACPISDTHeader));

    dword entries = (rsdt->Length - sizeof(struct ACPISDTHeader)) / 4;

    for(dword i = 0; i < entries; i++){
        struct ACPISDTHeader *h = (struct ACPISDTHeader *) &((*PointerToOtherSDT)[i]);
        if (!strncmp(h->Signature, "FACP", 4)){
            return (void *) h;
        }
    }

    return NULL;
}

void *findFACP_XSDT(void *XSDT){
    struct ACPISDTHeader *xsdt = XSDT;
    qword (*PointerToOtherSDT) [(xsdt->Length - sizeof(struct ACPISDTHeader)) / 4];
    PointerToOtherSDT = (void *)((byte *)xsdt + sizeof(struct ACPISDTHeader));

    dword entries = (xsdt->Length - sizeof(struct ACPISDTHeader)) / 4;

    for(dword i = 0; i < entries; i++){
        struct ACPISDTHeader *h = (struct ACPISDTHeader *) &((*PointerToOtherSDT)[i]);
        if (!strncmp(h->Signature, "FACP", 4)){
            
            return (void *) h;
        }
    }
    return NULL;
}

byte Check_FCAP(struct ACPISDTHeader *fcap){
    byte *FCAP_Byte = (byte *)fcap;
    dword checksum = 0;

    for(word i = 0; i < fcap->Length; i++){
        checksum += FCAP_Byte[i];
    }

    return (byte)(checksum & 0xFF);
}