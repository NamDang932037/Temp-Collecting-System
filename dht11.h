#ifndef __DHT11_H
#define __DHT11_H

#include "stm32f4xx.h"

//define
#define DHT11_Port GPIOA
#define DHT11_Pin  GPIO_Pin_4

//Khai bao cac ham
void DHT11_Start(void);
void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
uint8_t DHT11_CheckRespose(void);
uint8_t DHT11_Read(void);

#endif 