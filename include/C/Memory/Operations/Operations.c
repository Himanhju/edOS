#include <Types/Common.h>

#include "Operations.h"

byte dc(const int num){
    byte result = 1;
    if (num >= 10) {
        result = 1 + dc(num / 10);
    }
    return result;
}

int intpow(const int base, const short exp){
    int answer = 1;
    for(short i = 0; i < exp; i++){
        answer *= base;
    }
    return answer;
}

void uitos(const int num, char* output){
    byte digits = dc(num);
    int curnum;
    for (int i = 0; i < digits; i++) {
        curnum = num;
        curnum %= intpow(10, (short)(digits - i));               // Remove digits to the left
        curnum -= (curnum % intpow(10, (short)(digits - i - 1))); // Remove digits to the right
        curnum /= intpow(10, (short)(digits - i - 1));           // Isolate current digit
        output[i] = ((char)curnum) + '0';
    }

    output[digits] = '\0';           // Null-terminate the string
}

byte Upper(byte low){
    if(low <= 'z' && low >= 'a'){
        return low - 32;
    }else{
        return low;
    }
}

//returns the lengths of the given string
short strlen(const char* str){
    short len;
    for(len = 0; str[len] != '\0';len++);
    return len;
}

//compares the first n bytes, returns 0 if they are the same
byte strncmp(const char* str1, const char* str2, const short n){
    for(short i = 0; i < n; i++){
        if(str1[i] != str2[i]){
            return 2;
        }
    }
    return 0;
}

//returns 0 if they are the same
byte strcmp(const char* str1, const char* str2){
    return strncmp(str1, str2, strlen(str1));
}

//copies the first string into the second string
void strcpy(const char* src, char* dest, const short offset){
    for(int i = offset; i < strlen(src); i++){       //if necisarry then also add return the amount of bytes copied
        dest[i - offset] = src[i];
    }
}

//copies the first n bytes from the first sting into the second string
void strncpy(const char* src, char* dest, const short n, const short offset){
    for(int i = offset; i < offset + n; i++){       //if necisarry then also add return the amount of bytes copied
        dest[i] = src[i];
    }
}

//converts a string into an unsigned integer
unsigned int stoui(const char* str){
    unsigned int ret = 0;
    int len = strlen(str);

    for(short i = 0; i < len; i++){
        char c = str[i];
        if(c >= '0' && c <= '9'){
            ret = (unsigned int)((ret * 10) + (unsigned int)(c - '0'));
        } else {
            break; // or handle error
        }
    }
    return ret;
}

void memcpy(void *src, void *dest, unsigned int bytes){
    byte* Dest = (byte *)dest;
    byte* Src  = (byte *)src;// set the pointers

    for(unsigned int i = 0; i < bytes; i++){
        Dest[i] = Src[i];
    }
}

byte memcmp(void *thing1, void *thing2, unsigned int bytes){
    byte *Thing1 = (byte *)thing1;
    byte *Thing2 = (byte *)thing2; // set the pointers

    for(unsigned int i = 0; i < bytes; i++){
        if(Thing1[i] != Thing2[i]){
            return 1;
        }
    }
    return 0;
}

void memset(void *dest, byte value, unsigned int bytes){
    byte *pointer = (byte *) dest;
    for(unsigned int i = 0; i < bytes; i++){
        pointer[i] = value;
    }
}

unsigned int Hex_To_Dec(char* num){
    unsigned int ret = 0;
    for(int i = strlen(num) - 1; i >= 0; i--){
        switch (Upper((byte)num[i])){
            case 'F':ret += (unsigned int)(15 * (intpow(16, (short)((strlen(num) - 1) - i))));break;
            case 'E':ret += (unsigned int)(14 * (intpow(16, (short)((strlen(num) - 1) - i))));break;
            case 'D':ret += (unsigned int)(13 * (intpow(16, (short)((strlen(num) - 1) - i))));break;
            case 'C':ret += (unsigned int)(12 * (intpow(16, (short)((strlen(num) - 1) - i))));break;
            case 'B':ret += (unsigned int)(11 * (intpow(16, (short)((strlen(num) - 1) - i))));break;
            case 'A':ret += (unsigned int)(10 * (intpow(16, (short)((strlen(num) - 1) - i))));break;
            case '9':ret += (unsigned int)(9 * (intpow(16, (short)((strlen(num) - 1) - i))));break;
            case '8':ret += (unsigned int)(8 * (intpow(16, (short)((strlen(num) - 1) - i))));break;
            case '7':ret += (unsigned int)(7 * (intpow(16, (short)((strlen(num) - 1) - i))));break;
            case '6':ret += (unsigned int)(6 * (intpow(16, (short)((strlen(num) - 1) - i))));break;
            case '5':ret += (unsigned int)(5 * (intpow(16, (short)((strlen(num) - 1) - i))));break;
            case '4':ret += (unsigned int)(4 * (intpow(16, (short)((strlen(num) - 1) - i))));break;
            case '3':ret += (unsigned int)(3 * (intpow(16, (short)((strlen(num) - 1) - i))));break;
            case '2':ret += (unsigned int)(2 * (intpow(16, (short)((strlen(num) - 1) - i))));break;
            case '1':ret += (unsigned int)(1 * (intpow(16, (short)((strlen(num) - 1) - i))));break;
            case '0':break;
            default :continue;
        }
        if(i == 0){
            break;
        }
    }
    return ret;
}

byte dc_16(const int num){
    byte result = 1;
    if (num >= 16) {
        result = 1 + dc_16(num / 16);
    }
    return result;
}

void Dec_To_Hex(unsigned int num, char* output){
    char INDEX[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    byte digits = dc_16((int)num);
    int curnum;
    for (int i = 0; i < digits; i++) {
        curnum = (int)num;
        curnum %= intpow(16, (short)(digits - i));                // Remove digits to the left
        curnum -= (curnum % intpow(16, (short)(digits - i - 1))); // Remove digits to the right
        curnum /= intpow(16, (short)(digits - i - 1));            // Isolate current digit
        output[i] = INDEX[curnum];
    }

    output[digits] = '\0'; 
}

unsigned int Hex_To_Dec_Short(byte* num) {
    return ((unsigned int)num[0] << 24) |
           ((unsigned int)num[1] << 16) |
           ((unsigned int)num[2] << 8)  |
           ((unsigned int)num[3]);
}

void Dec_To_Hex_Short(unsigned int num, byte* output) {
    output[0] = (byte)((num >> 24) & 0xFF);  // Most significant byte
    output[1] = (byte)((num >> 16) & 0xFF);
    output[2] = (byte)((num >> 8 ) & 0xFF);
    output[3] = (byte)((num >> 0 ) & 0xFF);          // Least significant byte
}