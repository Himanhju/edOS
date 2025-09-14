#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <Graphics/VESA/VESA.h>

typedef struct Key{
    byte Scan;
    byte Ascii;
    byte Caps_Lock; // whether caps lock is on  58
}Key;

extern const byte stoa_data[127];

//parses a keystroke for ascii and toggles from the scancode
void Parse_Keystoke(void);

//gets the next key press after waiting for it to be pressed
void Get_Key(RGB color);

//get a string
void Get_Str(unsigned int max, char* output, RGB color);

#endif