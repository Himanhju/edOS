#ifndef NVME_DATA_TYPES
#define NVME_DATA_TYPES

#include <Types/Common.h>

#define NVME_ADMIN_SUBMISSION_QUEUE_TYPE    0b00
#define NVME_ADMIN_COMPLETION_QUEUE_TYPE    0b01
#define NVME_IO_SUBMISSION_QUEUE_TYPE       0b10
#define NVME_IO_COMPLETION_QUEUE_TYPE       0b11

struct NVMe_Queue {
    uint64_t queue_addr;    // the address of the queue
    uint64_t queue_size;    // size of the queue (in entries) 
    uint16_t type:2;        // high bit determines queue type (admin:0 IO:1) low bit determines direction (0:submission 1:completion)
    uint16_t position:6;    // the current position of the tail/head
    uint16_t index:8;       // the index of the queue (ex admin=0)
};

struct NVMe_Submission_Entry {
    
    /*
    command bits:
    0 - 7 opcode
    8 - 9 Fused operation (0 = normal operation)
    10 - 13 reserved
    14 - 15 PRP/SGL selection (0 = PRP)
    16 - 31 Command identifier (put in completion entry)
    */

    uint32_t command;
    uint32_t NSID; // namespace identifier. 0 if not used
    uint32_t reserved[2];
    uint64_t Metadata_addr;
    uint64_t Data_addr[2];
    uint32_t Command_specific[6];
};

struct NVMe_Completion_Entry {
    uint32_t command_specific;
    uint32_t reserved;
    uint16_t queue_head_pointer;
    uint16_t queue_identifier;
    uint16_t command_identifier;
    uint16_t phase_bit:1; // set if entry writen
    uint16_t status:15; // 0 on success
};

#define NVME_MAX_PRIORITIES 4

enum {
  NVME_IO_QUEUE_PRIORITY_URGENT,
  NVME_IO_QUEUE_PRIORITY_HIGH,
  NVME_IO_QUEUE_PRIORITY_MEDIUM,
  NVME_IO_QUEUE_PRIORITY_LOW
};

#endif