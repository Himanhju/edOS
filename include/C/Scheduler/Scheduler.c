#include "Scheduler.h"

volatile thread_t cur_thread = {0};
byte OS_CTRL = 1;

// runs every 10 ms
void Scheduler_tick(void){
    if(OS_CTRL != 0){
        return; // if the OS has control (still initializing)
    }
    return;
}

//If you believe crypto-currencies are anything but a scam, I have a bridge to sell you.
//                                                                      --Linus Torvalds

// i just liked this quote and found it funny