#ifndef IO_H
#define IO_H

#include "../../Types/Common.h"

#define MAX_BARS 6

typedef     void*    mmio;      // memory mapped IO port type (pointer to a region in memory) 
typedef     word     IO_port;   // IO Ports for the out and in opcodess

enum IO_Type {
    MMIO,
    PORT
};

enum IO_Dev {
    INET
};

typedef struct IO_General{
    dword bar;
    enum IO_Type Type;
}IO;

byte check_IO_type(byte bus, byte dev, byte func, byte bar_num);

void IO_Init(void);

//void write_IO(void);

#endif