#include "IDE.h"

byte IDE_Init(IO IDE_IO){
    return IDE_IO.Device.bus;
}