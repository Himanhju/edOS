#include <Coms/PCI/PCI.h>
#include <Graphics/VESA/VESA.h>
#include <Memory/Operations/Operations.h>
#include <FS/vdFS/vdFS.h>


#include <Drivers/Inet/Inet.h>
#include <Drivers/ACPI/ACPI.h>
#include <Drivers/Disk/Disk.h>

#include "IO.h"

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
    RGB RED = Get_RGB(0xff0033);
    RGB white = Get_RGB(0xffffff);

    pstr_8x8("Initializing IO Devices\n", white);

    // identify supported devices and vendors (so much for temporary)
    dword IO_Support = IO_Identify(); 
    if(((IO_Support >> 16) & 0xffff) != 0){
        pstr_8x8("\nNumber Of Vendors Unsupported: ", RED);
        pint_8x8((IO_Support >> 16) & 0xffff, RED);
        newline();
        newline();
    }
    if((IO_Support & 0xffff) != 0){
        pstr_8x8("\nNumber Of Devices Unsupported: ", RED);
        pint_8x8(IO_Support & 0xffff, RED);
        newline();
        newline();
    }

    // Dectect Disk types
    switch(Disk_Init()){
        case ATA:
            pstr_8x8("\nATA Drive Detected using ATA driver\n", white);
            break;

        case SATA:
            pstr_8x8("\nSATA Drive Detected using SATA driver\n", white);
            break;

        case NVME:
            pstr_8x8("\nNVME Drive Detected using NVME driver\n", white);
            break;

        default:
            pstr_8x8("\nMin ERR: DRIVE NOT SUPPORTED: falling back on ATA driver\n", RED);
            break;
    }

    // load the rest of the kernel because BIOS is annoying
    Bulk_Read_Sectors(0x81, (void *)0x17C00, 0x7F);

    switch(Inet_Init()){
        case E1000:
            pstr_8x8("E1000 NIC Detected Using E1000 driver\n", white);
            break;

        default:
            pstr_8x8("Min ERR: NIC NOT SUPPORTED: Skipping Internet driver\n", RED);
            break;
    }
    
    switch(ACPI_Init()){
        
        case 2:
            pstr_8x8("ERR: could not initialize acpi: Invalid SDP Checksum\n", white);
            break;

        case 3:
            pstr_8x8("ERR: could not initialize acpi: Invalid SDT Checksum\n", white);
            break;

        case 4:
            pstr_8x8("ERR: could not initialize acpi: Wrong signature\n", white);
            break;

        case 5:
            pstr_8x8("ERR: could not initialize acpi: Invalid FCAP Checksum\n", white);
            break;

        case 1:
            pstr_8x8("WRN: ACPI Not enabled: skipping ACPI Driver\n", white);
            break;

        default:
            pstr_8x8("ACPI Enabled successfully\n", Get_RGB(0x7CFC00));
    }    

    pstr_8x8("\nIO Devices initialized\n\n", Get_RGB(0x7CFC00));

}

