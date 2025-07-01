#include "vdFS.h"

#include "../Types/Common.h"
#include "../Memory/Opcodes/Opcodes.h"
#include "../Memory/Operations/Operations.h"
#include "../General/Time/Time.h"

#include "../Graphics/VESA.h"

struct Superblock superblock;

void ata_print_err(byte error){
    if((error & 0b1) != 0){
        pstr_8x8("Address Mark Not Found\n", Get_RGB(0xffffff));
    }
    if((error & 0b10) != 0){
        pstr_8x8("Track 0 Not Found\n", Get_RGB(0xffffff));
    }
    if((error & 0b100) != 0){
        pstr_8x8("Command Aborted\n", Get_RGB(0xffffff));
    }
    if((error & 0b1000) != 0){
        pstr_8x8("Media Change Request\n", Get_RGB(0xffffff));
    }
    if((error & 0b10000) != 0){
        pstr_8x8("ID Not Found\n", Get_RGB(0xffffff));
    }
    if((error & 0b100000) != 0){
        pstr_8x8("Media Changed\n", Get_RGB(0xffffff));
    }
    if((error & 0b1000000) != 0){
        pstr_8x8("Uncorrectable data Error\n", Get_RGB(0xffffff));
    }
    if((error & 0b10000000) != 0){
        pstr_8x8("Bad Block Detected\n", Get_RGB(0xffffff));
    }else if(error == 0){
        pstr_8x8("No err\n", Get_RGB(0xffffff));
    }
}

byte ata_write_sector(dword lba, void* buffer) {
    // Wait for BSY to clear
    while(inb(0x1F7) & 0x80);

    //wait for it to be read for a command
    while(!(inb(0x1F7) & 0x40));

    // Send drive/head LBA info, and command to write
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);                      // Sector count
    outb(0x1F3, (byte)lba);              // LBA low
    outb(0x1F4, (byte)(lba >> 8));       // LBA mid
    outb(0x1F5, (byte)(lba >> 16));      // LBA high
    outb(0x1F7, 0x30);                   // Command: WRITE SECTOR

    // Check for ERR
    byte status = inb(0x1F7);

    if (status & 0x01) {
        byte error = inb(0x1F1);
        // error now contains the error code bits
        ata_print_err(error);
        return error;
        
    }
    // Wait for the drive to be ready for data
    while (!(inb(0x1F7) & 0x08));

    const word *words = (word *)buffer;
    // Read 256 words (512 bytes)
    for (int i = 0; i < 256; i++) {
        outw(0x1F0, words[i]);
    }
    return 0;
}

byte ata_bulk_write_sectors(dword Start_lba, void* buffer, byte Sectors) {
    // Wait for BSY to clear
    while(inb(0x1F7) & 0x80);

    //wait for it to be read for a command
    while(!(inb(0x1F7) & 0x40));

    // Send drive/head LBA info, and command to write
    outb(0x1F6, 0xE0 | ((Start_lba >> 24) & 0x0F));
    outb(0x1F2, (byte)(Sectors & 0b01111111)); // Sector count
    outb(0x1F3, (byte)Start_lba);              // LBA low
    outb(0x1F4, (byte)(Start_lba >> 8));       // LBA mid
    outb(0x1F5, (byte)(Start_lba >> 16));      // LBA high
    outb(0x1F7, 0x30);                         // Command: WRITE SECTOR

    // Check for ERR
    byte status = inb(0x1F7);

    if (status & 0x01) {
        byte error = inb(0x1F1);
        // error now contains the error code bits
        ata_print_err(error);
        return error;
        
    }
    // Wait for the drive to be ready for data
    while (!(inb(0x1F7) & 0x08));

    const word *words = (word *)buffer;
    // Read 256 words (512 bytes)
    for (int i = 0; i < 256 * (Sectors & 0b01111111); i++) {
        outw(0x1F0, words[i]);
    }
    return 0;
}

