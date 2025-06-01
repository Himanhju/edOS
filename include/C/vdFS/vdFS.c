#include "vdFS.h"

#include "../Types/Common.h"
#include "../Memory/Opcodes/Opcodes.h"
#include "../Memory/Operations/Operations.h"
#include "../General/Time/Time.h"
#include "../General/Info/Info.h"

__asm__("nop\n""nop\n""nop\n""nop\n""nop\n""nop\n");

struct Superblock superblock;

void ata_write_sector(dword lba, void* buffer) {
    // Send drive/head and LBA info
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);                      // Sector count
    outb(0x1F3, (byte)lba);              // LBA low
    outb(0x1F4, (byte)(lba >> 8));       // LBA mid
    outb(0x1F5, (byte)(lba >> 16));      // LBA high
    outb(0x1F7, 0x30);                   // Command: WRITE SECTOR

    // Wait for the drive to be ready
    while (!(inb(0x1F7) & 0x08));
    const word *words = (word *)buffer;
    // Read 256 words (512 bytes)
    for (int i = 0; i < 256; i++) {
        outw(0x1F0, words[i]);
    }
}

void ata_read_sector(dword lba, void* buffer) {
    // Send drive/head and LBA info
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);                      // Sector count
    outb(0x1F3, (byte)lba);              // LBA low
    outb(0x1F4, (byte)(lba >> 8));       // LBA mid
    outb(0x1F5, (byte)(lba >> 16));      // LBA high
    outb(0x1F7, 0x20);                   // Command: READ SECTOR

    // Wait for the drive to be ready
    while (!(inb(0x1F7) & 0x08));
    word data;
    byte* buffer_bytes = (byte*)buffer;
    // Read 256 words (512 bytes)
    for (int i = 0; i < 256; i++) {
        data = inw(0x1F0);
        buffer_bytes[i * 2]     = data & 0xFF;
        buffer_bytes[i * 2 + 1] = (data >> 8);
    }
}

unsigned int Find_First_Free_Sector(void){
    unsigned int ret = 0;
    byte buf[512];
    for(unsigned int i = DATA_START_BLOCK/*all things before this are used by the OS or are pre-allocated*/; i < superblock.total_sectors; i++){
        ata_read_sector(i, buf);
        if((buf[511] & 1) == 1){// if the allocation flag is set
            continue;
        }else{
            ret = i;
            break;
        }
    }
    return ret;
}

inline void Write_Superblock(void){
    ata_write_sector(SUPERBLOCK_BLOCK, &superblock);
}

void FS_Init(void){
    Superblock_Init(); // initialize the superblock 
}

void Superblock_Init(void){
    superblock.block_size = 512;
    superblock.magic = 0xDEADBEEF; // for shits and giggles
    superblock.root_dir_start = ROOT_START_BLOCK; // beginning of filesystem
    superblock.total_sectors = 0x40000000 / 0x200; // 1GB total disk size / sector size
    for(int i = 0; i < 496; i++){
        superblock.padding[i] = '\0';
    }
    Write_Superblock();
}

//allocates a block and returns the block allocated (WARNING: do not continusly call this)
unsigned int balloc(void){
    char Alloc_Flag[512];
    memset(Alloc_Flag, 0, 512); // set the sector to all zeros

    Alloc_Flag[511] = ENTRY_ALLOCATED_FLAG; //set the allocation flag ONLY

    const unsigned int Sector_Allocated = Find_First_Free_Sector(); // find the first free sector

    ata_write_sector(Sector_Allocated, Alloc_Flag); //write the allocation flag to the sector and zero the rest od the sector

    return Sector_Allocated;//returns the sector it allocates
}

//frees an allocated block
void freeb(unsigned int sector){
    char Zeros[512];
    memset(Zeros, 0, 512);//set the sector to all zeros

    ata_write_sector(sector, Zeros); // overwrite entire sector with 0 including but not limited to the allocation flag
}

