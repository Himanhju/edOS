#ifndef IO_H
#define IO_H

#include "../../Types/Common.h"

#include "../PCI/PCI.h"

typedef     void*    mmio;      // memory mapped IO port type (pointer to a region in memory) 
typedef     word     IO_port;   // IO Ports for the out and in opcodess

// The type of IO that is done to communicate with the device
enum IO_Type {
    MMIO,
    PORT
};

// The type of devices (internet, graphics, ect.)
enum IO_Dev {
    INET,
    DISC,
    PMC,
    ACPI,
    IDE,
    ISA,
    GRAPH
};

// A General IO struct for all devices
typedef struct IO{
    PCI_Device Device;
    //dword bar[MAX_BARS];
}IO;


extern IO *IO_RAM;

// returns the type of IO device
enum IO_Type check_IO_type(byte bus, byte dev, byte func, byte bar_num);

// initializes IO_RAM to store IO info
void IO_Init(void);

// Reads IO info from the IO sectors (DO NOT USE OUTSIDE OF RECOVERY)
void Read_IO(void);

// Writes IO info to the IO sectors  (DO NOT USE WHEN IO_RAM IS WRONG)
void Write_IO(void);

#endif