#include <Drivers/Disk/Disk.h>
#include <Memory/Heap/Palloc/Palloc.h>
#include <Memory/Operations/Operations.h>
#include <Graphics/VESA/VESA.h>
#include <General/Time/Time.h>

#include "NVMe.h"
#include "Data_Types.h"
#include "Registers.h"

static byte status = 0;

static uint64_t nvme_base_addr = 0;

static struct NVMe_Queue nvme_admin_submission_queue = {0};

static struct NVMe_Queue nvme_admin_completion_queue = {0};

// urgent - low priority queue
static struct NVMe_Queue nvme_io_submission_queue[NVME_MAX_PRIORITIES] = {0};
static struct NVMe_Queue nvme_io_completion_queue[NVME_MAX_PRIORITIES] = {0};


int NVMe_Init(byte bus, byte slot, byte func){

    if(Enable_Bus_Mastering(bus, slot, func) != 0){
        pstr_8x8("ERR: Failed to enable bus mastering on nvme device\n", FAIL);
        return 4;
    }

    
    /*uint32_t bar_buf_low = read_bar(bus, slot, func, NVME_REGISTER_BAR);
    uint32_t bar_buf_high = read_bar(bus, slot, func, NVME_REGISTER_BAR + 1);

    if(bar_buf_low == 0 || bar_buf_high == 0){
        pstr_8x8("ERR: invalid NVMe register bar\n", FAIL);
        return 3;
    }*/

    nvme_base_addr = (uint64_t)(uint32_t)pcalloc(2, PAGE_SIZE, PAGE_SIZE);

    write_bar(bus, slot, func, NVME_REGISTER_BAR + 1, (uint32_t)((nvme_base_addr >> 32) & 0xFFFFFFFF));
    write_bar(bus, slot, func, NVME_REGISTER_BAR, (uint32_t)((nvme_base_addr & 0xFFFFFFF0) | (0x2 << 1)));

    // set up admin queues
    int ret = Init_Admin_Queues(&nvme_admin_submission_queue, &nvme_admin_completion_queue, NVME_NUM_ADMIN_QUEUE_ENTRIES);

    if (ret != 0){
        pstr_8x8("NVME Err: couldn't initialize admin queues: code ", FAIL);
        pint_8x8(ret, FAIL);
        newline();
        return ret;
    }

    // set up IO queues
    ret = Init_IO_Queue(&nvme_admin_submission_queue, &nvme_admin_completion_queue, NVME_NUM_IO_QUEUE_ENTRIES);

    if (ret != 0){
        pstr_8x8("NVME Err: couldn't initialize IO queues: code ", FAIL);
        pint_8x8(ret, FAIL);
        newline();
        return ret;
    }

    /*if(returned == NULL){
        pstr_8x8("nvme error: ", WHITE);
        pint_8x8(status, WHITE);
        newline();
        while(1);
    }


    pstr_8x8("status: ", WHITE);
    pint_8x8(returned->status, WHITE);
    newline();
    while(1);*/

    return 0;
}

// sets up admin queues
int Init_Admin_Queues(
    struct NVMe_Queue *Admin_Submission_queue, 
    struct NVMe_Queue *Admin_Completion_queue, 
    uint16_t Entries
){

    // set up admin submission queue
    Admin_Submission_queue->queue_addr = (uint64_t)(uint32_t)pcalloc(PAGE_SIZE, 1, PAGE_SIZE);
    if(Admin_Submission_queue->queue_addr == (uint64_t)(uint32_t)NULL) {
        pstr_8x8("ERR: Couldn't allocate memory for nvme submission queue\n", FAIL);
        return 1;
    }
    Admin_Submission_queue->queue_size = Entries - 1;
    Admin_Submission_queue->index = ADMIN_QUEUE_ID;
    Admin_Submission_queue->position = 0;
    Admin_Submission_queue->type = NVME_ADMIN_SUBMISSION_QUEUE_TYPE;

    // set up admin completion queue
    Admin_Completion_queue->queue_addr = (uint64_t)(uint32_t)pcalloc(PAGE_SIZE, 1, PAGE_SIZE);
    if(Admin_Completion_queue->queue_addr == (uint64_t)(uint32_t)NULL) {
        pstr_8x8("ERR: Couldn't allocate memory for nvme completion queue\n", FAIL);
        return 2;
    }
    Admin_Completion_queue->queue_size = Entries - 1;
    Admin_Completion_queue->index = ADMIN_QUEUE_ID;
    Admin_Completion_queue->position = 0;
    Admin_Completion_queue->type = NVME_ADMIN_COMPLETION_QUEUE_TYPE;


    nvme_write_reg_4(AQA_REG, 
        (Admin_Submission_queue->queue_size & 0xFFF) |        // write the admin submission queue size (in entries)
        ((Admin_Completion_queue->queue_size & 0xFFF) << 16)  // write the admin completion queue size (in entries)
    );

    nvme_write_reg_8(ASQ_REG, Admin_Submission_queue->queue_addr); // write the admin submission queue address
    nvme_write_reg_8(ACQ_REG, Admin_Completion_queue->queue_addr); // write the admin completion queue address

    nvme_write_reg_4(SQxTDBL(Admin_Submission_queue->index), Admin_Submission_queue->position); // write the admin submission queue tail number
    nvme_write_reg_4(CQxHDBL(Admin_Completion_queue->index), Admin_Completion_queue->position); // write the admin completion queue head number

    return 0;
}

