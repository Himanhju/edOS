#ifndef COMMON_TYPES
#define COMMON_TYPES

#define NULL ((void *)0)

//common assembly types

//a byte, 8 bits unsigned
typedef unsigned char byte;

//a word, 16 bits unsigned
typedef unsigned short word;

//a dword, double word, 32 bits unsigned
typedef unsigned int dword;

//a qword, quadruple word, 64 bits unsigned
typedef unsigned long long qword;

#include "pointers.h"
#include "stdint.h"

#endif