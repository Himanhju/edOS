#include "e1000/e1000.h"
#include <Graphics/VESA/VESA.h>

#include "Inet.h"

static enum Inet_ID Inet = NO_NIC;

enum Inet_ID Inet_Init(void){
    for(word bus = 0; bus < Highs.bus; bus++){
        for(byte slot = 0; slot < Highs.slot; slot++){
            for(byte func = 0; func < Highs.func; func++){
                switch ((int)Get_Device(PCI_Index[bus][slot][func][VENDOR], PCI_Index[bus][slot][func][DEVICE])){
                    case ((int)INTEL_82544GC_ID_CP):
                    case ((int)INTEL_82544GC_ID_LM):
                    case ((int)INTEL_82541EI_ID1):
                    case ((int)INTEL_82541EI_ID2):
                    case ((int)INTEL_82541ER_ID1):
                    case ((int)INTEL_82541ER_ID2):
                    case ((int)INTEL_82541GI_ID1):
                    case ((int)INTEL_82541GI_ID2):
                    case ((int)INTEL_82541PI_ID):
                    case ((int)INTEL_82547GI_ID):
                    case ((int)INTEL_82540EM_ID):
                    //  82540EM Gigabit Ethernet Controller

                        // initialize device and check if it worked
                        if(e1000_Init((byte)bus & 0xFF, slot, func) == E1000){
                            Inet = E1000;
                        }
                        
                        break;

                    default:

                        break;

                }
            }
        }
    }
    return Inet;
}