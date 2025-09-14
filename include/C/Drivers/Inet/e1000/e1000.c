#include <Coms/PCI/PCI.h>
#include <Graphics/VESA/VESA.h>
#include <Memory/Opcodes/Opcodes.h>
#include <Memory/Heap/Palloc/Palloc.h>
#include <Memory/Operations/Operations.h>
#include <Drivers/Inet/Inet.h>

#include "e1000.h"

static volatile word  e1000_Base_Port = 0xFFFF;
static volatile byte *e1000_Regs_Mem;

static word NIC_version = 0xffff;

byte MAC_Addr[6] = {0};


// RX ring
//struct e1000_rx_desc rx_ring[NUM_RX_DESC];
struct e1000_rx_desc* rx_ring;
byte *(rx_buffers[NUM_RX_DESC]);//[PACKET_SIZE];
                                            // TODO: FINISH PALLOC AND ALLOCATE THESE WITH THAT
// TX ring
//struct e1000_tx_desc tx_ring[NUM_TX_DESC];
struct e1000_tx_desc* tx_ring;
byte *(tx_buffers[NUM_TX_DESC]);//[PACKET_SIZE];


byte e1000_Init(byte bus, byte slot, byte func){
    RGB white = Get_RGB(0xffffff);

    NIC_version = PCI_Index[bus][slot][func][DEVICE]; // keep which nic it is in memory
    
    e1000_Regs_Mem = (byte *)read_bar(bus, slot, func, 0);
    if((((dword)e1000_Regs_Mem) & 0x00000006) >> 1 == 2){
        pstr_8x8("64-Bit address returned?????\n", white); // SHOULDN'T BE RUN, WE HAVE PROBLEMS IF IT IS
    }else{
        e1000_Regs_Mem = (void *)(((dword)e1000_Regs_Mem) & 0xFFFFFFF0);
    }

    
    for(byte i = 1; i < MAX_BARS; i++){
        dword cur_bar = read_bar(bus, slot, func, i);
        if((cur_bar & 1) == 1){
            e1000_Base_Port = (word)(cur_bar & 0x0000FFFF);
            break;
        }
    }

    if(e1000_Base_Port == 0xFFFF){
        pstr_8x8("Could not initialize the NIC: no window\n", white);
        return 2;
    }
    
    dword Reg_Buf;
    

    if(Enable_Bus_Mastering(bus, slot, func) != 0){
        pstr_8x8("Could not initialize the NIC: bus mastering failed\n", white);
        return 1;
    }


    // reset the device

    e1000_Write_Register(CTRL_REG, e1000_Read_Register(CTRL_REG) | CTRL_RST);


    while((e1000_Read_Register(CTRL_REG) & CTRL_RST) != 0); //wait for it to be reset

    
    // allow software to read the EEPROM of the device
    //outl(REG_WIN, EECD_REG);
    Reg_Buf = e1000_Read_Register(EECD_REG);
    e1000_Write_Register(EECD_REG, Reg_Buf | EECD_SK | EECD_CS | EECD_DI);

    // read MAC address
    Lock_EEPROM();
    *((word *)&MAC_Addr[0]) = Read_EEPROM(0x00);
    *((word *)&MAC_Addr[2]) = Read_EEPROM(0x01);
    *((word *)&MAC_Addr[4]) = Read_EEPROM(0x02);
    Unlock_EEPROM();


    // allocate the tx and rx ring
    
    // rx
    rx_ring = (struct e1000_rx_desc*)pcalloc(NUM_RX_DESC, sizeof(struct e1000_rx_desc), 16);
    //byte rx_buffers[NUM_RX_DESC][PACKET_SIZE];
    for(word i = 0; i < NUM_RX_DESC; i++){
        rx_ring[i].addr = (((qword)(dword)pmalloc(PACKET_SIZE, 2048)) & 0xffffffff);
    }
    // tx
    tx_ring = (struct e1000_tx_desc*)pcalloc(NUM_TX_DESC, sizeof(struct e1000_tx_desc), 16);
    //byte tx_buffers[NUM_TX_DESC][PACKET_SIZE];
    for(word i = 0; i < NUM_TX_DESC; i++){
        tx_ring[i].addr = (((qword)(dword)pmalloc(PACKET_SIZE, 2048)) & 0xffffffff);
    }

    for(word i = 0; i < NUM_RX_DESC; i++){
        rx_ring[i].status |= 1;
    }
    for(word i = 0; i < NUM_TX_DESC; i++){
        tx_ring[i].status |= 1;
    }


    // tell the nic where the rx descriptor table is 
    e1000_Write_Register(RDBAL_REG, ((dword)rx_ring & 0xFFFFFFFF));
    e1000_Write_Register(RDBAH_REG, 0x00000000); // not in 64 bit mode so no need to have this be non-zero
    e1000_Write_Register(RDLEN_REG, (dword)(16 * NUM_RX_DESC));
    e1000_Write_Register(RDH_REG, 0x00000000);
    e1000_Write_Register(RDT_REG, (dword)NUM_RX_DESC - 1);

    // tell the nic where the tx descriptor table is 
    e1000_Write_Register(TDBAL_REG, (dword)tx_ring);
    e1000_Write_Register(TDBAH_REG, 0x00000000); // not in 64 bit mode so no need to have this be non-zero
    e1000_Write_Register(TDLEN_REG, (dword)(16 * NUM_TX_DESC));
    e1000_Write_Register(TDH_REG, 0x00000000);
    e1000_Write_Register(TDT_REG, 0x00000000); // dont have anything to send currently

    /*

    | Register  | Description                     | What to write                                              |
    | --------- | ------------------------------- | ---------------------------------------------------------- |
    |   RDBAL   | RX Descriptor Base Address Low  | Lower 32 bits of RX ring physical address                  |
    |   RDBAH   | RX Descriptor Base Address High | Upper 32 bits of RX ring physical address (zero if 32-bit) |
    |   RDLEN   | RX Descriptor Ring Length       | Size in bytes of RX ring (e.g., 128 * 16 = 2048)           |
    |   RDH     | RX Descriptor Head              | Initially 0 (NIC sets this)                                |
    |   RDT     | RX Descriptor Tail              | Initially 0 or last descriptor index (depends on driver)   |

    | Register  | Description                     | What to write                                              |
    | --------- | ------------------------------- | ---------------------------------------------------------- |
    |   TDBAL   | TX Descriptor Base Address Low  | Lower 32 bits of TX ring physical address                  |
    |   TDBAH   | TX Descriptor Base Address High | Upper 32 bits of TX ring physical address (zero if 32-bit) |
    |   TDLEN   | TX Descriptor Ring Length       | Size in bytes of TX ring (e.g., 128 * 16 = 2048)           |
    |   TDH     | TX Descriptor Head              | Initially 0                                                |
    |   TDT     | TX Descriptor Tail              | Initially 0                                                |

    */

    Enable_e1000();

    return 0;
}

