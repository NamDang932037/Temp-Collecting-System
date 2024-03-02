#include "dht11.h"

//PA4 as DATA Pin of DHT11

void DHT11_Start(void) {
		Set_Pin_Output(DHT11_Port,DHT11_Pin);
		GPIO_WriteBit(DHT11_Port,DHT11_Pin,0);
		Delay_ms(20);
		GPIO_WriteBit(DHT11_Port,DHT11_Pin,1);
		Delay_us(30);
		Set_Pin_Input(DHT11_Port,DHT11_Pin);
}

void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
		GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
		GPIO_Init(GPIOx, &GPIO_InitStructure);
	
}

void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_Init(GPIOx, &GPIO_InitStructure);
}

uint8_t DHT11_CheckRespose(void) {
		uint8_t Respose;
		Delay_us(40);
		if(!GPIO_ReadInputDataBit(DHT11_Port,DHT11_Pin)) {
				Delay_us(80);
				if(GPIO_ReadInputDataBit(DHT11_Port,DHT11_Pin)) Respose = 1;
				else Respose = -1; 
		}
		Delay_us(3);
		if(GPIO_ReadInputDataBit(DHT11_Port,DHT11_Pin) == 1) {
				Delay_us(25);
				if(GPIO_ReadInputDataBit(DHT11_Port,DHT11_Pin) == 1) {
						return Respose = -1;
				}
		}
		while(GPIO_ReadInputDataBit(DHT11_Port,DHT11_Pin));
		return Respose;
}

uint8_t DHT11_Read(void) {
		uint8_t i,j;
		for(j = 0; j < 8; j++) {
				while(!GPIO_ReadInputDataBit(DHT11_Port,DHT11_Pin));
				Delay_us(40);
				if(!GPIO_ReadInputDataBit(DHT11_Port,DHT11_Pin)) {
						i&= ~(1<<(7-j));
				}
				else i|= (1<<(7-j));
				while(GPIO_ReadInputDataBit(DHT11_Port,DHT11_Pin));
				}
		return i;
		}