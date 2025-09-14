#include <Memory/Heap/Dalloc/Dalloc.h>
#include <Memory/Operations/Operations.h>
#include <Drivers/ROM/ROM.h>
#include <Drivers/Disk/Disk.h>
#include <Graphics/VESA/VESA.h>
#include <General/Time/Time.h>

#include "FAT32.h"

FSInfo *FSinfo = (void *)FSINFO_BASE;
Boot_Sector *boot_sector = (void *)BOOTLOADER_BASE;

dword FAT_Start;
dword Data_Start_sector = FIRST_FREE_SECTOR;
dword Total_Clusters;

OpenFile open_file_table[MAX_OPEN_FILES] = {0};

byte Format_FAT32(void){
    pstr_8x8("\nFormatting drive as FAT32\n", Get_RGB(0xffffff));

    // ---- Choose geometry ----
    const uint16_t byps = SECTOR_SIZE;               // 512
    const uint8_t  spc  = SECT_PER_CLUST;            // 8 sectors/cluster -> 4 KiB clusters
    const uint16_t rsvd = FIRST_FREE_SECTOR;         // reserved sectors (>=32 typical)
    const uint8_t  nfats= NUM_FATS;                  // 2 FATs
    const uint8_t  media= 0xF8;                      // fixed disk
    const uint32_t totsec = NUM_SECTS;               // EXAMPLE: 2,097,152 sectors (~1 GiB). Replace with real size.

    // ---- Fill BPB (Volume Boot Record) ----
    //OEM
    strncpy("EDOS    ", boot_sector->BP_Block.OEM, 8, 0);

    boot_sector->BP_Block.bytes_per_sector = byps;
    boot_sector->BP_Block.sectors_per_cluster = spc;
    boot_sector->BP_Block.reserved_sectors = rsvd;
    boot_sector->BP_Block.FAT_count = nfats;
    boot_sector->BP_Block.dir_entries = 0;           // 0 for FAT32
    boot_sector->BP_Block.total_sectors = 0;         // use 32-bit field
    boot_sector->BP_Block.media_descriptor_type = media;
    boot_sector->BP_Block.count_sectors_per_track = 63;
    boot_sector->BP_Block.count_heads_or_sides_on_media = 255;
    boot_sector->BP_Block.count_hidden_sectors = 0;
    boot_sector->BP_Block.large_sectors_on_media = totsec;

    // FAT32 extensions (compute FATSz32 iteratively so TotClus is consistent)
    boot_sector->BP_Block.cluster_number_root_dir = 2;         // root at cluster 2
    boot_sector->BP_Block.sector_number_FSInfo = FS_INFO_SECTOR; // usually 1
    boot_sector->BP_Block.sector_number_backup_boot_sector = BK_BOOT_SECTOR; // usually 6
    boot_sector->BP_Block.flags = 0;                           // mirroring enabled
    boot_sector->BP_Block.FAT_version = 0x0000;

    // Compute FAT size: depends on #clusters which depends on FAT size -> iterate to convergence
    uint32_t fatsz32 = 0;
    for(;;){
        uint32_t data_sectors = totsec - (rsvd + nfats * fatsz32);
        uint32_t totclus = data_sectors / spc;                 // clusters in data area
        uint32_t needed_fat_bytes = (totclus + 2) * 4;         // +2 for reserved cluster entries
        uint32_t new_fatsz32 = (needed_fat_bytes + byps - 1) / byps;
        if (new_fatsz32 == fatsz32){ break; }
        fatsz32 = new_fatsz32;
        if (!fatsz32) fatsz32 = 1;
    }
    boot_sector->BP_Block.count_sectors_per_FAT32 = fatsz32;

    // Remaining BPB ext fields
    boot_sector->BP_Block.windows_flags = 0;
    boot_sector->BP_Block.signature = 0x29;                    // 0x29 recommended
    boot_sector->BP_Block.volume_id = 0x124578ab;              // any serial
    strncpy("EDOS IMG    ", boot_sector->BP_Block.volume_label, 11, 0);
    strncpy("FAT32   ", boot_sector->BP_Block.system_id, 8, 0);

    // 0x55AA signature at end of the VBR sector (little-endian 55 AA)
    boot_sector->Boot_Signature = 0xAA55;

    // ---- Write primary & backup VBR ----
    if(Write_Sector(BOOT_SECTOR, boot_sector) != 0){
        return 1;
    }

    if(Write_Sector(BK_BOOT_SECTOR, boot_sector) != 0){
        return 2;
    }

    // ---- Build FSInfo ----
    memset(FSinfo, 0, sizeof(*FSinfo));
    FSinfo->lead_signature  = 0x41615252;   // 'RRaA'
    FSinfo->signature_2     = 0x61417272;   // 'rrAa'
    // We'll fill free/next below after we know TotClus
    FSinfo->trail_signature = 0xAA550000;   // 55 AA at end

    // Compute cluster counts now that fatsz32 is stable
    const uint32_t data_sectors = totsec - (rsvd + nfats * fatsz32);
    const uint32_t totclus = data_sectors / spc;

    // Root dir allocates cluster #2 immediately; free clusters = totclus-1 (root used)
    FSinfo->num_free_clusters = (totclus > 1) ? (totclus - 1) : 0xFFFFFFFF;
    FSinfo->next_free_cluster = 3; // first free hint after root (cluster 2)

    // Primary and backup FSInfo (backup is immediately after backup VBR for convenience)
    if(Write_Sector(FS_INFO_SECTOR, FSinfo) != 0){
        return 3;
    }

    // ---- Initialize both FATs ----
    // FAT[0] entry 0: media + EOC in high nibble
    // FAT[0] entry 1: EOC
    // FAT[0] entry 2: EOC (root cluster allocated)
    // All others: 0 (free)
    const uint32_t fat0_lba = rsvd;               // primary FAT start (relative to VBR LBA)
    const uint32_t fat_sectors = fatsz32;
    uint8_t sec[SECTOR_SIZE];
    memset(sec, 0x00, sizeof(sec));
    // First sector gets the first few entries
    // Each FAT32 entry is 4 bytes (little endian), we’ll lay out 3 entries.
    ((uint32_t*)sec)[0] = RESERVED | media;     // entry 0: media + reserved mask
    ((uint32_t*)sec)[1] = EOC;                  // entry 1: EOC
    ((uint32_t*)sec)[2] = EOC;                  // entry 2: EOC (root)
    // Write first sector of both FATs

    if(Write_Sector(fat0_lba + 0, sec) != 0){
        return 4;
    }

    if(Write_Sector(fat0_lba + fat_sectors + 0, sec) != 0){// mirror
        return 5;
    } 

    // Zero the remainder of both FATs
    memset(sec, 0x00, sizeof(sec));

    phex_8x8(fat_sectors, Get_RGB(0xffffff));
    newline();

    for (uint32_t s = 1; s < fat_sectors; s++){
        if(Write_Sector(fat0_lba + s, sec) != 0){
            return 6;
        } 

        if(Write_Sector(fat0_lba + fat_sectors + s, sec) != 0){
            return 7;
        } 

    }

    // ---- Zero the root directory cluster (cluster #2) ----
    const uint32_t sectors_per_cluster = spc;
    const uint32_t first_data_lba = rsvd + nfats * fat_sectors;
    const uint32_t root_lba = first_data_lba + (boot_sector->BP_Block.cluster_number_root_dir - 2) * sectors_per_cluster; // clus #2 => first data LBA
    memset(sec, 0x00, sizeof(sec));
    for (uint32_t i = 0; i < sectors_per_cluster; i++){
        if(Write_Sector(root_lba + i, sec) != 0){
            return 8;
        } 
    }

    superblock.info.magic = 0xa20Fac76;
    superblock.info.root_dir_start = root_lba;

    pstr_8x8("FAT32 format complete\n", Get_RGB(0x7CFC00));
    
    return 0;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"


static inline byte Write_Cluster(dword Cluster_num, void * data){
    const dword lba = Data_Start_sector + ((Cluster_num - boot_sector->BP_Block.cluster_number_root_dir) * SECT_PER_CLUST);
    return Bulk_Write_Sectors(lba, data, SECT_PER_CLUST);
}

static inline byte Read_Cluster(dword Cluster_num, void * buffer){
    const dword lba = Data_Start_sector + ((Cluster_num - boot_sector->BP_Block.cluster_number_root_dir) * SECT_PER_CLUST);
    return Bulk_Read_Sectors(lba, buffer, SECT_PER_CLUST);
}

#pragma GCC diagnostic pop

byte Mount_FAT32(void){
    byte status = Read_Sector(BOOT_SECTOR, boot_sector); // read in the boot sector (technically redundant on first boot)
    if(status != 0){ return 1; }


    status = Read_Sector(FS_INFO_SECTOR, FSinfo); // read in the FS info
    if(status != 0){ return 2; }

    
    status = strncmp("FAT32   ", boot_sector->BP_Block.system_id, 8);
    if(status != 0){ return 3; }

    FAT_Start = Data_Start_sector; // set the starting sector for the FATs
    Data_Start_sector += boot_sector->BP_Block.count_sectors_per_FAT32 * boot_sector->BP_Block.FAT_count;
    // Data_Start_sector now holds the first data sector

    Total_Clusters = (boot_sector->BP_Block.large_sectors_on_media - Data_Start_sector) / SECT_PER_CLUST;
    

    return 0;
}

dword Get_Fat_Entry(dword cluster){

    dword Fat_sector = FAT_Start + (cluster / FAT_ENTS_PER_SECT);
    
    if(Fat_sector >= (FAT_Start + boot_sector->BP_Block.count_sectors_per_FAT32)){
        return 0xffffffff;
    }
    
    word ent = cluster % FAT_ENTS_PER_SECT;
    dword Fat_sector_data[FAT_ENTS_PER_SECT] = {0};

    for(word FAT = 0; FAT < boot_sector->BP_Block.FAT_count; FAT++){
        if(Read_Sector(Fat_sector + (boot_sector->BP_Block.count_sectors_per_FAT32 * FAT), Fat_sector_data) == 0){
            break;
        }

        if(FAT == boot_sector->BP_Block.FAT_count - 1){
            return 0xfffffffe;
        }
    }

    return Fat_sector_data[ent] & 0x0fffffff;
}

byte Set_Fat_Entry(dword cluster, dword data){
    dword Fat_sector = FAT_Start + (cluster / FAT_ENTS_PER_SECT);
    
    if(Fat_sector >= (FAT_Start + boot_sector->BP_Block.count_sectors_per_FAT32)){
        return 1;
    }
    
    word  ent = cluster % FAT_ENTS_PER_SECT;
    dword Fat_sector_data[FAT_ENTS_PER_SECT] = {0};

    for(word FAT = 0; FAT < boot_sector->BP_Block.FAT_count; FAT++){
        if(Read_Sector(Fat_sector + (boot_sector->BP_Block.count_sectors_per_FAT32 * FAT), Fat_sector_data) == 0){
            Fat_sector_data[ent] = (Fat_sector_data[ent] & 0xF0000000) | (data & 0x0FFFFFFF);
            if(Write_Sector(Fat_sector + (boot_sector->BP_Block.count_sectors_per_FAT32 * FAT), Fat_sector_data) != 0){
                return 3;
            }
        }else{
            return 2;
        }
    }

    return 0;
}

dword clalloc(dword prev){
    for(dword clus = 2; clus < Total_Clusters; clus++){
        dword ent = Get_Fat_Entry(clus) & 0x0fffffff;

        if(ent == FREE){
            //entry is free
            if(Set_Fat_Entry(ent, EOC) != 0){
                return 0; // error setting the fat
            }

            if(prev >= 2 && prev <= 0xf0000000){
                if(Set_Fat_Entry(prev, clus) != 0){
                    return 0;
                }
            }

            byte zero[BYTES_PER_CLUST] = {0};

            if(Write_Cluster(clus, zero) != 0){
                return 0;
            }

            return clus;
        }
    }
    return 1;
}

byte freecl(dword Cluster) {
    dword cur = Cluster;
    dword next;

    while (1) {
        next = Get_Fat_Entry(cur);

        if(next >= 0xfffffffe){
            return 2;
        }

        // Free current cluster
        if (Set_Fat_Entry(cur, FREE) != 0) {
            return 1;
        }

        // If this was the last cluster
        if (next >= 0x0FFFFFF8 || next == FREE) {
            break; // end the loop
        }

        cur = next; // advance chain
    }

    return 0;
}

static inline word FAT32_get_time(byte hour, byte minute, byte second){
    return
    ((hour & 0x1f) << 11) | 
    ((minute & 0x3f) << 5) |
    ((second / 2) & 0x1f);
}

static inline word FAT32_get_date(byte year, byte month, byte day){
    return
    ((year & 0x7f) << 9) |
    ((month & 0x0f) << 5) |
    ((day & 0x1f));
}

byte add_dir_ent(dword clust, Dir_Ent ent){
    dword prev_clust;
    dword cluster_num = clust;
    Dir_Ent cluster_buf[BYTES_PER_CLUST / sizeof(Dir_Ent)] = {0};

    if(Read_Cluster(cluster_num, cluster_buf) != 0){
        return 1;
    }

    while(1){
        for(word i = 0; i < BYTES_PER_CLUST / sizeof(Dir_Ent); i++){
            if(cluster_buf[i].filename[0] == (char)0xE5 || cluster_buf[i].filename[0] == 0x00){
                // found free entry
                cluster_buf[i] = ent; // set the entry
                if(Write_Cluster(cluster_num, cluster_buf) != 0){
                    return 2;
                }

                return 0;
            }
        }

        // no free entry in this cluster

        prev_clust = cluster_num;
        cluster_num = Get_Fat_Entry(cluster_num);

        if(cluster_num >= 0xfffffffe){
            return 3;
        }

        if(cluster_num == EOC){
            cluster_num = clalloc(prev_clust); // set the cluster to zeros

            if(cluster_num < 2){
                return 4;
            }

            memset(cluster_buf, 0, sizeof(cluster_buf));

            cluster_buf[0] = ent;

            if(Write_Cluster(cluster_num, cluster_buf) != 0){
                return 5;
            }

            return 0;
        }
    }
}

byte del_dir_ent(dword clust, const char* filename){
    dword cluster_num = clust;
    Dir_Ent cluster_buf[BYTES_PER_CLUST / sizeof(Dir_Ent)] = {0};

    while (cluster_num != EOC){

        if(Read_Cluster(cluster_num, cluster_buf) != 0){
            return 2;
        }

        for(word i = 0; i < BYTES_PER_CLUST / sizeof(Dir_Ent); i++){
            if(cluster_buf[i].filename[0] == 0x00) return 1;

            if(
                cluster_buf[i].filename[0] != (char)0xE5 && 
                strncmp(
                     cluster_buf[i].filename,
                     filename, 
                     strlen(filename) > (short)sizeof(cluster_buf[i].filename) ? (short)sizeof(cluster_buf[i].filename) : strlen(filename)
                ) == 0
            ){
                cluster_buf[i].filename[0] = (char)0xE5;
                if(Write_Cluster(cluster_num, cluster_buf) != 0){
                    return 3;
                }

                if(
                    freecl(
                        ((cluster_buf[i].First_cluster_high & 0xffff) << 16) | (cluster_buf[i].First_cluster_low & 0xffff)
                    ) != 0
                ){
                    return 4;
                }

                return 0;
            }
        } 

        cluster_num = Get_Fat_Entry(cluster_num);
    }

    return 1;
}

dword get_cluster_from_path(const char* path){
    // Root shortcut
    if(strcmp(path, "/") == 0){
        return boot_sector->BP_Block.cluster_number_root_dir;
    }

    char local_path[MAX_PATH_LEN];
    memcpy((char *)path, local_path, MAX_PATH_LEN);
    int len = strlen(local_path);

    // Remove trailing slash
    if(local_path[len - 1] == '/'){
        local_path[len - 1] = '\0';
        len--;
    }

    dword cluster = boot_sector->BP_Block.cluster_number_root_dir; // start at the root directory

    word num_dirs = 0;
    for(short i = 0; i < len; i++){
        if(local_path[i] == '/') {
            num_dirs++;
            local_path[i] = '\0'; // split path into pieces
        }
    }

    char *dirs[num_dirs];

    {
        short Cur_pos = 1; // skip initial '/'
        for(int i = 0 ; i < num_dirs; i++){
            dirs[i] = &(local_path[Cur_pos]);
            Cur_pos += (short)(strlen(dirs[i]) + 1);
        }
    }

    Dir_Ent cluster_buf[BYTES_PER_CLUST / sizeof(Dir_Ent)] = {0};

    for(int d = 0; d < num_dirs; d++){
        byte found = 0;

        dword cur_cl = cluster;
        while(cur_cl < 0x0ffffff8 && cur_cl != FREE){
            if(Read_Cluster(cur_cl, cluster_buf) != 0){
                return 0xf0000000;
            }

            for(word i = 0; i < BYTES_PER_CLUST / sizeof(Dir_Ent); i++){
                if(cluster_buf[i].filename[0] == 0x00) break; // end of directory

                if(
                    cluster_buf[i].filename[0] != (char)0xE5 && 
                    strncmp(
                        cluster_buf[i].filename, 
                        dirs[d],
                        strlen(dirs[d]) > (short)sizeof(cluster_buf[i].filename) ? (short)sizeof(cluster_buf[i].filename) : strlen(dirs[d])
                    ) == 0
                )
                {
                    // Found matching entry
                    if((cluster_buf[i].attributes & FAT32_ATTR_SUBDIR) == 0 && d != num_dirs - 1){
                        // not a directory
                        return 0xf0000002;
                    }
                    cluster = ((cluster_buf[i].First_cluster_high & 0xffff) << 16) | 
                               (cluster_buf[i].First_cluster_low  & 0xffff);
                    found = 1;
                    break;
                }
            }

            if(found) break;
            cur_cl = Get_Fat_Entry(cur_cl); // next cluster in directory chain
        }

        if(!found) return 0xf0000001; // directory not found
    }

    return cluster;
}

byte Create_File(char* filename, dword creat_flags, dword cluster, word entry){
    Dir_Ent cluster_buf[BYTES_PER_CLUST / sizeof(Dir_Ent)] = {0};

    if(Read_Cluster(cluster, cluster_buf) != 0){
        return 1;
    }

    if(cluster_buf[entry].filename[0] != (char)0xE5 || cluster_buf[entry].filename[0] != 0x00){
        return 2;
    }


    if((creat_flags & 0xf0) != FAT32_ATTR_DEV){
        const dword tmp = clalloc(0);
        if (tmp < 2) {
            return 4;
        }

        cluster_buf[entry].First_cluster_high = (word)((tmp & 0xffff0000) >> 16);
        cluster_buf[entry].First_cluster_low =  (word)(tmp & 0xffff);
    }else{
        const dword tmp = DEV; // its a device so use the device cluster (non existant)

        cluster_buf[entry].First_cluster_high = (word)((tmp & 0xffff0000) >> 16);
        cluster_buf[entry].First_cluster_low =  (word)(tmp & 0xffff);
    }

    cluster_buf[entry].attributes = (byte)(creat_flags & 0xff);

    strncpy(
         filename, 
         cluster_buf[entry].filename, 
         strlen(filename) > (short)sizeof(cluster_buf[entry].filename) ? (short)sizeof(cluster_buf[entry].filename) : strlen(filename),
         0
    );

    if(Write_Cluster(cluster, cluster_buf) != 0){
        return 3;
    }

    return 0;
}

int open_file(const char *path, dword access_flags, dword creat_flags, byte system){
    byte mode = access_flags & O_MODE;
    const byte type = access_flags & O_TYPE;

    dword cluster;
    Dir_Ent cluster_buf[BYTES_PER_CLUST / sizeof(Dir_Ent)] = {0};
    char *filename = NULL;

    int fd = MAX_OPEN_FILES + 1; // define fd as max_open_files + 1 so we can check if theres space for it to be opened

    for(int i = 0; i < MAX_OPEN_FILES; i++){ // check all of the slots for the open
        if(open_file_table[i].used == 0){ // if its free
            memset(&open_file_table[i], 0, sizeof(OpenFile)); // set all values to zero to remove the previously opened file's info
            open_file_table[i].used = 1;  // it is no longer free
            fd = i; // set the fd
        }
    }

    if(fd == MAX_OPEN_FILES + 1){
        return -1; // no free open slot
    }    

    word ent_num = 0;

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

    { // only need the big var in this scope
        char path_tmp[MAX_PATH_LEN] = {0};
        memcpy((char*)path, path_tmp, MAX_PATH_LEN);

        for(byte i = 0; i < sizeof(cluster_buf->filename) + 1; i++){
            if(path_tmp[(strlen(path_tmp) - i)] == '/'){
                path_tmp[(strlen(path_tmp) - i)] = '\0';
                filename = &(path_tmp[(strlen(path_tmp) - i) + 1]);
            }
        }

        if(filename == NULL){
            return -6;
        }

        dword clust = get_cluster_from_path(path_tmp);

        if(clust >= 0xf0000000){
            if(clust == 0xf0000002){
                return -4;
            }else{
                return -5;
            }
        }

        byte found = 0;
        while(clust < 0x0ffffff8 && clust != FREE){
            if(Read_Cluster(clust, cluster_buf) != 0){
                return -2;
            }

            for(word i = 0; i < BYTES_PER_CLUST / sizeof(Dir_Ent); i++){
                if(cluster_buf[i].filename[0] == 0x00) break; // end of directory

                if(cluster_buf[i].filename[0] == (char)0xE5 || cluster_buf[i].filename[0] == 0x00){
                    if((type & O_CREAT) != 0){
                        // create the file
                        if(creat_flags == 0 && system == 0){
                            //a user program either tried to make a device file (not allowed), or forgot to input
                            return -10;
                        }

                        if(Create_File(filename, creat_flags, cluster, i) != 0){
                            return -8;
                        }
                    }
                }

                if(
                    cluster_buf[i].filename[0] != (char)0xE5 && 
                    strncmp(
                        cluster_buf[i].filename, 
                        filename,
                        strlen(filename) > (short)sizeof(cluster_buf[i].filename) ? (short)sizeof(cluster_buf[i].filename) : strlen(filename)
                    ) == 0
                )
                {
                    // Found matching entry
                    if((cluster_buf[i].attributes & FAT32_ATTR_FILE) == 0){
                        // not a file
                        return -3;
                    }

                    ent_num = i;
                    cluster = clust;

                    found = 1;
                    break;
                }
            }

            if(found) break;
            clust = Get_Fat_Entry(clust); // next cluster in directory chain
        }
    }

    if((type & O_TRUNC) != 0 && (type & O_CREAT) == 0){

       // TODO: need to stop trucation for device files 

        // need to truncate
        cluster_buf[ent_num].size = 0;
        if(freecl((((dword)cluster_buf[ent_num].First_cluster_high & 0xffff) << 16) | ((dword)cluster_buf[ent_num].First_cluster_high & 0xffff)) != 0){
            return -7;
        } else {
            dword tmp = clalloc(0);
            if(tmp < 2){
                return -9;
            }
            cluster_buf[ent_num].First_cluster_high = (word)((tmp & 0xffff0000) >> 16);
            cluster_buf[ent_num].First_cluster_low  = (word)(tmp & 0xffff);
        } 
        
        if(Write_Cluster(cluster, cluster_buf) != 0){
            return -11;
        }
    }
    
    #pragma GCC diagnostic pop

    if((type & O_APPEND) != 0){
        // need to append
        mode |= O_APPEND; // not the openers job to append its the writer, so tell it to append
    }
    

    open_file_table[fd].mode = mode; // tell the function that reads/writes this the mode it was opened in
    open_file_table[fd].file_size = cluster_buf[ent_num].size;
    open_file_table[fd].start_cluster = (((dword)cluster_buf[ent_num].First_cluster_high & 0xffff) << 16) | ((dword)cluster_buf[ent_num].First_cluster_high & 0xffff);
    open_file_table[fd].cur_cluster = open_file_table[fd].start_cluster;
    open_file_table[fd].pos = 0;
    
    return fd;
}

int write_file(int fd, const void *buf, int bytes){
    if(fd < 0 || fd >= MAX_OPEN_FILES){ // the fd is invalid
        return -1; 
    }else if(open_file_table[fd].used == 0){ // the fd is invalid
        return -2;
    }else if(bytes == 0){ // if were meant to write 0 bytes 
        return 0; // dont even write
    }else if(bytes < 0){ // cant write a negative amount of bytes
        return -3;
    }else if(buf == NULL){ // invalid pointer
        return -4; 
    }

    int bytes_writen = 0; // we haven't writen any bytes yet 

//  open_file_table[fd].cur_cluster; // the cluster that we are writing to right now
//  open_file_table[fd].file_size; // the size of the file (writer changes that)
//  open_file_table[fd].mode; // the mode it was opened (ex: RO, WO, RW, AP)
//  open_file_table[fd].pos; // the position in the cur_cluster were writing to
//  open_file_table[fd].start_cluster; // the start cluster, not really useful here

//  write code

    //dword num_cluster = bytes / BYTES_PER_CLUST; // number of full cluster we have to write
    //word  additional_byte = bytes % BYTES_PER_CLUST; // number of additional bytes we have to write


    
    return bytes_writen;
}

int read_file(int fd, void *buf, int bytes){
    if(fd < 0 || fd >= MAX_OPEN_FILES){ // the fd is invalid
        return -1; 
    }else if(open_file_table[fd].used == 0){ // the fd is invalid
        return -2;
    }else if(bytes == 0){ // if were meant to read 0 bytes 
        return 0; // dont even read
    }else if(bytes < 0){ // cant read a negative amount of bytes
        return -3;
    }else if(buf == NULL){ // invalid pointer
        return -4; 
    }

    int bytes_read = 0;

//    open_file_table[fd].cur_cluster; // the cluster that we are writing to right now
//    open_file_table[fd].file_size; // the size of the file (writer changes that)
//    open_file_table[fd].mode; // the mode it was opened (ex: RO, WO, RW, AP)
//    open_file_table[fd].pos; // the position in the cur_cluster were writing to
//    open_file_table[fd].start_cluster; // the start cluster, not really useful here

//read code


    return bytes_read;
}