struct NVMe_Completion_Entry* NVMe_Run_Admin_command(
    struct NVMe_Submission_Entry Command, 
    struct NVMe_Queue *Admin_Submission_queue, 
    struct NVMe_Queue *Admin_Completion_queue
){
    if(Admin_Submission_queue->type != NVME_ADMIN_SUBMISSION_QUEUE_TYPE || Admin_Completion_queue->type != NVME_ADMIN_COMPLETION_QUEUE_TYPE){
        status = 3;
        return NULL;
    }else if(Admin_Submission_queue->position == Admin_Completion_queue->position - 1){ // queue full
        status = 1;
        return NULL;
    }

    volatile struct NVMe_Submission_Entry *sub_pointer = (volatile struct NVMe_Submission_Entry *)(uint32_t)(Admin_Submission_queue->queue_addr & 0xFFFFFFFF);
    volatile struct NVMe_Completion_Entry *com_pointer = (volatile struct NVMe_Completion_Entry *)(uint32_t)(Admin_Completion_queue->queue_addr & 0xFFFFFFFF);

    sub_pointer[Admin_Submission_queue->position] = Command;


    Admin_Submission_queue->position++;
    nvme_write_reg_4(SQxTDBL(Admin_Submission_queue->index), Admin_Submission_queue->position); // write the admin submission queue tail number

    int num = -1;
    uint32_t Start_time = PIT_Seconds;
    #define TIMEOUT 1 // wait one second
    while(num < 0){
        // wait for the command to return
        if(PIT_Seconds >= Start_time + TIMEOUT) {
            status = 2;
            Admin_Submission_queue->position--;
            nvme_write_reg_4(SQxTDBL(Admin_Submission_queue->index), Admin_Submission_queue->position);
            return NULL;
        }

        for(int i = 0; i < NVME_NUM_ADMIN_QUEUE_ENTRIES; i++){
            if(com_pointer[i].queue_identifier == (Admin_Submission_queue->position - 1)){
                // an entry with the submission queue id of the recently sent command
                num = i;
            }
        }
    }
    Admin_Completion_queue->position++;
    nvme_write_reg_4(CQxHDBL(Admin_Completion_queue->index), Admin_Completion_queue->position); // write the admin completion queue head number

    status = 0;
    return (struct NVMe_Completion_Entry *)&(com_pointer[num]);
}

