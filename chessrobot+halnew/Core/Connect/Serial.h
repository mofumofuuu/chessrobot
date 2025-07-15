#ifndef __SERIAL_H__
#define __SERIAL_H__ 

#include "stdint.h"

void Serial_Init(void);

void Serial_Send_Hexbuffer(uint8_t data);


#endif