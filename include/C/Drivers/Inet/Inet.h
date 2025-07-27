#ifndef INET_H
#define INET_H

#include "../../Types/Common.h"

#include "../../Coms/IO/IO.h"

#include "Registers.h"


#define NUM_RX_DESC  256
#define NUM_TX_DESC  256

#define PACKET_SIZE  2048

#define RX_BUF_SIZE  NUM_RX_DESC * PACKET_SIZE
#define RX_DESC_SIZE NUM_RX_DESC * 16
#define RX_MEM_SIZE  RX_BUF_SIZE + RX_DESC_SIZE 

#define TX_BUF_SIZE  NUM_TX_DESC * PACKET_SIZE
#define TX_DESC_SIZE NUM_TX_DESC * 16
#define TX_MEM_SIZE  TX_BUF_SIZE + TX_DESC_SIZE

#define INET_MEM_SIZE TX_MEM_SIZE + RX_MEM_SIZE

struct e1000_rx_desc {
    qword addr;
    word length;
    word checksum;
    byte status;
    byte errors;
    word special;
} __attribute__((packed));

struct e1000_tx_desc { 
    qword addr;
    word length;
    byte cso;
    byte cmd;
    byte status;
    byte css;
    word special;
} __attribute__((packed));




// initializes the NIC to be able to use commands
byte INET_Init(IO INET_IO);

// locks the EEPROM which allows software to safely read the EEPROM
void Lock_EEPROM(void);

// unlocks it to allow hardware to read it safely while software isnt
void Unlock_EEPROM(void);

// reads EEPROM 
word Read_EEPROM(byte addr);

// returns the contents of a register
dword Read_Register(dword Reg);

// writes Data to a register
void Write_Register(dword Reg, dword Data);

#endif 