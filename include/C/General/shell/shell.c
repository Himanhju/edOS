#include "../../Types/Common.h"
#include "../../Memory/Opcodes/Opcodes.h"
#include "../../Memory/Operations/Operations.h"
#include "../../Graphics/VESA.h"

#include "shell.h"

void parse(char* input){
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

    if(strncmp(args[0], "halt", 5) == 0){
        halt();
    }else if(strncmp("echo", args[0], 5) == 0){
        for(byte i = 1; i < num_input + 1; i++){
            if(args[i][0] == '\0'){
                pchar_8x8(' ', Get_RGB(0xffffff));
                continue;
            }
            pstr_8x8(args[i], Get_RGB(0xffffff));
            pchar_8x8(' ', Get_RGB(0xffffff));
        }
    }else{
        pstr_8x8(args[0], Get_RGB(0xffffff));
        pstr_8x8(": command not found", Get_RGB(0xffffff));
    }
    newline();
}