void Lock_EEPROM(void){
    //set the request bit to request access to the EEPROM
    //outl(REG_WIN, EECD_REG);
    dword Register = e1000_Read_Register(EECD_REG);
    if((Register & EECD_REQ) != 0){
        return;
    }
    e1000_Write_Register(EECD_REG, Register | EECD_REQ);

    // wait for the permission of the device to read it
    while((e1000_Read_Register(EECD_REG) & EECD_GNT) == 0);
}

void Unlock_EEPROM(void){
    //outl(REG_WIN, EECD_REG);
    dword Register = e1000_Read_Register(EECD_REG);
    if((Register & EECD_REQ) != 0){
        e1000_Write_Register(EECD_REG, Register ^ EECD_REQ);
    }
}

word Read_EEPROM(byte addr){
    dword tmp;
    word  data;
   
    // set the register window

    if(
           NIC_version == (INTEL_82547GI_ID & 0xffff)
        || NIC_version == (INTEL_82541EI_ID1 & 0xffff)
        || NIC_version == (INTEL_82541EI_ID2 & 0xffff)
        || NIC_version == (INTEL_82541ER_ID1 & 0xffff)
        || NIC_version == (INTEL_82541ER_ID2 & 0xffff)
        || NIC_version == (INTEL_82541GI_ID1 & 0xffff)
        || NIC_version == (INTEL_82541GI_ID2 & 0xffff)
        || NIC_version == (INTEL_82541PI_ID & 0xffff)
    ) {
        tmp = ((dword)addr & 0xfff) << 2;
    }else{
        tmp = ((dword)addr & 0xff) << 8;
    }

    
    tmp |= EERD_START;
    
    // send the register the address
    e1000_Write_Register(EERD_REG, tmp);

    // wait for it to be done reading
    while((e1000_Read_Register(EERD_REG) & EERD_DONE) == 0);

    // get the data
    data = (word)((e1000_Read_Register(EERD_REG) >> 16) & 0xFFFF);

    // stop reading
    tmp = e1000_Read_Register(EERD_REG);
    tmp &= ~(dword)EERD_START;
    e1000_Write_Register(EERD_REG, tmp);

    return data;
}

