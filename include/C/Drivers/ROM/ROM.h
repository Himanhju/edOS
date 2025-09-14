#ifndef ROM_H
#define ROM_H

#define FSINFO_BASE     0x00000200
#define FSINFO_SIZE     0x00000200

#define BDA_BASE        0x00000400
#define BDA_SIZE        0x00000100

#define BDA_SERIAL_PIO  0x00 // (4 words)     IO ports for COM1-COM4 serial (each address is 1 word, zero if none)
#define BDA_LPTS_PIO    0x08 // (3 words)	    IO ports for LPT1-LPT3 parallel (each address is 1 word, zero if none)
#define BDA_EBDA_BASE   0x0E // (word)	    EBDA base address >> 4 (usually!)
#define BDA_HARD_FLAGS  0x10 // (word)	    packed bit flags for detected hardware
#define BDA_KB_BEF_EBDA 0x13 // (word)	    Number of kilobytes before EBDA / unusable memory
#define BDA_KEY_STATE   0x17 // (word)	    keyboard state flags
#define BDA_KEY_BUF     0x1E // (32 bytes)	keyboard buffer
#define BDA_DISP_MODE   0x49 // (byte)	    Display Mode
#define BDA_COL_TEXT    0x4A // (word)	    number of columns in text mode
#define BDA_VID_PIO     0x63 // (2 bytes, taken as a word)	base IO port for video
#define BDA_TICK_UPTIME 0x6C // (word)	    # of IRQ0 timer ticks since boot
#define BDA_DRIVES      0x75 // (byte)	    # of hard disk drives detected
#define BDA_KEY_BUF_ST  0x80 // (word)	    keyboard buffer start
#define BDA_KEY_BUF_END 0x82 // (word)	    keyboard buffer end
#define BDA_SH_KEY_STAT 0x97 // (byte)	    last keyboard LED/Shift key state

#define BOOTLOADER_BASE 0x00007C00
#define BOOTLOADER_SIZE 0x00000200

#define VID_BIOS_BASE   0x000C0000
#define VID_BIOS_SIZE   0x00008000

#define BIOS_EXP_BASE   0x000C8000
#define BIOS_EXP_SIZE   0x00028000

#define BIOS_BASE       0x000F0000
#define BIOS_SIZE       0x00010000

#include <Types/Common.h>
#include <Types/stdint.h>

typedef struct bios_parameter_block {
    uint8_t jmp_cmd[3]; //0x00
    char OEM[8]; //0x03
    uint16_t bytes_per_sector; //0x0b
    uint8_t sectors_per_cluster; //0x0d
    uint16_t reserved_sectors; //0x0e
    uint8_t FAT_count; //0x10
    uint16_t dir_entries; //0x11
    uint16_t total_sectors; //0x13
    uint8_t media_descriptor_type; //0x15
    uint16_t count_sectors_per_FAT12_16; //0x16
    uint16_t count_sectors_per_track; //0x18
    uint16_t count_heads_or_sides_on_media; //0x1a
    uint32_t count_hidden_sectors; //0x1c
    uint32_t large_sectors_on_media; //0x20

    // Extended Boot Record
    uint32_t count_sectors_per_FAT32; //0x24
    uint16_t flags; //0x28
    uint16_t FAT_version; //0x2a
    uint32_t cluster_number_root_dir; //0x2c
    uint16_t sector_number_FSInfo; //0x30
    uint16_t sector_number_backup_boot_sector; //0x32
    char Reserved[12]; //0x34
    uint8_t drive_number; //0x40
    uint8_t windows_flags; //0x41
    uint8_t signature; //0x42
    uint32_t volume_id; //0x43
    char volume_label[11]; //0x47
    char system_id[8]; //0x52
}BPB;

typedef struct Boot_Sector {
    BPB BP_Block;
    byte Code[512 - (sizeof(BPB) + sizeof(word))];
    word Boot_Signature;
}Boot_Sector; 

extern dword EDBA_BASE;
extern dword EDBA_SIZE;

void ROM_Init(void);

byte  BDA_Read_Byte(byte addr);

word  BDA_Read_Word(byte addr);

dword BDA_Read_Dword(byte addr);

byte  EDBA_Read_Byte(byte addr);

word  EDBA_Read_Word(byte addr);

dword EDBA_Read_Dword(byte addr);

#endif

