//Pixel Part:

#include "../Types/Common.h"
#include "../Memory/Operations/Operations.h"

#include "VESA.h"

// the cursor position in    {x, y} type
unsigned int cursor_pos[2] = {0, 0};
volatile RGB* VRAM;

#define PIXEL_WIDTH  1024
#define PIXEL_HEIGHT 1024

void VESA_Init(){
    volatile unsigned int* vbe_mode_info = (volatile unsigned int*) 0x5028;
    unsigned int framebuffer_phys = *(vbe_mode_info + (0x28 / 4)); // offset 0x28
    VRAM = (volatile RGB*) (framebuffer_phys);
}

void newline(void){
    cursor_pos[0] = 0;
    cursor_pos[1] += 8;
}

void Move_Cursor_Up(void){
    cursor_pos[1] -= 8;
}

void Move_Cursor_Down(void){
    cursor_pos[1] += 8;
}

void Set_Pixel(int x, int y, RGB color){
    VRAM[(y * PIXEL_WIDTH) + x] = color; 
    
}

void Draw_Rect(int Origin_X, int Origin_Y, int Width, int Height, RGB color){
    for(int x = Origin_X; x < (Origin_X + Width) % (PIXEL_WIDTH + 1); x++){
        for(int y = Origin_Y; y < (Origin_Y + Height) % (PIXEL_HEIGHT + 1); y++){
            Set_Pixel(x, y, color);
        }
    }
}

RGB Get_RGB(int hex){
    RGB color;
    color.r =(byte) ((hex >> 16)  & 0xFF);  // 0x123456 >> 16 = 0x12        & 0xFF = 0x12         = 0x12
    color.g =(byte) (((hex >> 8) & 0xFF));  // 0x123456 >> 8  = 0x1234      & 0xFF = 0x0034       = 0x34
    color.b =(byte) (((hex)  & 0xFF));      // 0x123456       = 0x123456    & 0xFF = 0x000056     = 0x56
    return color;
}

void clr_scrn(void){
    Draw_Rect(0, 0, PIXEL_WIDTH, PIXEL_HEIGHT, Get_RGB(0X00000000));
    cursor_pos[0] = 0;
    cursor_pos[1] = 0;
}

char Graphics_A_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0}
};
char Graphics_B_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 0}
};
char Graphics_C_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}
};
char Graphics_D_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 0}
};
char Graphics_E_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0}
};
char Graphics_F_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0}
};
char Graphics_G_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0}
};
char Graphics_H_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0}
};
char Graphics_I_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0}
};
char Graphics_J_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0}
};
char Graphics_K_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 0},
    {0, 1, 0, 1, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0},
    {0, 1, 0, 1, 0, 0, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0}
};
char Graphics_L_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0}
};
char Graphics_M_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 1, 0, 1, 1, 0, 0},
    {0, 1, 0, 1, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0}
};
char Graphics_N_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 0, 0},
    {0, 1, 0, 1, 0, 1, 0, 0},
    {0, 1, 0, 0, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0}
};
char Graphics_O_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0}
};
char Graphics_P_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0}
};
char Graphics_Q_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 0},
    {0, 1, 0, 0, 1, 0, 1, 0},
    {0, 1, 0, 0, 1, 0, 1, 0},
    {0, 0, 1, 1, 0, 1, 0, 0}
};
char Graphics_R_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 0}
};
char Graphics_S_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 0}
};
char Graphics_T_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0}
};
char Graphics_U_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0}
};
char Graphics_V_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0}
};
char Graphics_W_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 1, 0, 1, 0, 0},
    {0, 1, 1, 0, 1, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0}
};
char Graphics_X_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0}
};
char Graphics_Y_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0}
};
char Graphics_Z_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0}
};

char Graphics_0_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 1, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 0 ,0 ,0}
};
char Graphics_1_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0}
};
char Graphics_2_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0}
};
char Graphics_3_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0}
};
char Graphics_4_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0}
};
char Graphics_5_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 0, 0}
};
char Graphics_6_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0}
};
char Graphics_7_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0}
};
char Graphics_8_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0}
};
char Graphics_9_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 1, 0, 0, 1, 0, 0},
    {0, 0, 0, 1, 1, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 1, 1, 1, 0, 0, 0}
};

char Graphics_space_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

char Graphics_colon_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 1, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

char Graphics_greater_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0}
};

char Graphics_percent_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 0, 1, 1, 0, 0}
};

char Graphics_comma_8x8[8][8] = {
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0}
};

