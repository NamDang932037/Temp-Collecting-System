#ifndef __UART_H
#define __UART_H

#include "stm32f4xx.h"

//Khai bao cac ham
uint16_t USART_GetChar(void);
void USART_SendString(char *Str);
void USART_SendChar(char data);

#endif 