int Init_IO_Queue(
    struct NVMe_Queue *Admin_Submission_queue, 
    struct NVMe_Queue *Admin_Completion_queue, 
    uint16_t Entries
){
    struct NVMe_Submission_Entry Command = {0};

    

    // for each priority
    for(int i = 0; i < NVME_MAX_PRIORITIES; i++){
        nvme_io_submission_queue[i].index = (uint8_t)(i + 1);
        nvme_io_submission_queue[i].position = 0;
        nvme_io_submission_queue[i].queue_addr = (uint64_t)(uint32_t)pcalloc(1, PAGE_SIZE, PAGE_SIZE);
        nvme_io_submission_queue[i].queue_size = Entries;
        nvme_io_submission_queue[i].type = NVME_IO_SUBMISSION_QUEUE_TYPE;

        Command.command = NVME_ADMIN_CREATE_IO_SUB_QUEUE;
        Command.NSID    = 0;
        Command.Data_addr[0] = nvme_io_submission_queue[i].queue_addr; // base physicall address of sumbission queue
        Command.Command_specific[0] = (uint32_t)(
            (1 + i) | // first 16 bits is the submission id
            ((Entries) << 16) // next 16 bits is the size
        );
        Command.Command_specific[1] = (uint32_t)(
            (1 + i) | // first 16 bits is the completion id
            ((i) << 16) | // priority
            (1 << 18) // physically contiguous
        );

        struct NVMe_Completion_Entry* Sub_returned = NVMe_Run_Admin_command(Command, Admin_Submission_queue, Admin_Completion_queue);

        if(Sub_returned == NULL){
            return (int)status; // if command call failed
        }
        if(Sub_returned->status != 0){
            return (int)Sub_returned->status; // if command failed
        }


        nvme_io_completion_queue[i].index = (uint8_t)(i + 1);
        nvme_io_completion_queue[i].position = 0;
        nvme_io_completion_queue[i].queue_addr = (uint64_t)(uint32_t)pcalloc(1, PAGE_SIZE, PAGE_SIZE);
        nvme_io_completion_queue[i].queue_size = Entries;
        nvme_io_completion_queue[i].type = NVME_IO_COMPLETION_QUEUE_TYPE;

        Command.command = NVME_ADMIN_CREATE_IO_COM_QUEUE;
        Command.NSID    = 0;
        Command.Data_addr[0] = nvme_io_completion_queue[i].queue_addr; // base physicall address of sumbission queue
        Command.Command_specific[0] = (uint32_t)(
            (1 + i) | // first 16 bits is the submission id
            ((Entries) << 16) // next 16 bits is the size
        );
        Command.Command_specific[1] = (uint32_t)(
            (1 + i) | // first 16 bits is the completion id
            ((i) << 16) | // priority
            (1 << 18) // physically contiguous
        );

        struct NVMe_Completion_Entry* Com_returned = NVMe_Run_Admin_command(Command, Admin_Submission_queue, Admin_Completion_queue);

        if(Com_returned == NULL){
            return (int)(status); // if command call failed
        }
        if(Com_returned->status != 0){
            return (int)(Com_returned->status + 3); // if command failed
        }
    }
    return 0;
}

/*
byte NVMe_read_sector(qword lba, void *buffer){
    
}

byte NVMe_write_sector(qword lba, void *buffer){

}

byte NVMe_bulk_read_sectors(qword Start_lba, void *buffer, word sectors){

}

byte NVMe_bulk_write_sectors(qword Start_lba, void *buffer, word sectors){

}*/

/*
byte NVMe_Send_IO_Command(
    uint8_t opcode, 
    uint32_t nsid, 
    void *data, 
    uint64_t lba, 
    uint16_t num_blocks, 
    struct NVMe_Completion_Entry *completion
){
	uint64_t sq_entry_addr = submission_queue.address + (submission_queue_tail * sizeof(nvme_command_entry));
	uint64_t cq_entry_addr = completion_queue.address + (completion_queue_head * sizeof(nvme_completion));
	nvme_command_entry command_entry;
	command_entry.opcode = opcode;
	command_entry.nsid = nsid;
	command_entry.prp1 = (uintptr_t)data;
	command_entry.prp2 = 0;
	command_entry.command_specific[0] = (uint32_t)lba;
	command_entry.command_specific[1] = (uint32_t)((uint64_t)lba >> 32);
	command_entry.command_specific[2] = (uint16_t)(num_blocks - 1);
	memcpy((void *)sq_entry_addr, &command_entry, sizeof(nvme_command_entry));
	submission_queue_tail++;
	nvme_write_reg(0x1000 + 2 * (4 << nvme_cap_strd), submission_queue_tail);
	if (submission_queue_tail == QUEUE_SIZE)
		submission_queue_tail = 0;
	// You should wait for completion here
	map_page(cq_entry_addr);
	completion = (nvme_completion *)cq_entry_addr;
	completion_queue_head++;
	nvme_write_reg(0x1000 + 3 * (4 << nvme_cap_strd), completion_queue_head);
	if (completion_queue_head == QUEUE_SIZE)
		completion_queue_head = 0;
	return completion->status != 0;
}*/

uint64_t nvme_read_reg_8(uint32_t offset) {
	volatile uint64_t *nvme_reg = (volatile uint64_t *)(uint32_t)((nvme_base_addr + offset) & 0xFFFFFFFF);
	return *nvme_reg;
}

void nvme_write_reg_8(uint32_t offset, uint64_t value) {
	volatile uint64_t *nvme_reg = (volatile uint64_t *)(uint32_t)((nvme_base_addr + offset) & 0xFFFFFFFF);
	*nvme_reg = value;
}

uint32_t nvme_read_reg_4(uint32_t offset) {
	volatile uint32_t *nvme_reg = (volatile uint32_t *)(uint32_t)((nvme_base_addr + offset) & 0xFFFFFFFF);
	return *nvme_reg;
}

void nvme_write_reg_4(uint32_t offset, uint32_t value) {
	volatile uint32_t *nvme_reg = (volatile uint32_t *)(uint32_t)((nvme_base_addr + offset) & 0xFFFFFFFF);
	*nvme_reg = value;
}