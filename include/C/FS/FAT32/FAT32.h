#ifndef FAT32_H
#define FAT32_H

#include <Types/Common.h>

#define SECT_PER_CLUST      16
#define BYTES_PER_CLUST     (SECT_PER_CLUST * SECTOR_SIZE)
#define NUM_FATS            2
#define NUM_SECTS           0x200000 * 10
#define FAT_ENTS_PER_SECT   (SECTOR_SIZE / sizeof(dword))

#define EOC                 0x0FFFFFFF
#define RESERVED            0x0FFFFF00
#define FREE                0x00000000
#define DEV                 0x0ffffffa

#define FAT32_ATTR_RO       0x01
#define FAT32_ATTR_HIDDEN   0x02
#define FAT32_ATTR_SYSTEM   0x04
#define FAT32_ATTR_VOLUME   0x08
#define FAT32_ATTR_DEV      0x00 // Device File, Stores Data On Devices/In Memory (ex: framebuffer, Drives, Input, Inet, etc.)
#define FAT32_ATTR_SUBDIR   0x10
#define FAT32_ATTR_FILE     0x20 // Normal File, Stores Data On The Drive

#define DIR_CLUSTER         2

#define MAX_PATH_LEN        4096

#define MAX_OPEN_FILES      256

#define O_MODE              0x0000000f // the mode bits
#define O_RDONLY            0x00000000 // (Read-only access)
#define O_WRONLY            0x00000001 // (Write-only access)
#define O_RDWR              0x00000002 // (Read and write access)

#define O_TYPE              0x000000f0 // the open type bits
#define O_APPEND            0x00000010 // (Append mode, writes always go to the end of the file)
#define O_TRUNC             0x00000020 // (Truncate file to zero length if it exists)
#define O_CREAT             0x00000040 // (Create file if it doesn't exist)


typedef struct FSInfo {
    dword lead_signature;
    byte reserved1[480];
    dword signature_2;
    dword num_free_clusters;
    dword next_free_cluster;
    byte reserved2[12];
    dword trail_signature;
}FSInfo;

typedef struct Dir_Ent {
    char filename[11]; // padded with spaces (no null terminaltor)
    //char extention[3]; // padded with spaces (no null terminator) just make part of the filename right now
    byte attributes; // info (ex: dir, read only, etc.)
    byte reserved;
    
    byte creat_tenth_second;
    word creat_time;
    word creat_date;
    
    word last_accessed;

    word First_cluster_high;

    word mod_time;
    word mod_date;
    
    word First_cluster_low;

    dword size;
}Dir_Ent;

typedef struct {
    //char     path[MAX_PATH_LEN]; // optional, for debugging
    dword    start_cluster;      // where file data begins
    dword    cur_cluster;        // current cluster in chain
    dword    file_size;          // in bytes
    dword    pos;                // current offset in file
    byte     mode;               // read/write flags
    byte     used;               // 0 = free, 1 = allocated
} OpenFile;

extern OpenFile open_file_table[MAX_OPEN_FILES];

// formats the drive as FAT32 (probably not well but works for my OS)
byte Format_FAT32(void);

// mounts a FAT32 partition (again probably not well but work for drives formated with my OS)
byte Mount_FAT32(void);

// returns the value of a FAT entry for the given cluster
dword Get_Fat_Entry(dword cluster);

// sets the value of a FAT entry for a given cluster
byte Set_Fat_Entry(dword cluster, dword data);

// returns an allocated cluster (will also link it to the previous one in the chain if prev >= 2)
dword clalloc(dword prev);

// frees an allocated cluster (will also delink all cluster that it points towards recusively) does not set all pointing to it as free
byte freecl(dword Cluster);

// adds a directory entry to a directory
byte add_dir_ent(dword clust, Dir_Ent ent);

// soft deletes an entry with a matching filename
byte del_dir_ent(dword clust, const char* filename);

// returns the cluster that the entry that path points to
dword get_cluster_from_path(const char* path);

// creates a file, use open its better
byte Create_File(char* filename, dword creat_flags, dword cluster, word entry);

/**  opens a file in the open files table
 *   
 * @param path          the path to the file you would like to open (keep the max path lenth in mind)
 * 
 * @param access_flags  how you would like to access the file (ex: RO WO TRUNC APPEND CREAT, ...)
 * 
 * @param creat_flags   not necissary if you arent making a file, can just be 0 if system is equal to 0, but if system is 1 then it will create a device file
 * 
 * @param system        control the privelige always set to 0 through syscalls and could always be 1 when the kernel is running it (has to be 1 to create a device)
*/
int open_file(const char *path, dword access_flags, dword creat_flags, byte system);

int write_file(int fd, const void *buf, int bytes);

int read_file(int fd, void *buf, int bytes);

#endif