dword IO_Identify(void){
    RGB RED = Get_RGB(0xff0033);

    //first word is the amount of vendors that were unsupported and second is the number of devices that are unsupported
    dword returned = 0; 

    // check for compatable devices
    for(word b = 0; b < Highs.bus; b++){
        for(byte s = 0; s < Highs.slot; s++){
            for(byte f = 0; f < Highs.func; f++){

                if(PCI_Index[b][s][f][VENDOR] == 0xFFFF){ // not valid device
                    continue;
                }

                //check compatable vendors
                switch(PCI_Index[b][s][f][VENDOR]){
                    case 0x8086:
//                      Intel

                        // check if the intel device is supported
                        switch(PCI_Index[b][s][f][DEVICE]){
                            case ((int)INTEL_82544GC_ID_CP & 0xffff):
                            case ((int)INTEL_82544GC_ID_LM & 0xffff):
                            case ((int)INTEL_82541EI_ID1 & 0xffff):
                            case ((int)INTEL_82541EI_ID2 & 0xffff):
                            case ((int)INTEL_82541ER_ID1 & 0xffff):
                            case ((int)INTEL_82541ER_ID2 & 0xffff):
                            case ((int)INTEL_82541GI_ID1 & 0xffff):
                            case ((int)INTEL_82541GI_ID2 & 0xffff):
                            case ((int)INTEL_82541PI_ID & 0xffff):
                            case ((int)INTEL_82547GI_ID & 0xffff):
                            case ((int)INTEL_82540EM_ID & 0xffff):
//                              82540EM Gigabit Ethernet Controller


                                //Dev.bus  = (byte)(b & 0x00FF);
                                //Dev.slot = s;
                                //Dev.func = f;
                                
                                //(void)INET_Init(&IO_RAM[INET]);

                                //(void)Inet_Init();

                                

                            case 0x1237:

//                              440FX - 82441FX PMC [Natoma]
//                              PCI and Memory Controller (PMC)

                                //Dev.bus  = (byte)(b & 0x00FF);
                                //Dev.slot = s;
                                //Dev.func = f;  

                                //PMC_Init(Dev);


                            case 0x2918:

                            //  82801IB (ICH9) LPC Interface Controller

                                

                            case 0x2922:

                            //  82801IR/IO/IH (ICH9R/DO/DH) 6 port SATA Controller [AHCI mode]

                                //for(byte i = 0; i < MAX_BARS; i++){
                                //    phex_8x8(read_bar((byte)(b & 0xff), s, f, i), RED);
                                //    newline();
                                //}
                                //newline();


                            case 0x2930:

                            //  82801I (ICH9 Family) SMBus Controller



                            case 0x293E:

                            //  82801I (ICH9 Family) HD Audio Controller

                                

                                break;

                            //  82G33/G31/P35/P31 Express DRAM Controller

                                

                            case 0x7000:

//                              82371SB PIIX3 ISA [Natoma/Triton II]
//                              USB Host/Hub, IDE Controller, PCI-to-ISA Bridge

//                              isa = Instruction Set Architecture (what the cpu can do)

                                //Dev.bus  = (byte)(b & 0x00FF);
                                //Dev.slot = s;
                                //Dev.func = f;  

                                //ISA_Init(Dev);
                            

                            case 0x7010:

//                              82371SB PIIX3 IDE [Natoma/Triton II]
//                              PCI-to-ISA Bridge, IDE Controller, USB Host/Hub Controller

//                              ide = Integrated Drive Electronics (disc)

                                //Dev.bus  = (byte)(b & 0x00FF);
                                //Dev.slot = s;
                                //Dev.func = f;

                                //IDE_Init(Dev); 


                            case 0x7113:

//                              82371AB/EB/MB PIIX4 ACPI
//                              PCI-to-ISA Bridge, Enhanced IDE Controller, USB Host/Hub, Enhanced Power Management, RTC(Real-Time Clock), SMBus, CMOS

//                              acpi = Advanced Configuation and Power Interface (self explanatory)

                                //Dev.bus  = (byte)(b & 0x00FF);
                                //Dev.slot = s;
                                //Dev.func = f;

                                //ACPI_Init(&IO_RAM[ACPI]);

                                //ACPI_Init(Dev);
                            
                                break;
                            

                            default:
                                // not a known device

                                goto DEVICE_UNKNOWN;
                        }

                        break;


                    case 0x1234:
                        // bochs/technical corp?

                        //check if the bochs device is supported
                        switch(PCI_Index[b][s][f][DEVICE]){
                            case 0x1111:

//                              Bochs Graphics Adaptor
//                              QEMU Virtual Video Controller ["-vga std"]

                                //Dev.bus  = (byte)(b & 0x00FF);
                                //Dev.slot = s;
                                //Dev.func = f;

                                //GRAPH_Init(Dev);

                                break;
                            default:
                                // not a known device

                                goto DEVICE_UNKNOWN;
                        }

                        break;


                    case 0x1B36:
                        // Red Hat, Inc.

                        // check if the Red Hat device is supported
                        switch(PCI_Index[b][s][f][DEVICE]){
                            case 0x10:

                            // QEMU NVM Express Controller

                                

                                break;

                            case 0xD:

                            // QEMU XHCI Host Controller

                                

                                break;

                            default:
                                // not a known device

                                goto DEVICE_UNKNOWN;
                        }

                        break;

                    case 0x1Af4:
                        // Red Hat, Inc.

                        // check if the Red Hat device is supported
                        switch(PCI_Index[b][s][f][DEVICE]){
                            case 0x1001:

                            // Virtio block device

                                

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

                        returned += 0x10000;

                        break;
DEVICE_UNKNOWN:
                        pstr_8x8("MIN ERR: UNABLE TO IDENTIFY PCI DEVICE: 0x", RED);
                        phex_8x8(PCI_Index[b][s][f][DEVICE], RED);
                        newline();

                        returned += 0x1;

                        break;
                }
            }
        }
    }

    return returned;
}