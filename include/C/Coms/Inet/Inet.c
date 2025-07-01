#include "../PCI/PCI.h"

#include "Inet.h"

dword inet_init(void){
    dword bar = read_bar(0, 3, 0, 0); // BAR0
    if (bar & 1) {
        // It's an I/O BAR
        word io_base = bar & 0xFFFC;
        return (dword)io_base;
    } else {
        // It's a memory BAR
        dword mem_base = bar & 0xFFFFFFF0;
        return (dword)mem_base;
    }
    
}