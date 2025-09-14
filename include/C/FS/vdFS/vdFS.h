#ifndef VDFS_H
#define VDFS_H

#include <Types/Common.h>
#include <Drivers/Disk/Disk.h>

#define MAX_PATH_SIZE                       4096

#define ROOT_START_BLOCK                    FIRST_FREE_SECTOR
#define DATA_START_BLOCK                    FIRST_FREE_SECTOR+1

#define MAX_ENTNAME_LENGTH                 19

#define SECTOR_ALLOCATED_FLAG              0x1

#define ENTRY_ALLOCATED_FLAG               0x80
#define FILE_FLAG                          0x1
#define DIRECTORY_FLAG                     0x2
#define HARDLINK_FLAG                      0x4

//#define MAX_BLOCKS_PER_FILE                 112// This was decided by the developers friend (His name is Kyle, He is sorry)

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
    byte Padding[SECTOR_SIZE - (sizeof(int) * 0)];
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

//returns the first free sector (may fail alot (IDK) returns 0 if its not found)
unsigned int Find_First_Free_Sector(void);

//initializes the file system should only be run once ever
void vdFS_Init(void);

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

// actually reads the data from the files metadata
void Read_File(char* Path, void *buf, dword len);

// actaully writes the data from the files metadata
void Write_File(char* Path, void *buf, dword len);

#endif