#include <Interrupts/ISRs/ISRs.h>

#include "Time.h"

void sleep(unsigned int Seconds){
    volatile unsigned int start_time = PIT_Seconds;
    while(PIT_Seconds < start_time + Seconds);
}

//precice sleep (millisecond)
void psleep(unsigned int Milliseconds){
    volatile unsigned int Start_Tick = PIT_Ticks;
    volatile unsigned int Target_Tick = ((Start_Tick + Milliseconds) % 2000);

    if (Target_Tick > Start_Tick) {
        while (PIT_Ticks < Target_Tick);
    } else {
        while (PIT_Ticks >= Start_Tick || PIT_Ticks < Target_Tick);
    }
}


int Get_Uptime(void){
    return (int)PIT_Seconds;
}

