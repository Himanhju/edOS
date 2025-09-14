#include <Memory/Opcodes/Opcodes.h>
#include <Memory/Operations/Operations.h>
#include <General/Time/Time.h>
#include <Graphics/VESA/VESA.h>
#include <Drivers/Disk/Disk.h>

#include "vdFS.h"

dword Find_First_Free_Sector(void){
    dword ret = 0;
    byte buf[SECTOR_SIZE];
    for(dword i = DATA_START_BLOCK/*all things before this are used by the OS or are pre-allocated*/; i < (((dword)superblock.info.total_sectors) & 0xFFFFFFFE); i++){

        if(Read_Sector(i, buf) != 0){
            return 1;
        }

        if((buf[SECTOR_SIZE - 1] & SECTOR_ALLOCATED_FLAG) != 0){// if the allocation flag is set
            continue;
        }else{
            ret = i;
            break;
        }
    }
    
    return ret;
}

void vdFS_Init(void){
    superblock.info.magic = 0xDEADBEEF;
    superblock.info.root_dir_start = ROOT_START_BLOCK;
    Superblock_Init(); // initialize the superblock and default info
}

//allocates a block and returns the block allocated (WARNING: do not continusly call this)
dword balloc(void){
    char Alloc_Flag[SECTOR_SIZE];
    memset(Alloc_Flag, 0, SECTOR_SIZE); // set the sector to all zeros

    Alloc_Flag[511] = SECTOR_ALLOCATED_FLAG; // set the allocation flag ONLY

    const dword Sector_Allocated = Find_First_Free_Sector(); // find the first free sector

    Write_Sector(Sector_Allocated, Alloc_Flag); // write the allocation flag to the sector and zero the rest od the sector

    return Sector_Allocated; // returns the sector it allocates
}

//frees an allocated block
void freeb(dword sector){
    char Zeros[SECTOR_SIZE];
    memset(Zeros, 0, SECTOR_SIZE);//set the sector to all zeros

    Write_Sector(sector, Zeros); // overwrite entire sector with 0 including but not limited to the allocation flag
}

void free_file_Metadata(dword lba){

    File_Metadata Metadata;
    dword Sector = lba;

Free_Sector:
    //hope and pray this works
    (void)Read_Sector(Sector, &Metadata); // read sector into memory

    for(byte i = 0; i < (500 - (500 % sizeof(File_Entry))) / sizeof(File_Entry); i++){
        freeb(Hex_To_Dec_Short(Metadata.Entries[i].Sector));
    }

    Sector = Hex_To_Dec_Short(Metadata.Next_Metadata_Block);
    if(Sector != 0){
        goto Free_Sector;
    }
}

