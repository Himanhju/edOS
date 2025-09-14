#ifndef VESA_H
#define VESA_H

#include <Types/Common.h>

// The 8x8 Font
//Bitmage font
#include "8x8_Font.h"

//rgb version of vrams color mode
typedef struct RGB{
    byte b;  // Red channel (0-255)
    byte g;  // Green channel (0-255)
    byte r;  // Blue channel (0-255)
} RGB;

#define PIXEL_WIDTH  1024
#define PIXEL_HEIGHT 768

//video ram
extern volatile RGB* VRAM;

// the cursor position of where to print letters
extern unsigned int cursor_pos[2];

//initiallizes VRAM (Graphics will break if its not run)
void VESA_Init(void);

//goes to the next line and returns to columb 0
void newline(void);

//goes to the line above and does not return to columb 0
void Move_Cursor_Up(void);

//goes to the next line and does not return to columb 0
void Move_Cursor_Down(void);

//moves the cursor to the colum to the left
void Move_Cursor_Left(void);

//moves the cursor to the colum to the right 
void Move_Cursor_Right(void);

//sets the color of a pixel
void Set_Pixel(dword x, dword y, RGB color);

//draws a rectangle of color color
void Draw_Rect(dword Origin_X, dword Origin_Y, dword Width, dword Height, RGB color);

//returns the RGB struct based on the integer given (perferably hexadecimal)
RGB Get_RGB(int hex);

//clears the screen and sets all pixels to black
void clr_scrn(void);

//scrolls the screen down by lines lines
void scroll(dword lines);

//prints the bitmage of a letter
void pchar_8x8_bitmage(char letter[8][8], RGB color);

//prints a character of the color color
void pchar_8x8(char c, RGB color);

//prints a string at point str in the color color
void pstr_8x8(char* str, RGB color);

//prints the integer with the color color
void pint_8x8(long long integer, RGB color);

//prints the decimal in hex in color color without the 0x
void phex_8x8(unsigned int decimal, RGB color);

// prints the decimal in binary in color color with bit 31 on the left and bit 0 on the right
void pbin_8x8(unsigned int decimal, RGB color);

#endif