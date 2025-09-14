#include "NVMe.h"

byte NVMe_Init(byte bus, byte slot, byte func){
    return (byte)(bus + slot + func) & 0xff;
}

byte nvme_read_sector(dword lba, void *buffer){
    return (byte)(lba + (dword)buffer) & 0xFF;
}