void free_Dir_Metadata(dword lba){
    
    Dir_Metadata Metadata;
    dword Sector = lba;

Free_Sector:
    //hope and pray this works
    (void)Read_Sector(Sector, &Metadata);

    for(byte i = 0; i < (504 - (504 % sizeof(Dir_Entry))) / sizeof(Dir_Entry); i++){
        if((Metadata.Entries[i].flags & ENTRY_ALLOCATED_FLAG) != 0){
            break; // inefficient but it will work
        }
        //if it is a valid entry
        switch(Metadata.Entries[i].flags & 0x7F){
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
dword get_Metadata_Sector_From_Parent_Dir_Sector(const dword Parent, const char* Entname){
    byte Found = 0;
    dword Sector = Parent;
    Dir_Metadata Metadata;
    while(!Found){
        Read_Sector(Sector, &Metadata);
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

byte get_flags_From_Parent_Dir_Sector(const dword Parent, const char* Entname){
    byte Found = 0;
    dword Sector = Parent;
    Dir_Metadata Metadata;
    while(!Found){
        Read_Sector(Sector, &Metadata);
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
dword get_Dir_Sector_From_Path_Absolute(const char* Path){
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

    dword cur_sector = ROOT_START_BLOCK;

    Dir_Metadata cur_Metadata;
    
    for(word i = 1; i < num_Path; i++){
        Read_Sector(cur_sector, &cur_Metadata); // read the root sector to check for the first Dir
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
dword get_file_Metadata_Sector_From_Path_Absolute(const char* Path){
    dword Path_len = (dword)strlen(Path);
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

    char Dir_Path[(Path_len - (dword)seperator) + 2];
    strncpy(Path, Dir_Path, (short)((Path_len - (dword)seperator) + 1), 0);
    Dir_Path[(Path_len - (dword)seperator) + 1] = '\0';//null terminate
    dword Dir_Sector = get_Dir_Sector_From_Path_Absolute(Dir_Path); // get the sector

    return get_Metadata_Sector_From_Parent_Dir_Sector(Dir_Sector, filename); 
}

byte add_Dir_Entry(dword Dir_Metadata_Sector, Dir_Entry Entry){
    Dir_Metadata Metadata;
    dword TMP_Sector_Storage;
    dword Sector = Dir_Metadata_Sector;
    byte TMP_Next_Sector[4];

    memset(&Metadata, 0, sizeof(Dir_Metadata));

Check_Sector:
    //add a function to make it work idk which

    Read_Sector(Sector, &Metadata);
    
    for(byte i = 0; i < (504 - (504 % sizeof(Dir_Entry))) / sizeof(Dir_Entry); i++){
        if((Metadata.Entries[i].flags & 128) >> 7 != 1){
            //found a free entry
            Metadata.Entries[i] = Entry; // copy the entry
            break; // break the loop
        }else{
            //not a free entry
            if(memcmp(Entry.Entname, Metadata.Entries[i].Entname, (dword)strlen(Entry.Entname)) == 0){
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
                Write_Sector(Sector, &Metadata);

                Sector = TMP_Sector_Storage; // set the current sector to the allocated sector
                goto Check_Sector; // jump back to the sector checker
            }
        }
    }
    
    //pstr_8x8(Metadata.Entries[0].Entname, Get_RGB(0xffffff));

    return Write_Sector(Sector, &Metadata); // write the Sector with the Metadata
}

byte del_Dir_Entry(dword Dir_Metadata_Sector, char* Entname){
    // deletes and entry given and entry name and remove the next metadata file if its empty and shortens it so theres no empty entries surrounded by filled out ones

    Dir_Metadata Metadata;
    Dir_Entry Empty_Entry;
    dword Sector = Dir_Metadata_Sector;
    memset(&Empty_Entry, 0, sizeof(Dir_Entry));

Check_Sector:
    Read_Sector(Sector, &Metadata);

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
            Write_Sector(Sector, &Metadata); //write the metadata
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
Dir_Entry Get_Dir_Entry(byte flags, char* Entname, dword lba){
    Dir_Entry returned;
    Dec_To_Hex_Short(lba, returned.Metadata_Block);
    returned.flags = flags;

    memcpy(Entname, returned.Entname, strlen(Entname) > MAX_ENTNAME_LENGTH ? MAX_ENTNAME_LENGTH : strlen(Entname));

    return returned;
}
*/

// format: /dir1/dir2/dir3/file
byte add_File_With_Path(char* Path){ // problem doesnt work with files in the root dir
    dword Path_len = (dword)strlen(Path);
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

    char Dir_Path[(Path_len - (dword)seperator) + 2];
    strncpy(Path, Dir_Path, (short)(Path_len - (dword)seperator) + 1, 0);
    Dir_Path[(Path_len - (dword)seperator) + 1] = '\0';//null terminate
    
    dword Dir_Sector = get_Dir_Sector_From_Path_Absolute(Dir_Path); // get the sector


    Dir_Entry Entry;
    Entry.flags = ENTRY_ALLOCATED_FLAG | FILE_FLAG;
    memcpy(filename, Entry.Entname, MAX_ENTNAME_LENGTH);//Get_Dir_Entry(ENTRY_ALLOCATED_FLAG | FILE_FLAG, filename, balloc())
    Dec_To_Hex_Short(balloc(), Entry.Metadata_Block);

    return add_Dir_Entry(Dir_Sector, Entry);
}

//format: /dir1/dir2/dir3/newdir
byte add_Dir_With_Path(char* Path){
    dword Path_len = (dword)strlen(Path);
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
    
    char Dir_Path[(Path_len - (dword)seperator) + 2];
    memset(Dir_Path, 0, (Path_len - (dword)seperator) + 2);

    strncpy(Path, Dir_Path, (short)(Path_len - (dword)seperator) + 1, 0);
    Dir_Path[(Path_len - (dword)seperator) + 1] = '\0';//null terminate

    dword Dir_Sector = get_Dir_Sector_From_Path_Absolute(Dir_Path); // get the sector

    //pint_8x8(Dir_Sector, Get_RGB(0xffffff));

    Dir_Entry Entry;
    Entry.flags = ENTRY_ALLOCATED_FLAG | DIRECTORY_FLAG;
    memcpy(Dirname, Entry.Entname, MAX_ENTNAME_LENGTH);//Get_Dir_Entry(ENTRY_ALLOCATED_FLAG | DIRECTORY_FLAG, Dirname, balloc())
    Dec_To_Hex_Short(balloc(), Entry.Metadata_Block);

    return add_Dir_Entry(Dir_Sector, Entry);
}

//writes to a file's data sectors (0 = first sector, 1 = second sector) only 508 bytes per sector
void write_file_Sector(char* Path, dword Sector, void* buf){
    dword Original_Metadata_Sector = get_file_Metadata_Sector_From_Path_Absolute(Path);
    pstr_8x8("OMS: ", Get_RGB(0xffffff));
    pbin_8x8(Original_Metadata_Sector, Get_RGB(0xffffff));
    newline();
    File_Metadata Metadata;

    dword Num_Metadata_Sectors = (Sector / (500 - (500 % sizeof(File_Entry))) / sizeof(File_Entry)); // the number of metadata sectors to ckeck
    word Internal_Sector = (word)(Sector % (500 - (500 % sizeof(File_Entry))) / sizeof(File_Entry)); // the sector in the metadata sector that were writing to

    dword lba = Original_Metadata_Sector;

    Read_Sector(Original_Metadata_Sector, &Metadata);

    for(dword i = 0; i < Num_Metadata_Sectors; i++){
        lba = Hex_To_Dec_Short(Metadata.Next_Metadata_Block);
        Read_Sector(lba, &Metadata);
    }

    if(Hex_To_Dec_Short(Metadata.Entries[Internal_Sector].Sector) == 0){
        dword allocated = balloc();
        Dec_To_Hex_Short(allocated, Metadata.Entries[Internal_Sector].Sector);
        Write_Sector(lba, &Metadata);
    }

    lba = Hex_To_Dec_Short(Metadata.Entries[Internal_Sector].Sector); // lba now holds the sector to write to

    pstr_8x8("lba: ", Get_RGB(0xffffff));
    pbin_8x8(lba, Get_RGB(0xffffff));
    newline();

    Read_Sector(lba, &Metadata); // read to get the flags

    memcpy(buf, &Metadata, 508); // copy the data in

    Write_Sector(lba, &Metadata); // write to the sector with the flags and data
}
 
//reads a file's data sectors (0 = first sector, 1 = second sector) only 508 bytes per sector
void read_file_Sector(char* Path, dword Sector, void *buf){
    dword Original_Metadata_Sector = get_file_Metadata_Sector_From_Path_Absolute(Path);
    File_Metadata Metadata;

    dword Num_Metadata_Sectors = (Sector / (508 - (508 % sizeof(File_Entry))) / sizeof(File_Entry)); // the number of metadata sectors to ckeck
    word Internal_Sector = (word)(Sector % (508 - (508 % sizeof(File_Entry))) / sizeof(File_Entry)); // the sector in the metadata sector that were reading from

    dword lba = Original_Metadata_Sector;

    Read_Sector(Original_Metadata_Sector, &Metadata); // checked: not the problem
    
    for(dword i = 0; i < Num_Metadata_Sectors; i++){ // checked: not the problem
        lba = Hex_To_Dec_Short(Metadata.Next_Metadata_Block);
        Read_Sector(lba, &Metadata);
    }
    
    lba = Hex_To_Dec_Short(Metadata.Entries[Internal_Sector].Sector); // lba now holds the sector to read 

    Read_Sector(lba, &Metadata); // read to get the data (error hangs and doesnt return)

    memcpy(&Metadata, buf, 508); // copy the data to the buffer
}

// actually reads the data from the files metadata
void Read_File(char* Path, void *buf, dword len){
    dword Sectors = (word)(len / 508);
    word Additional_bytes = (word)(len % 508);
    byte Sector_buf[508] = {0};

    for(dword i = 0; i < Sectors; i++){
        read_file_Sector(Path, i, (void *)(((dword)buf) + (i * 508)));
    }
    
    read_file_Sector(Path, Sectors, Sector_buf);

    memcpy(Sector_buf, (void *)(((dword)buf) + (Sectors * 508)), Additional_bytes);
}

// actaully writes the data from the files metadata
void Write_File(char* Path, void *buf, dword len){
    dword Sectors = (word)(len / 508);
    word Additional_bytes = (word)(len % 508);
    byte Sector_buf[508] = {0};

    for(dword i = 0; i < Sectors; i++){
        write_file_Sector(Path, i, (void *)(((dword)buf) + (i * 508)));
    }
    
    memcpy((void *)(((dword)buf) + (Sectors * 508)), Sector_buf, Additional_bytes);
    
    write_file_Sector(Path, Sectors, Sector_buf);
}