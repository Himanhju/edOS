#include <Types/Common.h>
#include <Memory/Opcodes/Opcodes.h>
#include <Memory/Operations/Operations.h>
#include <Graphics/VESA/VESA.h>
#include <Coms/PCI/PCI.h>
#include <FS/FAT32/FAT32.h>

#include "shell.h"

byte parse(char* input){
    int input_len = strlen(input);
    int num_input = 1;
    for(int c = 0; c < input_len; c++) {
        if(input[c] == ' '){
            num_input++;
        }
    }
    int input_lens[num_input];
    int input_starts[num_input];
    int cur_input = 0;
    for(int o = 0; o < num_input; o++){
        input_lens[o] = 0;
        input_starts[o] = 0;
    }
    for(int c = 0; c < input_len; c++){
        if(input[c] == ' '){ cur_input++; input_starts[cur_input] = c + 1; continue;}                                             
        input_lens[cur_input]++;
    }
    char args[num_input][MAX_ARG_SIZE];

    for(int i = 0; i < num_input; i++){
        for(int o = 0; o < MAX_ARG_SIZE; o++) {
            args[i][o] = '\0';
        }
        for(int c = input_starts[i]; c < input_starts[i] + input_lens[i]; c++) {
            args[i][c - input_starts[i]] = input[c];
        }
    }

    if(strncmp("halt", args[0], 5) == 0){
        halt();
    }else if(strncmp("echo", args[0], 5) == 0){
        for(byte i = 1; i < num_input + 1; i++){
            if(args[i][0] == '\0'){
                pchar_8x8(' ', WHITE);
                continue;
            }
            pstr_8x8(args[i], WHITE);
            pchar_8x8(' ', WHITE);
        }
    }else if(strncmp("cont", args[0], 5) == 0){
        return 1;

    }else if(strncmp("mkfs.fat32", args[0], 11) == 0){

        //initialize the FS
        
        pstr_8x8("\nInitializing FS\n", WHITE);
        byte Format_return = Format_FAT32();
        if(Format_return == 0){
            pstr_8x8("Fat32 formatted\n", WHITE);
            pstr_8x8("\nFS Initialized\n", SUCCESS);


            pstr_8x8("\nMounting FS\n", WHITE);

            switch(Mount_FAT32()){
            default:
                pstr_8x8("Failed To Mount FS: Unknown error\n\n", FAIL);
                break;

            case 1:
                pstr_8x8("Failed To Mount FS: Couldn't read boot sector\n\n", FAIL);
                break;

            case 2:
                pstr_8x8("Failed To Mount FS: Couldn't read FS Info sector\n\n", FAIL);
                break;

            case 3:
                pstr_8x8("Failed To Mount FS: Invalid FS ID\n\n", FAIL);
                break;

            case 0:
                pstr_8x8("FS Mounted\n\n", SUCCESS);
            }

        }else{
            pstr_8x8("Failed To Format as fat32: error code: ", FAIL);
            pint_8x8((long long)Format_return, FAIL);
            newline();
            newline();
        }
        
    
    }else if(strncmp("lspci", args[0], 6) == 0){
        newline();
        newline();
        Print_All_Connected_PCI_Devices();
    }else if(strncmp("clear", args[0], 6) == 0){
        clr_scrn();
        return 0; // dont make a newline
    }else if(strncmp("help", args[0], 5) == 0){
        pstr_8x8(
"\n\
TMP Shell Built in Commands\n\
\n\
    halt: Halts execution not recommended\n\
    echo: prints the next arguments out to the screen\n\
    lspci: prints all pci devices only their vendor id, device id, and the port their connected to\n\
    clear: clears the screen\n\
    help: prints this help message\n\
    cont: continues booting to the rest of the OS\n\
    mkfs: formats the drive as this fs (can be destructive) ex: mkfs.fat32\n\
    \n\
    NOTE: the max length of an argument is 64 characters then it will overflow to the next argument\n"
            ,
            WHITE
        );
    }else{
        pstr_8x8(args[0], WHITE);
        pstr_8x8(": command not found", WHITE);
    }
    newline();
    return 0;
}