void free_file_Metadata(unsigned int lba){

    File_Metadata Metadata;
    unsigned int Sector = lba;

Free_Sector:
    ata_read_sector(Sector, &Metadata); // read sector into memory

    for(byte i = 0; i < (500 - (500 % sizeof(File_Entry))) / sizeof(File_Entry); i++){
        freeb(Hex_To_Dec_Short(Metadata.Entries[i].Sector));
    }

    Sector = Hex_To_Dec_Short(Metadata.Next_Metadata_Block);
    if(Sector != 0){
        goto Free_Sector;
    }
}

void free_Dir_Metadata(unsigned int lba){
    
    Dir_Metadata Metadata;
    unsigned int Sector = lba;

Free_Sector:
    ata_read_sector(Sector, &Metadata);

    for(byte i = 0; i < (504 - (504 % sizeof(Dir_Entry))) / sizeof(Dir_Entry); i++){
        if((Metadata.Entries[i].flags & ENTRY_ALLOCATED_FLAG) != 0){
            break; // inefficient but it will work
        }
        //if it is a valid entry
        switch(Metadata.Entries[i].flags & 0b01111111){
            case FILE_FLAG: 
                free_file_Metadata(Hex_To_Dec_Short(Metadata.Entries[i].Metadata_Block)); // free all of the blocks allocated to the file
                break; // if its a file
            case DIRECTORY_FLAG:
                free_Dir_Metadata(Hex_To_Dec_Short(Metadata.Entries[i].Metadata_Block)); // free all of the blocks allocated to the dir
                break; // if its a file
            default: break;
        }
        freeb(Hex_To_Dec_Short(Metadata.Entries[i].Metadata_Block)); // free the actual metadata block
    }

    Sector = Hex_To_Dec_Short(Metadata.Next_Metadata_Block);
    if(Sector != 0){
        goto Free_Sector;
    }
}

// could be implemented in more places then it is but i dont want to change it because im lazy
unsigned int get_Metadata_Sector_From_Parent_Dir_Sector(const unsigned int Parent, const char* Entname){
    byte Found = 0;
    unsigned int Sector = Parent;
    Dir_Metadata Metadata;
    while(!Found){
        ata_read_sector(Sector, &Metadata);
        for(byte e = 0; e < (504 - (504 % sizeof(Dir_Entry))) / sizeof(Dir_Entry); e++){
            if(strncmp(Metadata.Entries[e].Entname, Entname, strlen(Entname) > MAX_ENTNAME_LENGTH ? MAX_ENTNAME_LENGTH : strlen(Entname)) == 0){
                // found the entry
                Found = 1;
                return Hex_To_Dec_Short(Metadata.Entries[e].Metadata_Block);
                break;
            }
            if(e == (504 - (504 % sizeof(Dir_Entry))) / sizeof(Dir_Entry) - 1){
                //not in this sector
                Sector = Hex_To_Dec_Short(Metadata.Next_Metadata_Block);
                if(Sector == 0){
                    // doesnt exist
                    return 0;
                }
                // does have a continuation
                //go to next sector and check that
            }
        }
    }
    return 1; // dont know how it would get here
}

byte get_flags_From_Parent_Dir_Sector(const unsigned int Parent, const char* Entname){
    byte Found = 0;
    unsigned int Sector = Parent;
    Dir_Metadata Metadata;
    while(!Found){
        ata_read_sector(Sector, &Metadata);
        for(byte e = 0; e < (504 - (504 % sizeof(Dir_Entry))) / sizeof(Dir_Entry); e++){
            if(strncmp(Metadata.Entries[e].Entname, Entname, strlen(Entname) > MAX_ENTNAME_LENGTH ? MAX_ENTNAME_LENGTH : strlen(Entname)) == 0){
                // found the entry
                Found = 1;
                return Metadata.Entries[e].flags;
                break;
            }
            if(e == (504 - (504 % sizeof(Dir_Entry))) / sizeof(Dir_Entry) - 1){
                //not in this sector
                Sector = Hex_To_Dec_Short(Metadata.Next_Metadata_Block);
                if(Sector == 0){
                    // doesnt exist
                    return 0;
                }
                // does have a continuation
                //go to next sector and check that
            }
        }
    }
    return 1; // dont know how it would get here
}

