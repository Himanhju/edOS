#ifndef TIME_H
#define TIME_H

#include <Interrupts/ISRs/ISRs.h>

//sleeps a certain number of seconds
void sleep(unsigned int Seconds);

//sleeps a certain number of milliseconds
void psleep(unsigned int Milliseconds);

//returns the uptime of the IDT
int Get_Uptime(void);

#endif