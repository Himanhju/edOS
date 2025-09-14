#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <Types/Common.h>

//gets the digit count of the number given
byte dc(const int num);

//the exponetiation function for integers psudo code: base^exp 
int intpow(const int base, const short exp);

//converts the number given to a string put into output (in unsigned mode)
void uitos(const int num, char* output);

//converts letters to uppercase
byte Upper(byte low);

//gets the length of a string from the begining to the null terminator
short strlen(const char* str);

//compares the first n bytes of str1 with str2
byte strncmp(const char* str1, const char* str2, const short n);

//compares str1 to str2 until the null terminator of str1
byte strcmp(const char* str1, const char* str2);

//copys the data from src to dest at an off set of offset (i should really remove that offset part)
void strcpy(const char* src, char* dest, const short offset);

//copies the first n bytes of src to dest with an offset of offset
void strncpy(const char* src, char* dest, const short n, const short offset);

//returns an integer which is the integer version of the string  
unsigned int stoui(const char* str);

//copies bytes bytes of memory from src to dest
void memcpy(void *src, void *dest, unsigned int bytes);

//compares the first bytes bytes of thing1 with thing2, returns 0 if their the same and 1 if not
byte memcmp(void *thing1, void *thing2, unsigned int bytes);

//set the first bytes bytes of dest to value
void memset(void *dest, byte value, unsigned int bytes);

//convers Hex in the form of "FEDCBA98" to a decimal integer (depricated)
unsigned int Hex_To_Dec(char* num);

//get the digit count of a number in base 16
byte dc_16(const int num);

//converts a base ten number to base 16 which is put in output (depricated)
void Dec_To_Hex(unsigned int num, char* output);

//convers Hex in the form of {0x00, 0x00, 0x00, 0x1F} to a decimal integer in this case 31 (used for memory efficiency)
unsigned int Hex_To_Dec_Short(byte* num);

//converts a base ten number like 31 to base 16 in the form of {0x00, 0x00, 0x00, 0x1f}
void Dec_To_Hex_Short(unsigned int num, byte* output);

#endif