//with format /dir1/dir2/dir3
unsigned int get_Dir_Sector_From_Path_Absolute(const char* Path){
    int Path_len = strlen(Path);
    int num_Path = 0;
    for(int c = 1; c < Path_len; c++) {
        if(Path[c] == '/'){
            num_Path++;
        }
    }
    int Path_lens[num_Path];
    int Path_starts[num_Path];
    int cur_Path = 0;
    for(int o = 0; o < num_Path; o++){
        Path_lens[o] = 0;
        Path_starts[o] = 0;
    }
    for(int c = 0; c < Path_len; c++){
        if(Path[c] == '/'){ cur_Path++; Path_starts[cur_Path] = c + 1; continue;}                                             
        Path_lens[cur_Path]++;
    }
    char Dirs[num_Path][MAX_ENTNAME_LENGTH];

    for(int i = 0; i < num_Path; i++){
        for(int o = 0; o < MAX_ENTNAME_LENGTH; o++) {
            Dirs[i][o] = '\0';
        }
        for(int c = Path_starts[i]; c < Path_starts[i] + Path_lens[i]; c++) {
            Dirs[i][c - Path_starts[i]] = Path[c];
        }
    }

    unsigned int cur_sector = ROOT_START_BLOCK;

    Dir_Metadata cur_Metadata;

    for(word i = 0; i < num_Path; i++){
        ata_read_sector(cur_sector, &cur_Metadata); // read the root sector to check for the first Dir
        for(byte e = 0; e < (504 - (504 % sizeof(Dir_Entry))) / sizeof(Dir_Entry); e++){
            if(strncmp(cur_Metadata.Entries[e].Entname, Dirs[i], strlen(Dirs[i])) == 0){
                if((cur_Metadata.Entries[e].flags & DIRECTORY_FLAG) == 0){
                    return 2; //entry was not a directory
                }
                cur_sector = Hex_To_Dec_Short(cur_Metadata.Entries[e].Metadata_Block);// set the sector
                break; // go to next directory
            }else{
                //if(Hex_To_Dec_Short())
                return 1; // directory not found
            }
        }

    }
    return cur_sector;
}

//with format /dir1/dir2/file
unsigned int get_file_Metadata_Sector_From_Path_Absolute(const char* Path){
    unsigned int Path_len = strlen(Path);
    unsigned int seperator = -1;
    for(byte i = 0; i < MAX_ENTNAME_LENGTH + 1; i++){
        if(Path[Path_len - i] == '/'){
            seperator = Path_len - i;
            break;
        }
    }
    if(seperator == -1){
        return 3;
    }
    char filename[MAX_ENTNAME_LENGTH];
    strncpy(&Path[seperator + 1], filename, strlen(&Path[seperator + 1]) > MAX_ENTNAME_LENGTH ? MAX_ENTNAME_LENGTH : strlen(&Path[seperator + 1]), 0);

    char Dir_Path[Path_len - (Path_len - seperator) + 1];
    strncpy(Path, Dir_Path, Path_len - (Path_len - seperator), 0);
    Dir_Path[Path_len - (Path_len - seperator)] = '\0';//null terminate
    unsigned int Dir_Sector = get_Dir_Sector_From_Path_Absolute(Dir_Path); // get the sector

    return get_Metadata_Sector_From_Parent_Dir_Sector(Dir_Sector, filename); 
}

