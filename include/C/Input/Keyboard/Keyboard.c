#include <Types/Common.h>
#include <Memory/Operations/Operations.h>
#include <Interrupts/ISRs/ISRs.h>
#include <Graphics/VESA/VESA.h>
#include <General/Time/Time.h>

#include "Keyboard.h"

Key key;

const byte stoa_data[127] = {
    [0] = 0, [1] = 27, // escape
    [2] = '1', [3] = '2', [4] = '3', [5] = '4', [6] = '5', [7] = '6', [8] = '7', [9] = '8', [10] = '9', [11] = '0',
    [30] = 'a', [48] = 'b', [46] = 'c', [32] = 'd', [18] = 'e', [33] = 'f', [34] = 'g', [35] = 'h', [23] = 'i', [36] = 'j',
    [37] = 'k', [38] = 'l', [50] = 'm', [49] = 'n', [24] = 'o', [25] = 'p', [16] = 'q', [19] = 'r', [31] = 's', [20] = 't',
    [22] = 'u', [47] = 'v', [17] = 'w', [45] = 'x', [21] = 'y', [44] = 'z',
    [12] = '-', [13] = '=', [26] = '[', [27] = ']', [43] = '\\', [39] = ';', [40] = '\'', [41] = '`', [51] = ',', [52] = '.',
    [53] = '/', [57] = ' ', [14] = 8, // backspace
    [28] = '\r', // enter
    // Other keys default to 0 (no valid ASCII)
};

void Parse_Keystoke(void){

    if(key.Scan == 58){ // if its caps lock
        if(key.Caps_Lock == 0){
            key.Caps_Lock = 1;
        }else{
            key.Caps_Lock = 0;
        }
    }
    key.Ascii = stoa_data[key.Scan];
}

void Get_Key(RGB color){
    pchar_8x8('_', color);
    Move_Cursor_Left();
    while(!Key_Pressed); // Wait until a key is pressed

    // Key has been pressed
    key.Scan  = Scancode;
    Parse_Keystoke();
    
    psleep(175);
}

void Get_Str(unsigned int max, char* output, RGB color){

    for(unsigned int i = 0; i < max; i++){
        Get_Key(color);
        
        if(key.Ascii == '\r'){

            pchar_8x8(' ', color);
            Move_Cursor_Left();

            for(unsigned int o = i; o < max; o++){
                output[o] = '\0';
            }
            break;
        } else if(key.Ascii == 8 /*backspace*/){
            if(i < 1){
				i--;
				continue;
			}

            pchar_8x8(' ', color);
            Move_Cursor_Left();

			i -= 2;
			Move_Cursor_Left();
            pchar_8x8(0, color);
            Move_Cursor_Left();
			continue;
        }

        output[i] = (char)key.Ascii;
        pchar_8x8((char)Upper(key.Ascii), color);
    }
    newline();
}

/*
    switch (scan){
        case 0: break;
        case 1: ascii = 27; break; //escape
        
        //numbers
        case 2: ascii = '1'; break;
        case 3: ascii = '2'; break;
        case 4: ascii = '3'; break;
        case 5: ascii = '4'; break;
        case 6: ascii = '5'; break;
        case 7: ascii = '6'; break;
        case 8: ascii = '7'; break;
        case 9: ascii = '8'; break;
        case 10: ascii = '9'; break;
        case 11: ascii = '0'; break;

        //letters
        case 30: ascii = 'a'; break;
        case 48: ascii = 'b'; break;
        case 46: ascii = 'c'; break;
        case 32: ascii = 'd'; break;
        case 18: ascii = 'e'; break;
        case 33: ascii = 'f'; break;
        case 34: ascii = 'g'; break;
        case 35: ascii = 'h'; break;
        case 23: ascii = 'i'; break;
        case 36: ascii = 'j'; break;
        case 37: ascii = 'k'; break;
        case 38: ascii = 'l'; break;
        case 50: ascii = 'm'; break;
        case 49: ascii = 'n'; break;
        case 24: ascii = 'o'; break;
        case 25: ascii = 'p'; break;
        case 16: ascii = 'q'; break;
        case 19: ascii = 'r'; break;
        case 31: ascii = 's'; break;
        case 20: ascii = 't'; break;
        case 22: ascii = 'u'; break;
        case 47: ascii = 'v'; break;
        case 17: ascii = 'w'; break;
        case 45: ascii = 'x'; break;
        case 21: ascii = 'y'; break;
        case 44: ascii = 'z'; break;

        //special characters
        case 12: ascii = '-'; break;
        case 13: ascii = '='; break;
        case 26: ascii = '['; break;
        case 27: ascii = ']'; break;
        case 43: ascii = '\\'; break;
        case 39: ascii = ';'; break;
        case 40: ascii = '\''; break;
        case 41: ascii = '`'; break;
        case 51: ascii = ','; break;
        case 52: ascii = '.'; break;
        case 53: ascii = '/'; break;
        case 57: ascii = ' '; break;
        case 14: ascii = 8; break; // backspace
        case 28: ascii = '\r'; break; //enter

        // no value keys
        case 42: break; //shift
        case 58: break; //caps lock
        case 15: break; //tab
        case 59: break; // f1
        case 60: break; // f2
        case 61: break; // f3
        case 62: break; // f4
        case 63: break; // f5
        case 64: break; // f6
        case 65: break; // f7
        case 66: break; // f8
        case 67: break; // f9
        case 68: break; // f10
        case 87: break; // f11
        case 88: break; // f12
        case 29: break; // ctrl
        case 91: break; // super (windows)
        case 56: break; // alt
        case 55: break; // prt scn
        case 70: break; // scrl lock
        case 82: break; // ins
        case 71: break; // Home
        case 73: break; // PG UP
        case 83: break; // del
        case 79: break; // END
        case 81: break; //PG DN
        case 80: break; // down arrow
        case 75: break; // left arrow
        case 77: break; // right arrow
        case 72: break; // up arrow

        //numpad
        //currently not supported


        default: ascii = 0; break;
    }*/