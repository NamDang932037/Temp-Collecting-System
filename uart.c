#include "uart.h"

void USART_SendChar(char data){
	
	USART2->DR = 0x00;
	USART_SendData(USART2, data);
	//TxE = 1: Data is transferred to the shift register)
	//TxE = 0; Data is not transferred to the shift register
	while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
	
	
}

void USART_SendString(char *Str)
{
    while(*Str){
        USART_SendChar(*Str);
        Str++;
    }
}

uint16_t USART_GetChar(void)
{
	// Wait until data is received
	while (!USART_GetFlagStatus(USART2,USART_FLAG_RXNE));//cho den khi nhan due lieu xong
	// Read received char
	return USART_ReceiveData(USART2);
}
void USART_SendNum(USART_TypeDef *USARTx, uint16_t num, uint16_t len) {
	uint16_t i;
	char Num[len];
	uint8_t temp;
	for(i=0;i<len;i++) {
			temp = num%10;
			num /= 10;
			Num[i] = temp;
	}
	for(i=0; i<len;i++) {
			USART_SendChar(Num[len-1-i]+48);
	}
}


