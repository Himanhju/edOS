#include "../../Coms/PCI/PCI.h"
#include "../../Graphics/VESA.h"
#include "../../Memory/Opcodes/Opcodes.h"
#include "../../Memory/Palloc/Palloc.h"

#include "Inet.h"

static volatile word  INET_Base_Port = 0xFFFF;
static volatile byte *INET_Regs_Mem;

byte MAC_Addr[6] = {0};


// RX ring
//struct e1000_rx_desc rx_ring[NUM_RX_DESC];
struct e1000_rx_desc* rx_ring;
byte *rx_buffers[NUM_RX_DESC];//[PACKET_SIZE];
                                            // TODO: FINISH PALLOC AND ALLOCATE THESE WITH THAT
// TX ring
//struct e1000_tx_desc tx_ring[NUM_TX_DESC];
struct e1000_tx_desc* tx_ring;
byte tx_buffers[NUM_TX_DESC];//[PACKET_SIZE];


// takes a pointer to the INET IO space for bars types and device IDs to write to them
byte INET_Init(IO INET_IO){
    RGB white = Get_RGB(0xffffff);

    byte bus  = INET_IO.Device.bus;
    byte slot = INET_IO.Device.slot;
    byte func = INET_IO.Device.func;
    

    INET_Regs_Mem = (byte *)read_bar(bus, slot, func, 0);
    if((((dword)INET_Regs_Mem) & 0x00000006) >> 1 == 2){
        pstr_8x8("64-Bit address returned?????\n", white); // SHOULDN'T BE RUN, WE HAVE PROBLEMS IF IT IS
    }else{
        INET_Regs_Mem = (void *)(((dword)INET_Regs_Mem) & 0xFFFFFFF0);
    }

    
    for(byte i = 1; i < MAX_BARS; i++){
        dword cur_bar = read_bar(bus, slot, func, i);
        if((cur_bar & 1) == 1){
            INET_Base_Port = (word)(cur_bar & 0x0000FFFF);
            break;
        }
    }

    if(INET_Base_Port == 0xFFFF){
        pstr_8x8("Could not initialize the NIC: no window\n", white);
        return 1;
    }
    
    dword Reg_Buf;
    

    word cmd = pciConfigRead_Word(bus, slot, func, PCICMD_OFFSET);
    // Enable memory space and bus mastering
    cmd |= (PCICMD_MEM | PCICMD_MASTER);
    pciConfigWrite_Word(bus, slot, func, PCICMD_OFFSET, (word)cmd);
    cmd = 0;

    cmd = pciConfigRead_Word(bus, slot, func, PCICMD_OFFSET);

    if((cmd & (PCICMD_MEM | PCICMD_MASTER)) == 0){
        pstr_8x8("Could not initialize the NIC: bus mastering failed\n", white);
        return 1;
    }


    // reset the device

    Write_Register(CTRL_REG, Read_Register(CTRL_REG) | CTRL_RST);


    while((Read_Register(CTRL_REG) & CTRL_RST) != 0); //wait for it to be reset

    
    // allow software to read the EEPROM of the device
    //outl(REG_WIN, EECD_REG);
    Reg_Buf = Read_Register(EECD_REG);
    Write_Register(EECD_REG, Reg_Buf | EECD_SK | EECD_CS | EECD_DI);

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


    // tell the nic where the rx descriptor table is 
    Write_Register(RDBAL_REG, ((dword)rx_ring & 0xFFFFFFFF));
    Write_Register(RDBAH_REG, 0x00000000); // not in 64 bit mode so no need to have this be non-zero
    Write_Register(RDLEN_REG, (dword)(16 * NUM_RX_DESC));
    Write_Register(RDH_REG, 0x00000000);
    Write_Register(RDT_REG, (dword)NUM_RX_DESC - 1);

    // tell the nic where the tx descriptor table is 
    Write_Register(TDBAL_REG, (dword)tx_ring);
    Write_Register(TDBAH_REG, 0x00000000); // not in 64 bit mode so no need to have this be non-zero
    Write_Register(TDLEN_REG, (dword)(16 * NUM_TX_DESC));
    Write_Register(TDH_REG, 0x00000000);
    Write_Register(TDT_REG, 0x00000000); // dont have anything to send currently

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

    // Enable receiver
    Write_Register(RCTL_REG,
        RCTL_EN |       // Receiver Enable
        RCTL_BAM |      // Broadcast Accept Mode
        RCTL_SECRC |    // Strip Ethernet CRC
        (0 << RCTL_BSIZE_SHIFT)       // Buffer size (00 = 2048 bytes)
    );

    // Enable transmitter
    Write_Register(TCTL_REG,
        TCTL_EN |
        TCTL_PSP |
        (0x10 << TCTL_CT_SHIFT) |
        (0x40 << TCTL_COLD_SHIFT)
    );


    return 0;
}

void Lock_EEPROM(void){
    //set the request bit to request access to the EEPROM
    //outl(REG_WIN, EECD_REG);
    dword Register = Read_Register(EECD_REG);
    if((Register & EECD_REQ) != 0){
        return;
    }
    Write_Register(EECD_REG, Register | EECD_REQ);

    // wait for the permission of the device to read it
    while((Read_Register(EECD_REG) & EECD_GNT) == 0);
}

void Unlock_EEPROM(void){
    //outl(REG_WIN, EECD_REG);
    dword Register = Read_Register(EECD_REG);
    if((Register & EECD_REQ) != 0){
        Write_Register(EECD_REG, Register ^ EECD_REQ);
    }
}

word Read_EEPROM(byte addr){
    dword tmp;
    word  data;
   
    // set the register window
    //outl(REG_WIN, EERD_REG);

    tmp = ((dword)addr & 0xff) << 8;
    tmp |= EERD_START;
    
    // send the register the address
    Write_Register(EERD_REG, tmp);

    // wait for it to be done reading
    while((Read_Register(EERD_REG) & EERD_DONE) == 0);

    // get the data
    data = (word)((Read_Register(EERD_REG) >> 16) & 0xFFFF);

    // stop reading
    tmp = Read_Register(EERD_REG);
    tmp &= ~(dword)EERD_START;
    Write_Register(EERD_REG, tmp);

    return data;
}

dword Read_Register(dword Reg){
    return *((volatile dword *)(INET_Regs_Mem + Reg));
}

void Write_Register(dword Reg, dword Data){
    *((volatile dword *)(INET_Regs_Mem + Reg)) = Data;
}