byte ata_read_sector(dword lba, void* buffer) {
    // Wait for BSY to clear
    while (inb(0x1F7) & 0x80);

    //wait for it to be read for a command
    while(!(inb(0x1F7) & 0x40));

    // Send drive/head, LBA info then tell it to execute the read 
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);                      // Sector count
    outb(0x1F3, (byte)lba);              // LBA low
    outb(0x1F4, (byte)(lba >> 8));       // LBA mid
    outb(0x1F5, (byte)(lba >> 16));      // LBA high
    outb(0x1F7, 0x20);                   // Command: READ SECTOR
    
    // Check for ERR
    byte status = inb(0x1F7);
    
    if (status & 0x01) {
        byte error = inb(0x1F1);
        // error now contains the error code bits
        ata_print_err(error);
        return error;
        
    }

    // Wait for the drive to be ready for the data
    while (!(inb(0x1F7) & 0x08));

    word data;
    byte* buffer_bytes = (byte*)buffer;

    // Read 256 words (512 bytes)
    for (int i = 0; i < 256; i++) {
        data = inw(0x1F0);
        buffer_bytes[i * 2]     = (byte)(data & 0xFF);
        buffer_bytes[i * 2 + 1] = (byte)((data >> 8) & 0xFF);
    }
    return 0;
}

byte ata_bulk_read_sectors(dword Start_lba, void* buffer, byte Sectors) {
    // Wait for BSY to clear
    while (inb(0x1F7) & 0x80);

    //wait for it to be read for a command
    while(!(inb(0x1F7) & 0x40));

    // Send drive/head, LBA info then tell it to execute the read 
    outb(0x1F6, 0xE0 | ((Start_lba >> 24) & 0x0F));
    outb(0x1F2, (byte)(Sectors & 0b01111111));  // Sector count
    outb(0x1F3, (byte)Start_lba);               // LBA low
    outb(0x1F4, (byte)(Start_lba >> 8));        // LBA mid
    outb(0x1F5, (byte)(Start_lba >> 16));       // LBA high
    outb(0x1F7, 0x20);                          // Command: READ SECTOR
    
    // Check for ERR
    byte status = inb(0x1F7);
    
    if (status & 0x01) {
        byte error = inb(0x1F1);
        // error now contains the error code bits
        ata_print_err(error);
        return error;
        
    }

    // Wait for the drive to be ready for the data
    while (!(inb(0x1F7) & 0x08));

    word data;
    byte* buffer_bytes = (byte*)buffer;

    // Read 256 words (512 bytes)
    for (int i = 0; i < 256 * (Sectors & 0b01111111); i++) {
        data = inw(0x1F0);
        buffer_bytes[i * 2]     = (byte)(data & 0xFF);
        buffer_bytes[i * 2 + 1] = (byte)((data >> 8) & 0xFF);
    }
    return 0;
}

unsigned int Find_First_Free_Sector(void){
    unsigned int ret = 0;
    byte buf[512];
    for(unsigned int i = DATA_START_BLOCK/*all things before this are used by the OS or are pre-allocated*/; i < superblock.total_sectors; i++){

        if(ata_read_sector(i, buf) != 0){
            return 0;
        }

        if((buf[511] & SECTOR_ALLOCATED_FLAG) != 0){// if the allocation flag is set
            continue;
        }else{
            ret = i;
            break;
        }
    }
    return ret;
}

void FS_Init(void){
    Superblock_Init(); // initialize the superblock and default info
}

byte Read_Superblock(void){
    return ata_read_sector(SUPERBLOCK_BLOCK, &superblock);
}

void Superblock_Init(void){
    superblock.block_size = 512;
    superblock.magic = 0xDEADBEEF; // for shits and giggles
    superblock.root_dir_start = ROOT_START_BLOCK; // beginning of filesystem
    superblock.total_sectors = 0x40000000 / 0x200; // 1GB total disk size / sector size
    superblock.is_first_boot = 1; //
    for(int i = 0; i < 492; i++){
        superblock.padding[i] = '\0';
    }
    ata_write_sector(SUPERBLOCK_BLOCK, &superblock);
}