byte add_Dir_Entry(unsigned int Dir_Metadata_Sector, Dir_Entry Entry){
    Dir_Metadata Metadata;
    unsigned int TMP_Sector_Storage;
    unsigned int Sector = Dir_Metadata_Sector;
    byte TMP_Next_Sector[4];

Check_Sector:
    ata_read_sector(Dir_Metadata_Sector, &Metadata);

    for(byte i = 0; i < (504 - (504 % sizeof(Dir_Entry))) / sizeof(Dir_Entry); i++){
        if((Metadata.Entries[i].flags & 128) >> 7 != 1){
            //found a free entry
            Metadata.Entries[i] = Entry; // copy the entry
            break; // break the loop
        }else{
            //not a free entry
            if(memcmp(Entry.Entname, Metadata.Entries[i].Entname, strlen(Entry.Entname)) == 0){
                //if the current entry shares the same name 
                return 2;
            }
        }

        if(i == ((504 - (504 % sizeof(Dir_Entry))) / sizeof(Dir_Entry)) - 1){// if its the last one and no others have been found
            if(Hex_To_Dec_Short(Metadata.Next_Metadata_Block) != 0){
                Sector = Hex_To_Dec_Short(Metadata.Next_Metadata_Block);
                goto Check_Sector; // jump back to the sector checker
            }else{
                // there isnt another sector
                TMP_Sector_Storage = balloc(); // allocate a new sector
                if(TMP_Sector_Storage == 0){
                    //no more sectors could be allocated
                    return 1;
                }

                Dec_To_Hex_Short(TMP_Sector_Storage, TMP_Next_Sector); 
                memcpy(TMP_Next_Sector, Metadata.Next_Metadata_Block, 4); // write the number of the next sector in the current metadata sector
                ata_write_sector(Sector, &Metadata);

                Sector = TMP_Sector_Storage; // set the current sector to the allocated sector
                goto Check_Sector; // jump back to the sector checker
            }
        }
    }

    ata_write_sector(Sector, &Metadata); // write the Sector with the Metadata
    return 0;
}

byte del_Dir_Entry(unsigned int Dir_Metadata_Sector, char* Entname){
    // deletes and entry given and entry name and remove the next metadata file if its empty and shortens it so theres no empty entries surrounded by filled out ones

    Dir_Metadata Metadata;
    Dir_Entry Empty_Entry;
    unsigned int Sector = Dir_Metadata_Sector;
    memset(&Empty_Entry, 0, sizeof(Dir_Entry));

Check_Sector:
    ata_read_sector(Sector, &Metadata);

    for(byte i = 0; i < (504 - (504 % sizeof(Dir_Entry))) / sizeof(Dir_Entry); i++){
        if(strncmp(Metadata.Entries[i].Entname, Entname, strlen(Entname)) == 0){
            //if their the same name
            if((Metadata.Entries[i].flags & DIRECTORY_FLAG) != 0){
                //if its a directory
                free_Dir_Metadata(Hex_To_Dec_Short(Metadata.Entries[i].Metadata_Block)); // add function
            }else if ((Metadata.Entries[i].flags & FILE_FLAG) != 0){
                //if its a file
                free_file_Metadata(Hex_To_Dec_Short(Metadata.Entries[i].Metadata_Block)); // add function
            }
            
            freeb(Hex_To_Dec_Short(Metadata.Entries[i].Metadata_Block));
            Metadata.Entries[i] = Empty_Entry;
            ata_write_sector(Sector, &Metadata); //write the metadata
            //Metadata_Shorten(Sector); // shorten it to stop fragmentation (soon to be implemented)
            break;
        }

        if(i == (504 - (504 % sizeof(Dir_Entry))) / sizeof(Dir_Entry) - 1){
            // if its the last one and nothing was found

            if(Hex_To_Dec_Short(Metadata.Next_Metadata_Block) != 0){
                Sector = Hex_To_Dec_Short(Metadata.Next_Metadata_Block);
                goto Check_Sector; // jump back to the sector checker
            }else{
                return 1; //return an error because it didnt find the entry to delete
            }
        }
    }
    return 0;
}

