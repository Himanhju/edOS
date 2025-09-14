#ifndef NVME_H
#define NVME_H

#include <Types/Common.h>

byte NVMe_Init(byte bus, byte slot, byte func);

byte nvme_read_sector(dword lba, void *buffer);

byte nvme_write_sector(dword lba, void *buffer);

#endif