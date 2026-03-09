#ifndef NVME_H
#define NVME_H

#include <Types/Common.h>
#include "Data_Types.h"
#include "Commands.h"
#include "Registers.h"

#define NVME_REGISTER_BAR               0

#define ADMIN_QUEUE_ID                  0

#define NVME_NUM_ADMIN_QUEUE_ENTRIES    64
#define NVME_NUM_IO_QUEUE_ENTRIES       64

int NVMe_Init(byte bus, byte slot, byte func);

int Init_Admin_Queues(
    struct NVMe_Queue *Submission_queue, 
    struct NVMe_Queue *Completion_queue, 
    uint16_t Entries
);

//actual errors recorded in status
struct NVMe_Completion_Entry* NVMe_Run_Admin_command(
    struct NVMe_Submission_Entry Command, 
    struct NVMe_Queue *Submission_queue, 
    struct NVMe_Queue *Completion_queue
); 

int Init_IO_Queue(
    struct NVMe_Queue *Admin_Submission_queue, 
    struct NVMe_Queue *Admin_Completion_queue, 
    uint16_t Entries
);

byte nvme_read_sector(qword lba, void *buffer);

byte nvme_write_sector(qword lba, void *buffer);

byte nvme_bulk_read_sectors(qword Start_lba, void *buffer, word sectors);

byte nvme_bulk_write_sectors(qword Start_lba, void *buffer, word sectors);

uint64_t nvme_read_reg_8(uint32_t offset);

void nvme_write_reg_8(uint32_t offset, uint64_t value);

uint32_t nvme_read_reg_4(uint32_t offset);

void nvme_write_reg_4(uint32_t offset, uint32_t value);

#endif