//Text Part:
void pchar_8x8_bitmage(char letter[8][8], RGB color) {
    // Check if the cursor has reached the end of the line
    if (cursor_pos[0] >= PIXEL_WIDTH - 8) {
        newline();
    }

    // Check if the cursor has reached the bottom of the screen
    if (cursor_pos[1] >= PIXEL_HEIGHT - 8) {
        clr_scrn();                         // Clear the screen
        cursor_pos[0] = 0;                  // Reset cursor position
        cursor_pos[1] = 0;
    }

    // Draw the character
    for (byte y = 0; y < 8; y++) {
        for (byte x = 0; x < 8; x++) {
            if (letter[y][x] == 1) {
                Set_Pixel(x + cursor_pos[0], y + cursor_pos[1], color);
            }
        }
    }

    // Advance the cursor horizontally
    cursor_pos[0] += 8;
}

void pchar_8x8(char c, RGB color){
    switch (c){
        case 'A':
            pchar_8x8_bitmage(Graphics_A_8x8, color);
            break;
        case 'B':
            pchar_8x8_bitmage(Graphics_B_8x8, color);
            break;
        case 'C':
            pchar_8x8_bitmage(Graphics_C_8x8, color);
            break;
        case 'D':
            pchar_8x8_bitmage(Graphics_D_8x8, color);
            break;
        case 'E':
            pchar_8x8_bitmage(Graphics_E_8x8, color);
            break;
        case 'F':
            pchar_8x8_bitmage(Graphics_F_8x8, color);
            break;
        case 'G':
            pchar_8x8_bitmage(Graphics_G_8x8, color);
            break;
        case 'H':
            pchar_8x8_bitmage(Graphics_H_8x8, color);
            break;
        case 'I':
            pchar_8x8_bitmage(Graphics_I_8x8, color);
            break;
        case 'J':
            pchar_8x8_bitmage(Graphics_J_8x8, color);
            break;
        case 'K':
            pchar_8x8_bitmage(Graphics_K_8x8, color);
            break;
        case 'L':
            pchar_8x8_bitmage(Graphics_L_8x8, color);
            break;
        case 'M':
            pchar_8x8_bitmage(Graphics_M_8x8, color);
            break;
        case 'N':
            pchar_8x8_bitmage(Graphics_N_8x8, color);
            break;
        case 'O':
            pchar_8x8_bitmage(Graphics_O_8x8, color);
            break;
        case 'P':
            pchar_8x8_bitmage(Graphics_P_8x8, color);
            break;
        case 'Q':
            pchar_8x8_bitmage(Graphics_Q_8x8, color);
            break;
        case 'R':
            pchar_8x8_bitmage(Graphics_R_8x8, color);
            break;
        case 'S':
            pchar_8x8_bitmage(Graphics_S_8x8, color);
            break;
        case 'T':
            pchar_8x8_bitmage(Graphics_T_8x8, color);
            break;
        case 'U':
            pchar_8x8_bitmage(Graphics_U_8x8, color);
            break;
        case 'V':
            pchar_8x8_bitmage(Graphics_V_8x8, color);
            break;
        case 'W':
            pchar_8x8_bitmage(Graphics_W_8x8, color);
            break;
        case 'X':
            pchar_8x8_bitmage(Graphics_X_8x8, color);
            break;
        case 'Y':
            pchar_8x8_bitmage(Graphics_Y_8x8, color);
            break;
        case 'Z':
            pchar_8x8_bitmage(Graphics_Z_8x8, color);
            break;
        case '0':
            pchar_8x8_bitmage(Graphics_0_8x8, color);
            break;
        case '1':
            pchar_8x8_bitmage(Graphics_1_8x8, color);
            break;
        case '2':
            pchar_8x8_bitmage(Graphics_2_8x8, color);
            break;
        case '3':
            pchar_8x8_bitmage(Graphics_3_8x8, color);
            break;
        case '4':
            pchar_8x8_bitmage(Graphics_4_8x8, color);
            break;
        case '5':
            pchar_8x8_bitmage(Graphics_5_8x8, color);
            break;
        case '6':
            pchar_8x8_bitmage(Graphics_6_8x8, color);
            break;
        case '7':
            pchar_8x8_bitmage(Graphics_7_8x8, color);
            break;
        case '8':
            pchar_8x8_bitmage(Graphics_8_8x8, color);
            break;
        case '9':
            pchar_8x8_bitmage(Graphics_9_8x8, color);
            break;
        case ' ':
            pchar_8x8_bitmage(Graphics_space_8x8, color);
            break;
        case ':':
            pchar_8x8_bitmage(Graphics_colon_8x8, color);
            break;
        case '>':
            pchar_8x8_bitmage(Graphics_greater_8x8, color);
            break;
        case '%':
            pchar_8x8_bitmage(Graphics_percent_8x8, color);
            break;
        case ',':
            pchar_8x8_bitmage(Graphics_comma_8x8, color);
            break;
        case '\0':
            pchar_8x8_bitmage(Graphics_space_8x8, color);
            break;
        default:
            break;
    }
}

void pstr_8x8(char* str, RGB color){
    while (*str) {
        pchar_8x8(Upper(*str++), color);
    }
}

void pint_8x8(long long integer, RGB color){
    char buf[255];
    uitos(integer, buf);
    pstr_8x8(buf, color);
}

