#ifndef VESA_H
#define VESA_H

#include "../Types/Common.h"

//rgb version of vrams color mode
typedef struct RGB{
    byte b;  // Red channel (0-255)
    byte g;  // Green channel (0-255)
    byte r;  // Blue channel (0-255)
} RGB;

#define PIXEL_WIDTH 1024
#define PIXEL_HEIGHT 1024

//video ram
extern volatile RGB* VRAM;

//initiallizes VRAM (Graphics will break if its not run)
void VESA_Init();

//goes to the next line and returns to columb 0
void newline(void);

//goes to the line above and does not return to columb 0
void Move_Cursor_Up(void);

//goes to the next line and does not return to columb 0
void Move_Cursor_Down(void);

//sets the color of a pixel
void Set_Pixel(int x, int y, RGB color);

//draws a rectangle of color color
void Draw_Rect(int Origin_X, int Origin_Y, int Width, int Height, RGB color);

//returns the RGB struct based on the integer given (perferably hexadecimal)
RGB Get_RGB(int hex);

//clears the screen and sets all pixels to black
void clr_scrn(void);

// the cursor position of where to print letters
extern unsigned int cursor_pos[2];

// The 8x8 Font and print function

//Bitmage font
#include "8x8_Font.h"

//prints the bitmage of a letter
void pchar_8x8_bitmage(char letter[8][8], RGB color);

//prints a character of the color color
void pchar_8x8(char c, RGB color);

//prints a string at point str in the color color
void pstr_8x8(char* str, RGB color);

//prints the integer with the color color
void pint_8x8(long long integer, RGB color);

#endif