Dir_Entry Get_Dir_Entry(byte flags, char* Entname, unsigned int lba){
    Dir_Entry returned;
    byte Sector[4];
    Dec_To_Hex_Short(lba, Sector);
    memcpy(Sector, returned.Metadata_Block, 4);
    returned.flags = flags;
    memcpy(Entname, returned.Entname, strlen(returned.Entname) > MAX_ENTNAME_LENGTH ? MAX_ENTNAME_LENGTH : strlen(returned.Entname));
    return returned;
}

// format: /dir1/dir2/dir3/file
byte add_File_With_Path(char* Path){
    unsigned int Path_len = strlen(Path);
    unsigned int seperator = -1;
    for(byte i = 0; i < MAX_ENTNAME_LENGTH + 1; i++){
        if(Path[Path_len - i] == '/'){
            seperator = Path_len - i;
            break;
        }
    }
    if(seperator == -1){
        return 3;
    }
    char filename[MAX_ENTNAME_LENGTH];
    strncpy(&Path[seperator + 1], filename, strlen(&Path[seperator + 1]) > MAX_ENTNAME_LENGTH ? MAX_ENTNAME_LENGTH : strlen(&Path[seperator + 1]), 0);

    char Dir_Path[Path_len - (Path_len - seperator) + 1];
    strncpy(Path, Dir_Path, Path_len - (Path_len - seperator), 0);
    Dir_Path[Path_len - (Path_len - seperator)] = '\0';//null terminate
    unsigned int Dir_Sector = get_Dir_Sector_From_Path_Absolute(Dir_Path); // get the sector
    return add_Dir_Entry(Dir_Sector, Get_Dir_Entry(ENTRY_ALLOCATED_FLAG | FILE_FLAG, filename, balloc()));
}

//format: /dir1/dir2/dir3/newdir
byte add_Dir_With_Path(char* Path){
    int Path_len = strlen(Path);
    int seperator = -1;
    for(byte i = 0; i < MAX_ENTNAME_LENGTH + 1; i++){
        if(Path[Path_len - i] == '/'){
            seperator = Path_len - i;
            break;
        }
    }
    if(seperator == -1){
        return 3;
    }
    char Dirname[MAX_ENTNAME_LENGTH];
    strncpy(&Path[seperator + 1], Dirname, strlen(&Path[seperator + 1]) > MAX_ENTNAME_LENGTH ? MAX_ENTNAME_LENGTH : strlen(&Path[seperator + 1]), 0);

    char Dir_Path[Path_len - (Path_len - seperator) + 1];
    strncpy(Path, Dir_Path, Path_len - (Path_len - seperator), 0);
    Dir_Path[Path_len - (Path_len - seperator)] = '\0';//null terminate
    unsigned int Dir_Sector = get_Dir_Sector_From_Path_Absolute(Dir_Path); // get the sector

    return add_Dir_Entry(Dir_Sector, Get_Dir_Entry(ENTRY_ALLOCATED_FLAG | DIRECTORY_FLAG, Dirname, balloc()));
}

// writes bytes bytes of buf into the file at an offset stored in path (with format: /dir1/dir2/file)
void write(char* path, char* buf, unsigned int bytes, unsigned long long Offset){
// ========== define main variables ==========
    unsigned int Metadata_Sector = get_file_Metadata_Sector_From_Path_Absolute(path); // get the metadata sector
    File_Metadata Metadata;
// ======================================

    ata_read_sector(Metadata_Sector, &Metadata); // read in the main metadata sector
    

}

// reads up to bytes bytes from the file at an offset into buf
unsigned int read(char* path, char* buf, unsigned int bytes, unsigned long long Offset){
    return 1;
}
 


__asm__("nop\n""nop\n""nop\n""nop\n""nop\n""nop\n");