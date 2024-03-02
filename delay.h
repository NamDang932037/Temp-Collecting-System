#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f4xx.h"
//Khai bao cac ham
void TIM3_Init(void);
void Delay_ms(uint32_t dem);
void Delay_us(uint32_t dem);
void ConfigureSystemClock(void);

#endif 