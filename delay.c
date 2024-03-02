//Delay using TIM3
#include "delay.h"

void TIM3_Init(void) {
	//ABP1 Timer Clocks = 100MHz
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	TIM3-> PSC = 99; //CLK = SYSCLK/(PCS+1) = 100Mhz/100 = 1MHz = 10^-6s
	TIM3-> ARR = 0xFFFF;  
	TIM3-> CNT = 0;
	TIM3-> EGR = 1; //Update Register
	TIM3-> SR = 0; //Clear OVL Flag		
	TIM3-> CR1 = 1; //Enable timer
}
void Delay_ms(uint32_t dem) {
	for(uint16_t i = 0; i< dem; i++) {
			Delay_us(1000);
	}
}	

void Delay_us(uint32_t dem) {
	TIM3->CNT = 0;
	while(TIM3->CNT < dem);
}


void ConfigureSystemClock(void) {
	//Configure 100Mhz with 8Mhz HSE clock by cystal 
	#define PLL_M  4
	#define PLL_N  100
	#define PLL_P  0 //PLLP = 2
	//Enable HSE and wait for it to ready
	RCC->CR |= RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY));
	//Set the Power Enable Clock and Voltage Regulator
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	PWR->CR |= PWR_CR_VOS;
  //Configure the Flash Perfetch and the latency related settings
	FLASH->ACR |= FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_3WS;
	//Configure the prescaler HCLK,PCLK1,PCLK2
	//AHB prescaler 
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	//AHB1 perscaler
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
	//AHB2 prescaler
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
	//Configure main PLL
	RCC->PLLCFGR |= (PLL_M << 0)|(PLL_N <<6) |(PLL_P << 16)|(RCC_PLLCFGR_PLLSRC_HSE);
	//Enable PLL
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));
	//Select the clk src and wait for it to be set
	RCC->CFGR |= RCC_CFGR_SW_HSE;
	while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