//allocates a block and returns the block allocated (WARNING: do not continusly call this)
unsigned int balloc(void){
    char Alloc_Flag[512];
    memset(Alloc_Flag, 0, 512); // set the sector to all zeros

    Alloc_Flag[511] = SECTOR_ALLOCATED_FLAG; // set the allocation flag ONLY

    const unsigned int Sector_Allocated = Find_First_Free_Sector(); // find the first free sector

    ata_write_sector(Sector_Allocated, Alloc_Flag); // write the allocation flag to the sector and zero the rest od the sector

    return Sector_Allocated; // returns the sector it allocates
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
    //hope and pray this works
    (void)ata_read_sector(Sector, &Metadata); // read sector into memory

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
    //hope and pray this works
    (void)ata_read_sector(Sector, &Metadata);

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
    if(Path_len > MAX_PATH_SIZE){
        return 4;
    }
    int num_Path = 1;
    for(int c = 0; c < Path_len; c++) {
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
    
    for(word i = 1; i < num_Path; i++){
        ata_read_sector(cur_sector, &cur_Metadata); // read the root sector to check for the first Dir
        for(byte e = 0; e < (504 - (504 % sizeof(Dir_Entry))) / sizeof(Dir_Entry); e++){
            if(strncmp(cur_Metadata.Entries[e].Entname, Dirs[i], strlen(cur_Metadata.Entries[e].Entname)) == 0){
                if((cur_Metadata.Entries[e].flags & DIRECTORY_FLAG) == 0){
                    return 2; //entry was not a directory
                }
                cur_sector = Hex_To_Dec_Short(cur_Metadata.Entries[e].Metadata_Block);// set the sector
                break; // go to next directory
            }else{
                //if(Hex_To_Dec_Short())
                //return 1; // directory not found 
                //TODO: add extra things for more sectors
            }
        }

    }

    return cur_sector;
}

//with format /dir1/dir2/file
unsigned int get_file_Metadata_Sector_From_Path_Absolute(const char* Path){
    unsigned int Path_len = (unsigned int)strlen(Path);
    if(Path_len > MAX_PATH_SIZE){
        return 4;
    }
    int seperator = -1;
    for(byte i = 0; i < MAX_ENTNAME_LENGTH + 1; i++){
        if(Path[Path_len - i] == '/'){
            seperator = (int)(Path_len - i);
            break;
        }
    }
    if(seperator == -1){
        return 3;
    }
    char filename[MAX_ENTNAME_LENGTH];
    strncpy(&Path[seperator + 1], filename, strlen(&Path[seperator + 1]) > MAX_ENTNAME_LENGTH ? MAX_ENTNAME_LENGTH : strlen(&Path[seperator + 1]), 0);

    char Dir_Path[(Path_len - (unsigned int)seperator) + 2];
    strncpy(Path, Dir_Path, (short)((Path_len - (unsigned int)seperator) + 1), 0);
    Dir_Path[(Path_len - (unsigned int)seperator) + 1] = '\0';//null terminate
    unsigned int Dir_Sector = get_Dir_Sector_From_Path_Absolute(Dir_Path); // get the sector

    return get_Metadata_Sector_From_Parent_Dir_Sector(Dir_Sector, filename); 
}

byte add_Dir_Entry(unsigned int Dir_Metadata_Sector, Dir_Entry Entry){
    Dir_Metadata Metadata;
    unsigned int TMP_Sector_Storage;
    volatile unsigned int Sector = Dir_Metadata_Sector;
    byte TMP_Next_Sector[4];

    memset(&Metadata, 0, sizeof(Dir_Metadata));

Check_Sector:
    //add a function to make it work idk which

    ata_read_sector(Sector, &Metadata);
    
    for(byte i = 0; i < (504 - (504 % sizeof(Dir_Entry))) / sizeof(Dir_Entry); i++){
        if((Metadata.Entries[i].flags & 128) >> 7 != 1){
            //found a free entry
            Metadata.Entries[i] = Entry; // copy the entry
            break; // break the loop
        }else{
            //not a free entry
            if(memcmp(Entry.Entname, Metadata.Entries[i].Entname, (unsigned int)strlen(Entry.Entname)) == 0){
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
    
    //pstr_8x8(Metadata.Entries[0].Entname, Get_RGB(0xffffff));

    return ata_write_sector(Sector, &Metadata); // write the Sector with the Metadata
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

/*
Dir_Entry Get_Dir_Entry(byte flags, char* Entname, unsigned int lba){
    Dir_Entry returned;
    Dec_To_Hex_Short(lba, returned.Metadata_Block);
    returned.flags = flags;

    memcpy(Entname, returned.Entname, strlen(Entname) > MAX_ENTNAME_LENGTH ? MAX_ENTNAME_LENGTH : strlen(Entname));

    return returned;
}
*/

// format: /dir1/dir2/dir3/file
byte add_File_With_Path(char* Path){ // problem doesnt work with files in the root dir
    unsigned int Path_len = (unsigned int)strlen(Path);
    if(Path_len > MAX_PATH_SIZE){
        return 4;
    }
    int seperator = -1;
    char filename[MAX_ENTNAME_LENGTH];

    for(byte i = 0; i < MAX_ENTNAME_LENGTH + 1; i++){
        if(Path[Path_len - i] == '/'){
            seperator = (int)Path_len - i;
            strncpy(&Path[seperator + 1], filename, strlen(&Path[seperator + 1]) > MAX_ENTNAME_LENGTH ? MAX_ENTNAME_LENGTH : strlen(&Path[seperator + 1]), 0);
            if(seperator == 0){
                //add to root dir

                Dir_Entry Entry;
                Entry.flags = ENTRY_ALLOCATED_FLAG | FILE_FLAG;
                memcpy(filename, Entry.Entname, MAX_ENTNAME_LENGTH);//Get_Dir_Entry(ENTRY_ALLOCATED_FLAG | FILE_FLAG, filename, balloc())
                Dec_To_Hex_Short(balloc(), Entry.Metadata_Block);

                return add_Dir_Entry(ROOT_START_BLOCK, Entry);
            }
            break;
        }
    }
    if(seperator == -1){
        return 3;
    }

    char Dir_Path[(Path_len - (unsigned int)seperator) + 2];
    strncpy(Path, Dir_Path, (short)(Path_len - (unsigned int)seperator) + 1, 0);
    Dir_Path[(Path_len - (unsigned int)seperator) + 1] = '\0';//null terminate
    
    unsigned int Dir_Sector = get_Dir_Sector_From_Path_Absolute(Dir_Path); // get the sector


    Dir_Entry Entry;
    Entry.flags = ENTRY_ALLOCATED_FLAG | FILE_FLAG;
    memcpy(filename, Entry.Entname, MAX_ENTNAME_LENGTH);//Get_Dir_Entry(ENTRY_ALLOCATED_FLAG | FILE_FLAG, filename, balloc())
    Dec_To_Hex_Short(balloc(), Entry.Metadata_Block);

    return add_Dir_Entry(Dir_Sector, Entry);
}

//format: /dir1/dir2/dir3/newdir
byte add_Dir_With_Path(char* Path){
    unsigned int Path_len = (unsigned int)strlen(Path);
    if(Path_len > MAX_PATH_SIZE){
        return 4;
    }
    int seperator = -1;
    char Dirname[MAX_ENTNAME_LENGTH];
    memset(Dirname, 0, MAX_ENTNAME_LENGTH);

    for(byte i = 0; i < MAX_ENTNAME_LENGTH + 1; i++){
        if(Path[Path_len - i] == '/'){
            seperator = (int)Path_len - i;
            strncpy(&Path[seperator + 1], Dirname, strlen(&Path[seperator + 1]) > MAX_ENTNAME_LENGTH ? MAX_ENTNAME_LENGTH : strlen(&Path[seperator + 1]), 0);
            if(seperator == 0){
                //add to root dir

                Dir_Entry Entry;
                Entry.flags = ENTRY_ALLOCATED_FLAG | DIRECTORY_FLAG;
                memcpy(Dirname, Entry.Entname, MAX_ENTNAME_LENGTH);//Get_Dir_Entry(ENTRY_ALLOCATED_FLAG | DIRECTORY_FLAG, Dirname, balloc())
                Dec_To_Hex_Short(balloc(), Entry.Metadata_Block);

                return add_Dir_Entry(ROOT_START_BLOCK, Entry);
            }
            break;
        }
    }
    if(seperator == -1){
        return 3;
    }
    
    char Dir_Path[(Path_len - (unsigned int)seperator) + 2];
    memset(Dir_Path, 0, (Path_len - (unsigned int)seperator) + 2);

    strncpy(Path, Dir_Path, (short)(Path_len - (unsigned int)seperator) + 1, 0);
    Dir_Path[(Path_len - (unsigned int)seperator) + 1] = '\0';//null terminate

    unsigned int Dir_Sector = get_Dir_Sector_From_Path_Absolute(Dir_Path); // get the sector

    //pint_8x8(Dir_Sector, Get_RGB(0xffffff));

    Dir_Entry Entry;
    Entry.flags = ENTRY_ALLOCATED_FLAG | DIRECTORY_FLAG;
    memcpy(Dirname, Entry.Entname, MAX_ENTNAME_LENGTH);//Get_Dir_Entry(ENTRY_ALLOCATED_FLAG | DIRECTORY_FLAG, Dirname, balloc())
    Dec_To_Hex_Short(balloc(), Entry.Metadata_Block);

    return add_Dir_Entry(Dir_Sector, Entry);
}

//writes to a file's data sectors (0 = first sector, 1 = second sector) only 508 bytes per sector
void write_file_Sector(char* Path, unsigned int Sector, void* buf){
    unsigned int Original_Metadata_Sector = get_file_Metadata_Sector_From_Path_Absolute(Path);
    File_Metadata Metadata;

    unsigned int Num_Metadata_Sectors = (Sector / (500 - (500 % sizeof(File_Entry))) / sizeof(File_Entry)); // the number of metadata sectors to ckeck
    unsigned short Internal_Sector = (unsigned short)(Sector % (500 - (500 % sizeof(File_Entry))) / sizeof(File_Entry)); // the sector in the metadata sector that were writing to

    unsigned int lba = Original_Metadata_Sector;

    ata_read_sector(Original_Metadata_Sector, &Metadata);

    for(unsigned int i = 0; i < Num_Metadata_Sectors; i++){
        lba = Hex_To_Dec_Short(Metadata.Next_Metadata_Block);
        ata_read_sector(lba, &Metadata);
    }

    if(Hex_To_Dec_Short(Metadata.Entries[Internal_Sector].Sector) == 0){
        unsigned int allocated = balloc();
        Dec_To_Hex_Short(allocated, Metadata.Entries[Internal_Sector].Sector);
        ata_write_sector(lba, &Metadata);
    }

    lba = Hex_To_Dec_Short(Metadata.Entries[Internal_Sector].Sector); // lba now holds the sector to write to

    ata_read_sector(lba, &Metadata); // read to get the flags

    memcpy(buf, &Metadata, 508); // copy the data in

    ata_write_sector(lba, &Metadata); // write to the sector with the flags and data
}
 
//reads a file's data sectors (0 = first sector, 1 = second sector) only 508 bytes per sector
void read_file_Sector(char* Path, unsigned int Sector, void *buf){
    unsigned int Original_Metadata_Sector = get_file_Metadata_Sector_From_Path_Absolute(Path);
    File_Metadata Metadata;

    unsigned int Num_Metadata_Sectors = (Sector / (500 - (500 % sizeof(File_Entry))) / sizeof(File_Entry)); // the number of metadata sectors to ckeck
    unsigned short Internal_Sector = (unsigned short)(Sector % (500 - (500 % sizeof(File_Entry))) / sizeof(File_Entry)); // the sector in the metadata sector that were reading from

    unsigned int lba = Original_Metadata_Sector;

    ata_read_sector(Original_Metadata_Sector, &Metadata); // checked: not the problem
    
    for(unsigned int i = 0; i < Num_Metadata_Sectors; i++){ // checked: not the problem
        lba = Hex_To_Dec_Short(Metadata.Next_Metadata_Block);
        ata_read_sector(lba, &Metadata);
    }
    
    lba = Hex_To_Dec_Short(Metadata.Entries[Internal_Sector].Sector); // lba now holds the sector to read 

    ata_read_sector(lba, &Metadata); // read to get the data (error hangs and doesnt return)

    memcpy(&Metadata, buf, 508); // copy the data to the buffer
}
