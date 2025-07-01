#ifndef VDFS_H
#define VDFS_H

#include "../Types/Common.h"

#define MAX_PATH_SIZE                       4096

#define IO_STORAGE_BLOCK                    122   // the starting block to store the IO ports
#define IO_STORAGE_BLOCKS                   10    //the amount of block for IO

#define FIRST_FREE_BLOCK                    IO_STORAGE_BLOCK + IO_STORAGE_BLOCKS
#define SUPERBLOCK_BLOCK                    FIRST_FREE_BLOCK
#define ROOT_START_BLOCK                    FIRST_FREE_BLOCK+1
#define DATA_START_BLOCK                    FIRST_FREE_BLOCK+2

#define MAX_ENTNAME_LENGTH                 19

#define SECTOR_ALLOCATED_FLAG              0b00000001

#define ENTRY_ALLOCATED_FLAG               0b10000000
#define FILE_FLAG                          0b00000001
#define DIRECTORY_FLAG                     0b00000010
#define HARDLINK_FLAG                      0b00000100

//#define MAX_BLOCKS_PER_FILE                 112// This was decided by the developers friend (His name is Kyle, He is sorry)

typedef struct Superblock {
    unsigned int magic;          // 0xDEADBEEF
    unsigned int total_sectors;  // Disk size
    unsigned int block_size;     // Usually 512 or 1024
    unsigned int root_dir_start; // Sector number of root dir
    unsigned int is_first_boot;  // whether its the first boot or not
    char padding[512 - (5 * sizeof(unsigned int))];
}Superblock;

//size is 5 bytes + MAX_ENTNAME_LENGTH
typedef struct Dir_Entry {
    byte flags;                         // flags like what it is (file or dir)
    char Entname[MAX_ENTNAME_LENGTH];   // the name of the entry
    byte Metadata_Block[4];             // the address of the metadata of the file
}Dir_Entry;

typedef struct Dir_Metadata {
    Dir_Entry Entries[(504 - (504 % sizeof(Dir_Entry))) / sizeof(Dir_Entry)]; // all of the entries dynamicly* allocated dedpendent on the filename 
    byte Next_Metadata_Block[4];                                              // the block address of the next metadata block allocated to this directory  (only use if its non zero meaning it needs to be used)
    byte flags[4];                                                            // flags like the allocation flag
}Dir_Metadata;

typedef struct File_Entry {
    byte Sector[4]; // the sector this entry has
}File_Entry;

typedef struct File_Metadata {
    File_Entry Entries[(500 - (500 % sizeof(File_Entry))) / sizeof(File_Entry)];// all of the entries
    byte Next_Metadata_Block[4];                                                // sector number of the next sector allocated to this file
    byte File_info[4];                                                           // actual metadata (ex: size)
    byte flags[4];                                                              // flags like the allocation flag
}File_Metadata;

typedef struct File_info {
    byte Padding[512 - (sizeof(int) * 0)];
}File_info;

/*
st_perm: file permissions.
st_dev: Device ID of the device containing the file.
st_mode: File type.
st_nlink: Number of hard links to the file.
st_uid: User ID of the file owner.
st_gid: Group ID of the file owner's group.
st_rdev: Device ID (if the file is a character or block device).
st_size: Size of the file in bytes (if it is a regular file).
st_atime: Time of last access.
st_mtime: Time of last modification.
st_ctime: Time of last status change.
st_blocks: Number of blocks allocated for the file.
*/

//prints the errors in the rror register of the ata drive
void ata_print_err(byte error);

//write buffer into the lba-th sector
byte ata_write_sector(dword lba, void* buffer);

//writes buffer to multiple sectors starting at Start_lba and writing Sectors sectors
byte ata_bulk_write_sectors(dword Start_lba, void* buffer, byte Sectors);

//read the lba-th sector into buffer
byte ata_read_sector(dword lba, void* buffer);

//reads multiple sectors into buffer starting at Start_lba and reading Sectors sectors
byte ata_bulk_read_sectors(dword Start_lba, void* buffer, byte Sectors);

//returns the first free sector (may fail alot (IDK) returns 0 if its not found)
unsigned int Find_First_Free_Sector(void);

//initializes the file system should only be run once ever
void FS_Init(void);

// reads in the superblock
byte Read_Superblock(void);

//initializes the superblock do not worry it is also runs in FS_Init
void Superblock_Init(void);

//allocates a block, do not use outside of file and directory creating functions
unsigned int balloc(void);

//frees an allocated block, do not use unless dealing with removing sectors or parts of files also use other functions for that
void freeb(unsigned int sector);

void free_file_Metadata(unsigned int lba);

void free_Dir_Metadata(unsigned int lba);

// could be implemented in more places then it is but i dont want to change it because im lazy
unsigned int get_Metadata_Sector_From_Parent_Dir_Sector(const unsigned int Parent, const char* Entname);

byte get_flags_From_Parent_Dir_Sector(const unsigned int Parent, const char* Entname);

//with format /dir1/dir2/dir3
unsigned int get_Dir_Sector_From_Path_Absolute(const char* Path);

//returns the lba of the metadata sector of the file with a format of /dir1/dir2/file
unsigned int get_file_Metadata_Sector_From_Path_Absolute(const char* Path);

//adds an entry to a dir
byte add_Dir_Entry(unsigned int Dir_Metadata_Sector, Dir_Entry Entry);

//removes an entry from a dir
byte del_Dir_Entry(unsigned int Dir_Metadata_Sector, char* Entname);

//returns a Dir_Entry type based on the parameters (converts lba to proper type) (depricated?)
//Dir_Entry Get_Dir_Entry(byte flags, char* Entname, unsigned int lba);

// format: /dir1/dir2/dir3/file
byte add_File_With_Path(char* Path);

//format: /dir1/dir2/dir3/newdir
byte add_Dir_With_Path(char* Path);

//writes to a file's data sectors (0 = first sector, 1 = second sector) only 508 bytes per sector
void write_file_Sector(char* Path, unsigned int Sector, void* buf);

//reads a file's data sectors (0 = first sector, 1 = second sector) only 508 bytes per sector
void read_file_Sector(char* Path, unsigned int Sector, void *buf);

#endif