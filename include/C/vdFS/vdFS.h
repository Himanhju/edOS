#ifndef VDFS_H
#define VDFS_H

#include "../Types/Common.h"

#define FIRST_FREE_BLOCK                    121
#define SUPERBLOCK_BLOCK                    FIRST_FREE_BLOCK
#define ROOT_START_BLOCK                    FIRST_FREE_BLOCK+1
#define DATA_START_BLOCK                    FIRST_FREE_BLOCK+2

#define MAX_ENTNAME_LENGTH                 19

#define ENTRY_ALLOCATED_FLAG               0b10000000
#define FILE_FLAG                          0b00000001
#define DIRECTORY_FLAG                     0b00000010

//#define MAX_BLOCKS_PER_FILE                 112// This was decided by the developers friend (His name is Kyle, He is sorry)

typedef struct Superblock {
    unsigned int magic;          // 0xDEADBEEF
    unsigned int total_sectors;  // Disk size
    unsigned int block_size;     // Usually 512 or 1024
    unsigned int root_dir_start; // Sector number of root dir
    char padding[512 - (4 * sizeof(unsigned int))];
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
    byte Metadata[4];                                                           // actual metadata (ex: size)
    byte flags[4];                                                              // flags like the allocation flag
}File_Metadata;


//write buffer into the lba-th sector
void ata_write_sector(dword lba, void* buffer);

//read the lba-th sector into buffer
void ata_read_sector(dword lba, void* buffer);

//returns the first free sector (may fail alot (IDK) returns 0 if its not found)
unsigned int Find_First_Free_Sector(void);

//initializes the file system should only be run once ever
void FS_Init(void);

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

//returns a Dir_Entry type based on the parameters (converts lba to proper type)
Dir_Entry Get_Dir_Entry(byte flags, char* Entname, unsigned int lba);

// format: /dir1/dir2/dir3/file
byte add_File_With_Path(char* Path);

//format: /dir1/dir2/dir3/newdir
byte add_Dir_With_Path(char* Path);

#endif