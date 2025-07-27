#include "../../vdFS/vdFS.h"
#include "../../Memory/Segments/Segments.h"
#include "../../Memory/Operations/Operations.h"
#include "../../Graphics/VESA.h"

#include "Syscall.h"

//syscall function called when int 0x80 is called
int syscall(int EAX, int EBX, int ECX, int EDX, int EDI, int ESI){
    switch(EAX){
        case 0: 
            // ECX is the pointer to the start of the buf then we copy at an offset to a buffer each time we write
            // ebx should hold the address of the buffer in user memory
            // ecx should hold the address of the buffer in user memory
            // edx should hold the number of byte to read
            EAX = sys_read(EBX, (void *)ECX, (unsigned int)EDX, USER_DATA_SEG); // fix eax and add segment reading
            break;
        case 1: 
            EAX = sys_write(EBX, (void *)ECX, (unsigned int)EDX, USER_DATA_SEG);
            break;
        case 2: 
            EAX = sys_open((void *)EBX, USER_DATA_SEG);
            break;
        case 3: 
            EAX = sys_close(EBX);
            break;
        case 4:
            EAX = sys_fnctl(EBX, (void *)ECX, EDX, USER_DATA_SEG);
            break;
        case 5:
            EBX &= 0xFF; //only get ascii chars
            pchar_8x8((char)EBX, Get_RGB(ECX & 0x00FFFFFF)); // print ascii with the color in ECX
            return EBX; // return the character printed
        default:
            EAX = -1;
            break;
    }

    return EAX;
}

//read from a file
byte sys_read(int fd, void *buf, unsigned int bytes, byte segment){
    unsigned int sectors = bytes / 0x200; // auto-floors
    return 0;
}

//write to a file
byte sys_write(int fd, void *buf, unsigned int bytes, byte segment){
    unsigned int sectors = bytes / 0x200; // auto-floors
    short additional_bytes = bytes % 0x200;
    return 0;
}

// open a file
byte sys_open(void *path, byte segment){

    char local_path[seg_strlen(path, segment) > MAX_PATH_SIZE ? MAX_PATH_SIZE : seg_strlen(path, segment)]; // the path stored in kernel memory
    seg_memcpy(path, local_path, segment, Get_SS(), seg_strlen(path, segment) > MAX_PATH_SIZE ? MAX_PATH_SIZE : seg_strlen(path, segment)); // copy the data into kernel memory

    unsigned int metadata_sector = get_file_Metadata_Sector_From_Path_Absolute(local_path);
    if(metadata_sector < DATA_START_BLOCK){
        //returned error
        return metadata_sector + 1; // 0 is an error from the function as well 
    }

    Dir_Entry Entry;
    Entry.flags = ENTRY_ALLOCATED_FLAG | FILE_FLAG | HARDLINK_FLAG;
    Dec_To_Hex_Short(metadata_sector, Entry.Metadata_Block);
    unsigned int fd = get_Dir_Sector_From_Path_Absolute("/dev/fd");
    //TODO set entry name to the number needed
    if(fd < DATA_START_BLOCK){
        return 7; // couldnt get fd sectors
    }
    add_Dir_Entry(fd, Entry);
    return 0;
}

// closes a file
byte sys_close(int fd){
    return 0;
}

// do operations on a file 
byte sys_fnctl(int Selector, void *PATH, int Flags, byte segment){
    switch(Selector){
        case FNCTL_ADD_FILE:
            return add_File_With_Path(PATH);
            break;
        case FNCTL_ADD_DIR:
            return add_Dir_With_Path(PATH);
            break;
        default:
            break;
    }
    return -1;
}