#ifndef NVME_REGS
#define NVME_REGS

#include "NVMe.h"
#include <Memory/Operations/Operations.h>

#define CAP_REG         0x00    //  Controller Capabilities (size = 8)
#define VER_REG         0x08    //  Version                 (size = 4)
#define INTMS_REG       0x0C    //  Interrupt Mask Set      (size = 4)
#define INTMC_REG       0x10    //  Interrupt Mask Clear    (size = 4)
#define CC_REG          0x14    //  Controller Config       (size = 8)
#define CSTS_REG        0x1C    //  Controller Status       (size = 8)
#define AQA_REG         0x24    //  Admin Queue Attributes  (size = 4)
#define ASQ_REG         0x28    //  Admin Submission Queue  (size = 8)
#define ACQ_REG         0x30    //  Admin Completion Queue  (size = 8)

#define SQxTDBL(x)      (uint32_t)(0x1000 + (((2 * x)) * DOORBELL_STRIDE))    //  Submission Queue X Tail Doorbell
#define CQxHDBL(x)      (uint32_t)(0x1000 + (((2 * x) + 1) * DOORBELL_STRIDE))//  Completion Queue X Tail Doorbell

// Capabilities
#define MQES            ((nvme_read_reg_8(CAP_REG) >> 00) & 0xFFFF)     //  Max Queue Entries Supported
#define CQR             ((nvme_read_reg_8(CAP_REG) >> 16) & 0x0001)     //  Contiguous Queues Required
#define AMS             ((nvme_read_reg_8(CAP_REG) >> 17) & 0x0003)     //  Arbitration Mechanism Supported
// reserved bits (5)
#define TO              ((nvme_read_reg_8(CAP_REG) >> 24) & 0x00FF)     //  Timeout (max number of half seconds host should wait for either the device to be ready or be busy)
#define DSTRD           ((nvme_read_reg_8(CAP_REG) >> 32) & 0x000F)     //  Doorbell Stride (actual stride is 2^(2 + this register))
#define NSSRS           ((nvme_read_reg_8(CAP_REG) >> 36) & 0x0001)     //  NVM Subsystem Reset Supported (1-yes 0-no)
#define CSS             ((nvme_read_reg_8(CAP_REG) >> 37) & 0x00FF)     //  Command Sets Supported (bit 37 = NVM command set, rest are reserved for future additions)
// reserved bits (3)
#define MPSMIN          ((nvme_read_reg_8(CAP_REG) >> 48) & 0x000F)     //  Memory Page Size Minimum (actual size is 2^(12 + this register))
#define MPSMAX          ((nvme_read_reg_8(CAP_REG) >> 52) & 0x000F)     //  Memory Page Size Maximum (actual size is 2^(12 + this register))
// reserved bits (8)

#define DOORBELL_STRIDE (4 << DSTRD)

#endif