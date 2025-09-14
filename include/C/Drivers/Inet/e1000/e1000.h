#ifndef e1000_H
#define e1000_H

#include <Types/Common.h>

#include <Coms/IO/IO.h>

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

#define e1000_MEM_SIZE TX_MEM_SIZE + RX_MEM_SIZE


#define STATUS_DONE 0x01

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


struct Eth_Header{
    byte Dest_MAC[6];
    byte Source_MAC[6];
    word Protocol;
  //byte Data[length - 14]; // includes other things dependent on the protocol
};

extern byte MAC_Addr[6];

// initializes the NIC to be able to use commands
byte e1000_Init(byte bus, byte slot, byte func);

// locks the EEPROM which allows software to safely read the EEPROM
void Lock_EEPROM(void);

// unlocks it to allow hardware to read it safely while software isnt
void Unlock_EEPROM(void);

// reads EEPROM 
word Read_EEPROM(byte addr);

// returns the contents of a register
dword e1000_Read_Register(dword Reg);

// writes Data to a register
void e1000_Write_Register(dword Reg, dword Data);

// Enable bus mastering for the device
byte Enable_Bus_Mastering(byte bus, byte slot, byte func);

// begins sending or reciving packets
void Enable_e1000(void);

// stops sending or reciving packets
void Disable_e1000(void);

// sends a packet with data data and size size IF size is <= PACKET_SIZE
word e1000_SendPacket(void *data, word size);

// returns a the oldest received Packet and puts the data in buffer copies
struct e1000_rx_desc e1000_RscvPacket(void *buffer, word size);

#endif 