dword e1000_Read_Register(dword Reg){
    return *((volatile dword *)(e1000_Regs_Mem + Reg));
}

void e1000_Write_Register(dword Reg, dword Data){
    *((volatile dword *)(e1000_Regs_Mem + Reg)) = Data;
}

void Enable_e1000(void){
    // Enable receiver
    e1000_Write_Register(RCTL_REG,
        RCTL_EN |                       // Receiver Enable
        RCTL_BAM |                      // Broadcast Accept Mode
        RCTL_SECRC |                    // Strip Ethernet CRC
        (0 << RCTL_BSIZE_SHIFT)         // Buffer size (00 = 2048 bytes)
    );

    // Enable transmitter
    e1000_Write_Register(TCTL_REG,
        TCTL_EN |
        TCTL_PSP |
        (0x10 << TCTL_CT_SHIFT) |
        (0x40 << TCTL_COLD_SHIFT)
    );

}

void Disable_e1000(void){
    // Disable receiver
    e1000_Write_Register(RCTL_REG, ((~(dword)RCTL_EN) & e1000_Read_Register(RCTL_REG)));

    // Disable transmitter
    e1000_Write_Register(TCTL_REG, ((~(dword)TCTL_EN) & e1000_Read_Register(TCTL_REG)));
}

word e1000_SendPacket(void *data, word size){
 
    static dword TX_Cur = 0;

    if((tx_ring[TX_Cur].status & STATUS_DONE) == 0){// not ready to send
        return PACKET_SIZE + 1;    
    }

    word len = size > PACKET_SIZE ? PACKET_SIZE : size;

    memcpy(data, (void *)(dword)(tx_ring[TX_Cur].addr & 0xFFFFFFFF), len);

    tx_ring[TX_Cur].length = len;
    tx_ring[TX_Cur].status = 0;

    TX_Cur = ((TX_Cur + 1) % NUM_TX_DESC );
    e1000_Write_Register(TDT_REG, TX_Cur);

    return len;
}

struct e1000_rx_desc e1000_RscvPacket(void *buffer, word size){
    static dword RX_Cur = 0;

    struct e1000_rx_desc info = rx_ring[RX_Cur];

    if((rx_ring[RX_Cur].status & STATUS_DONE) == 0){// not ready to recieve
        return info;    
    }

    memcpy( (void *)(dword)(rx_ring[RX_Cur].addr & 0xFFFFFFFF),
            buffer, 
            size > rx_ring[RX_Cur].length ? rx_ring[RX_Cur].length : size
    );

    rx_ring[RX_Cur].status = 0;

    RX_Cur = ((RX_Cur + 1) % NUM_RX_DESC);
    e1000_Write_Register(RDT_REG, RX_Cur);

    return info;
}