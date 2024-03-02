#include "timer.h"

void TIM5_ISR_Init(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// Enable TIM4 clocks
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

	// Time base configuration
	TIM_TimeBaseStructure.TIM_Period = 50000 - 1; // 0.2 Hz (default = 5s)
	TIM_TimeBaseStructure.TIM_Prescaler = 10000 - 1; // Down to 1 KHz
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	// TIM Interrupts enable
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);
	// Configure the NVIC
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// TIM4 enable counter
	TIM_Cmd(TIM5, ENABLE);
}

