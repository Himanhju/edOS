#ifndef SATA_REGS
#define SATA_REGS

//Global register
#define CAP_REG         0x00
#define GHC_REG         0x04
#define IS_REG          0x08
#define PI_REG          0x0C
#define VS_REG          0x10
#define CCC_CTL_REG     0x14
#define CCC_PORTS_REG   0x18
#define EM_LOC_REG      0x1C
#define EM_CTL_REG      0x20
#define CAP2_REG        0x24
#define BOHC_REG        0x28

#define PORT_REG_BASE      0x100
#define PORT_OFFSET(n)     (PORT_REG_BASE + (n) * 0x80)

#define PORT_CLB(n)        (PORT_OFFSET(n) + 0x00)
#define PORT_CLBU(n)       (PORT_OFFSET(n) + 0x04)
#define PORT_FB(n)         (PORT_OFFSET(n) + 0x08)
#define PORT_FBU(n)        (PORT_OFFSET(n) + 0x0C)
#define PORT_IS(n)         (PORT_OFFSET(n) + 0x10)
#define PORT_IE(n)         (PORT_OFFSET(n) + 0x14)
#define PORT_CMD(n)        (PORT_OFFSET(n) + 0x18)
#define PORT_TFD(n)        (PORT_OFFSET(n) + 0x20)
#define PORT_SIG(n)        (PORT_OFFSET(n) + 0x24)
#define PORT_SSTS(n)       (PORT_OFFSET(n) + 0x28)
#define PORT_SCTL(n)       (PORT_OFFSET(n) + 0x2C)
#define PORT_SERR(n)       (PORT_OFFSET(n) + 0x30)
#define PORT_SACT(n)       (PORT_OFFSET(n) + 0x34)
#define PORT_CI(n)         (PORT_OFFSET(n) + 0x38)
#define PORT_SNTF(n)       (PORT_OFFSET(n) + 0x3C)
#define PORT_FBS(n)        (PORT_OFFSET(n) + 0x40)

#endif