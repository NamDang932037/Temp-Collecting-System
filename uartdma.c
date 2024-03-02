#include "uartdma.h"
//UART2 DMA: DMA1 - Stream 5 - Channel 4
#define SIZE_BUF 1
uint8_t UART2_DMA_Val[SIZE_BUF];
void UART2_DMA_Init(void) {
	GPIO_InitTypeDef    GPIO_InitStructure;
  USART_InitTypeDef   USART_InitStructure;
	DMA_InitTypeDef 		DMA_InitStructure;
	NVIC_InitTypeDef 		NVIC_InitStructure;
	
	
	/*enable clock*/
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE ); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE );
	
	// Connect PA2 to USART1_Tx
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	// Connect PA3 to USART1_Rx
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	/*RX DMA Configure*/
	DMA_DeInit(DMA1_Stream5);
	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART2->DR);
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&UART2_DMA_Val;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStructure.DMA_BufferSize = sizeof(UART2_DMA_Val);
	DMA_InitStructure.DMA_PeripheralInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
	DMA_Init(DMA1_Stream5,&DMA_InitStructure);
	DMA_Cmd(DMA1_Stream5,ENABLE);
	
	/*Configure PA2 and PA3 for USART2*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/*Configure USART2*/
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	

	// Enable the DMA1 Stream5 global Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	DMA_ITConfig(DMA1_Stream5,DMA_IT_TC,ENABLE);
	
	/*Connect UART2_RX to DMA*/
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	
	// Enable USART2
	USART_Cmd(USART2, ENABLE);
}

	