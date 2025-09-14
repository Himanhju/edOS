#ifndef INET_REGS
#define INET_REGS

//reg info: 233

// the CTRL register in the device
#define CTRL_REG    0x00
// the bits in the CTRL register
#define CTRL_FD              (1)
#define CTRL_LRST            (1 << 3)
#define CTRL_ASDE            (1 << 5)
#define CTRL_SLU             (1 << 6)
#define CTRL_ILOS            (1 << 7)
#define CTRL_SPEED           (0x300)
#define CTRL_FRCSPD          (1 << 11)
#define CTRL_FRCDPLX         (1 << 12)
#define CTRL_SDP0_DATA       (1 << 18)
#define CTRL_SDP1_DATA       (1 << 19)
#define CTRL_ADVD3WUC        (1 << 20)
#define CTRL_EN_PHY_PWR_MGMT (1 << 21)
#define CTRL_SDP0_IODIR      (1 << 22)
#define CTRL_SDP1_IODIR      (1 << 23)
#define CTRL_RST             (1 << 26)
#define CTRL_RFCE            (1 << 27)
#define CTRL_TFCE            (1 << 28)
#define CTRL_VME             (1 << 30)
#define CTRL_PHY_RST         (1 << 31)


// the STATUS register in the device
#define STATUS_REG  0x08
// the bits in the STATUS register
#define STATUS_FD           (1)
#define STATUS_LU           (1 << 1)
#define STATUS_FUNCID       (0xC)
#define STATUS_TXOFF        (1 << 4)
#define STATUS_TBIMODE      (1 << 5)
#define STATUS_SPED         (0xC0)
#define STATUS_ASDV         (0x300)
#define STATUS_PCI66        (1 << 11)
#define STATUS_BUS64        (1 << 12)
#define STATUS_PCIX_MODE    (1 << 13)
#define STATUS_PCIXSPD      (0xC000)


// the EECD register in the device
#define EECD_REG    0x10
// the bits in the EECD register
#define EECD_SK     (1)
#define EECD_CS     (1 << 1)
#define EECD_DI     (1 << 2)
#define EECD_DO     (1 << 3)
#define EECD_FWE    (0x30)
#define EECD_REQ    (1 << 6)
#define EECD_GNT    (1 << 7)
#define EECD_PRES   (1 << 8)
#define EECD_SIZE   (1 << 9)
#define EECD_TYPE   (1 << 13)


// the EERD register in the device
#define EERD_REG    0x14
// the bits in the EERD register
#define EERD_START  (1)
#define EERD_DONE   (1 << 4)
#define EERD_ADDR   (0xFF00)
#define EERD_DATA   (0xFFFF0000)

// the FLA register in the device (flash)
#define FLA_REG     0x1C
// the bits in the FLA register
#define FLA_SCK     (1)
#define FLA_CE      (1 << 1)
#define FLA_SI      (1 << 2)
#define FLA_SO      (1 << 3)
#define FLA_REQ     (1 << 4)
#define FLA_GNT     (1 << 5)
#define FLA_BUSY    (1 << 30)
#define FLA_ER      (1 << 31)

// the RCTL register in the device (Receive Control)
#define RCTL_REG    0x00100
// the bits in the RCTL register
#define RCTL_EN             (1 << 1)
#define RCTL_SBP            (1 << 2)
#define RCTL_UPE            (1 << 3)
#define RCTL_MPE            (1 << 4)
#define RCTL_LPE            (1 << 5)
#define RCTL_LBM            (0xC0)
#define RCTL_RDMTS          (0x300)
#define RCTL_MO             (0x3000)
#define RCTL_BAM            (1 << 15)
#define RCTL_BSIZE          (0x30000)
#define RCTL_BSIZE_SHIFT    (16)
#define RCTL_VFE            (1 << 18)
#define RCTL_CFIEN          (1 << 19)
#define RCTL_CFI            (1 << 20)
#define RCTL_DPF            (1 << 22)
#define RCTL_PMCF           (1 << 23)
#define RCTL_BSEX           (1 << 25)
#define RCTL_SECRC          (1 << 26)

// the FCRTL register in the device (Flow Control Receive Threshold Low)
#define FCRTL_REG   0x02160

// the FCRTH register in the device (Flow Control Receive Threshold High)
#define FCRTH_REG   0x02168

// the RDBAL register in the device (Receive Descriptor base low)
#define RDBAL_REG   0x02800

// the RDBAH register in the device (Receive Descriptor base high)
#define RDBAH_REG   0x02804

// the RDLEN register in the device (Receive Descriptor Length)
#define RDLEN_REG   0x02808

// the RDH register in the device (Receive Descriptor Head)
#define RDH_REG     0x02810

// the RDT register in the device (Receive Descriptor Tail)
#define RDT_REG     0x02818

// the RDTR register in the device (Receive Delay Timer)
#define RDTR_REG    0x02820

// the RADV register in the device (Receive Interrupt Absolute Delay Timer)
#define RADV_REG    0x0282C

// the RSRPD register in the device (Receive Small Packet Detect Interrupt)
#define RSRPD_REG   0x02C00

// the TCTL register in the device (Transmit Control)
#define TCTL_REG    0x00400
// the bits in the TCTL register
#define TCTL_EN         (1 << 1)
#define TCTL_PSP        (1 << 3)
#define TCTL_CT         (0xFF0)
#define TCTL_CT_SHIFT   (4)
#define TCTL_COLD       (0x3FF000)
#define TCTL_COLD_SHIFT (12)
#define TCTL_SWXOFF     (1 << 22)
#define TCTL_RTLC       (1 << 24)

// the TIPG register in the device (Transmit IPG)
#define TIPG_REG    0x00410

// the AIFS register in the device (Adaptive IFS Throttle - AIT)
#define AIFS_REG    0x00458

// the TDBAL register in the device (Transmit Descriptor Base Low)
#define TDBAL_REG   0x03800

// the TDBAH register in the device (Transmit Descriptor Base High)
#define TDBAH_REG   0x03804

// the TDLEN register in the device (Transmit Descriptor Length)
#define TDLEN_REG   0x03808

// the TDH register in the device (Transmit Descriptor Head)
#define TDH_REG     0x03810

// the TDT register in the device (Transmit Descriptor Tail)
#define TDT_REG     0x03818

// the TIDV register in the device (Transmit Interrupt Delay Value)
#define TIDV_REG    0x03820

// the TXDMAC register in the device (TX DMA Control)
#define TXDMAC_REG  0x03000

// the TXDCTL register in the device (Transmit Descriptor Control)
#define TXDCTL_REG  0x03828

// the TADV register in the device (Transmit Absolute Interrupt Delay Timer)
#define TADV_REG    0x0282C

#endif