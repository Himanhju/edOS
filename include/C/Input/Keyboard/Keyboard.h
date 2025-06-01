#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../../Graphics/VESA.h"

typedef struct Key{
    byte Scan;
    byte Ascii;
    byte Caps_Lock; // whether caps lock is on  58
}Key;

//gets the next key press after waiting for it to be pressed
Key Get_Key(void);

//get a string
void Get_Str(unsigned int max, char* output, RGB color);

#endif