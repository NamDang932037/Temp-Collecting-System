#include "main.h"

void GPIO_Test_Config(void);
//USART Variables
volatile uint8_t buffercounter = 0;
extern uint8_t UART2_DMA_Val[0];
volatile uint8_t ReceivedData[15];
volatile uint8_t SendData[17] = {0x2D,0,0,0,0,0,0,0x2D,0,0,0,0,0,0,0x2D,0x87,'*'};
volatile uint8_t UART2_Rx_Flag = 0;
volatile uint16_t LM35a_Lim_Temp_L = 25,LM35a_Lim_Temp_H = 35,LM35b_Lim_Temp_L = 25,LM35b_Lim_Temp_H = 35;
volatile uint8_t Sample_Time = 5,LM35a_Temp_MSB,LM35a_Temp_LSB,LM35b_Temp_MSB,LM35b_Temp_LSB,
LM35a_TempErr_MSB,LM35a_TempErr_LSB,LM35b_TempErr_MSB,LM35b_TempErr_LSB;
volatile uint16_t LM35a_Pre_Val,LM35b_Pre_Val,LM35a_Pre_ValErr,LM35b_Pre_ValErr;
volatile uint16_t LM35a_LastCorrectVal,LM35b_LastCorrectVal;
volatile uint8_t LM35a_Alarm_H,LM35a_Alarm_L,LM35b_Alarm_H,LM35b_Alarm_L,LM35a_Dis,LM35b_Dis,LM35a_Alarm=0,LM35b_Alarm=0;
volatile uint32_t New_Prescaler,New_Period;
volatile uint8_t LastTranferStatus = 0xFF;
//ADC_DMA Variables
extern uint16_t ADC1_Channel5_Val[2];
extern float LM35_Val = 0;
volatile uint8_t Rh_Byte1,Rh_Byte2,Temp_Byte1,Temp_Byte2,Presence;
volatile uint16_t TEMP,RH,SUM;
volatile double Arr[15],tam,SubArr[25];
//EXTI 
volatile uint8_t Display = 0;

//LCD Custom char
uint8_t plusminus[8] = {0x00,0x04,0x04,0x1F,0x04,0x04,0x00,0x1F};

int main(void)
{
	//Configuration
	ConfigureSystemClock();
	TIM3_Init();
	UART2_DMA_Init();
	GPIO_Test_Config();
	I2C1_Init();
	LCD_Init();
	LCD_Clear();
	ADC1_DMA_Init();
	LCD_Create_Custom_Char(0,plusminus);
	

	//Display
	LCD_Gotoxy(0,0);
	LCD_Send_String("TEMP COLLECT SYSTEM");
	LCD_Gotoxy(0,1);
	LCD_Send_String("By: Ban Neo Giau Ten");
	LCD_Gotoxy(3,2);
	LCD_Send_String("GUI QT SYSTEM");
	LCD_Gotoxy(0,3);
	LCD_Send_String("By: Quy Ngai Duvanh");
	USART_SendString("TEMP CONTROL SYSTEM\n");
	USART_SendString("By: Ban Neo Giau Ten\n");
	USART_SendString("GUI QT SYSTEM\n");
	USART_SendString("By: Quy Ngai Duvanh\n");
	Delay_ms(2000);
	
	Display = 0;
	LCD_Clear();
	LCD_Gotoxy(0,0);
	LCD_Send_String("LM35a:");
	LCD_Gotoxy(9,0);
	LCD_Send_String(".");
	LCD_Write_Custom_Char(12,0,0);
	LCD_Gotoxy(15,0);
	LCD_Send_String(".");
	LCD_Gotoxy(18,0);
	LCD_Data_Write(0xDF);
	LCD_Send_String("C");

	LCD_Gotoxy(0,1);
	LCD_Send_String("Hi_Lim: ");
	LCD_Gotoxy(11,1);
	LCD_Send_String("L_Lim: ");
	
	LCD_Gotoxy(0,2);
	LCD_Send_String("LM35b:");
	LCD_Gotoxy(9,2);
	LCD_Send_String(".");
	LCD_Write_Custom_Char(12,2,0);
	LCD_Gotoxy(15,2);
	LCD_Send_String(".");
	LCD_Gotoxy(18,2);
	LCD_Data_Write(0xDF);
	LCD_Send_String("C");

	LCD_Gotoxy(0,3);
	LCD_Send_String("Hi_Lim: ");
	LCD_Gotoxy(11,3);
	LCD_Send_String("L_Lim: ");
	PD2_Exti_Init();
	TIM5_ISR_Init();
  while (1)
  {
		GPIO_SetBits(GPIOD,GPIO_Pin_15);
		Delay_ms(500);
		GPIO_ResetBits(GPIOD,GPIO_Pin_15);
		Delay_ms(500);
  }
}
void EXTI2_IRQHandler(void) {
		if(EXTI_GetITStatus(EXTI_Line2) != RESET){
        EXTI_ClearITPendingBit(EXTI_Line2);
        Delay_ms(50);
				if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_2) == 0) {
						Display+=1;
						if(Display > 2) {Display = 0;}
						
						if(Display == 1) {
								//DHT11 Display
								LCD_Clear();
								LCD_Gotoxy(0,0);
								LCD_Send_String("DHT11 Sub TempSensor");
								LCD_Gotoxy(1,1);
								LCD_Send_String("TEMP:");
								if(Presence == 0xFF) {
										LCD_Send_String("NA.NA");
								}
								else {
										LCD_Number(Temp_Byte1,2);
										LCD_Send_String(".");
										LCD_Number(Temp_Byte2*10,2);
								}
								LCD_Write_Custom_Char(12,1,0);
								LCD_Gotoxy(13,1);
								LCD_Send_String("2.00");
								LCD_Data_Write(0xDF);
								LCD_Send_String("C");
								
								LCD_Gotoxy(2,2);
								LCD_Send_String("HUM:");
								if(Presence == 0xFF) {
										LCD_Send_String("NA.NA");
								}
								else {
										LCD_Number(Rh_Byte1,2);
										LCD_Send_String(".");
										LCD_Number(Rh_Byte2,2);
								}
								LCD_Write_Custom_Char(12,2,0);
								LCD_Send_String("5.00%");
								
								LCD_Gotoxy(0,3);
								LCD_Send_String("CONNECT STATE:");
								LCD_Gotoxy(15,3);
								if(Presence == 0xFF) {
									LCD_Send_String("OFF");
								}
								else LCD_Send_String("ON");
						}
						if(Display == 0){
								//LM35 Display
								LCD_Clear();
								LCD_Gotoxy(0,0);
								LCD_Send_String("LM35a:");
								LCD_Gotoxy(7,0);
								if(LM35a_Dis == 0) {	
									LCD_Number(LM35a_Temp_MSB,2);
									LCD_Send_String(".");
									LCD_Number(LM35a_Temp_LSB,2);
									LCD_Write_Custom_Char(12,0,0);
									LCD_Gotoxy(13,0);
									LCD_Number(LM35a_TempErr_MSB,1);
									LCD_Send_String(".");
									LCD_Number(LM35a_TempErr_LSB,2);
									LCD_Gotoxy(18,0);
									LCD_Data_Write(0xDF);
									LCD_Send_String("C");
								} else {
									LCD_Send_String("DISCONNECTED ");
								}
								LCD_Gotoxy(0,1);
								LCD_Send_String("Hi_Lim: ");
								LCD_Gotoxy(7,1);
								if(LM35a_Alarm == 1) {
									LCD_Send_String(" ");
									LCD_Number(LM35a_Lim_Temp_H,2);
								} else {
									LCD_Send_String("OFF");
								}
								LCD_Gotoxy(11,1);
								LCD_Send_String("L_Lim: ");
								LCD_Gotoxy(17,1);
								if(LM35a_Alarm == 1) {
									LCD_Send_String(" ");
									LCD_Number(LM35a_Lim_Temp_L,2);
								} else {
									LCD_Send_String("OFF");
								}
								
							
								LCD_Gotoxy(0,2);
								LCD_Send_String("LM35b:");
								LCD_Gotoxy(7,2);
								if(LM35b_Dis == 0) {
									LCD_Number(LM35b_Temp_MSB,2);
									LCD_Send_String(".");
									LCD_Number(LM35b_Temp_LSB,2);
									LCD_Write_Custom_Char(12,2,0);
									LCD_Gotoxy(13,2);
									LCD_Number(LM35b_TempErr_MSB,1);
									LCD_Send_String(".");
									LCD_Number(LM35b_TempErr_LSB,2);
									LCD_Gotoxy(18,2);
									LCD_Data_Write(0xDF);
									LCD_Send_String("C");
								} else {
									LCD_Send_String("DISCONNECTED ");
								}
								LCD_Gotoxy(0,3);
								LCD_Send_String("Hi_Lim: ");
								LCD_Gotoxy(7,3);
								if(LM35b_Alarm == 1) {
									LCD_Send_String(" ");
									LCD_Number(LM35b_Lim_Temp_H,2);
								} else {
									LCD_Send_String("OFF");
								}
								LCD_Gotoxy(11,3);
								LCD_Send_String("L_Lim: ");
								LCD_Gotoxy(17,3);
								if(LM35b_Alarm == 1) {
									LCD_Send_String(" ");
									LCD_Number(LM35b_Lim_Temp_L,2);
								} else {
									LCD_Send_String("OFF");
								}	
						} 
						if(Display == 2) {
								LCD_Clear();
								LCD_Gotoxy(1,0);
								LCD_Send_String("SYSTEM MANAGEMENT");
								LCD_Gotoxy(0,1);
								LCD_Send_String("Sample Time: ");
								LCD_Number(Sample_Time,2);
								LCD_Send_String(" Sec");
								LCD_Gotoxy(0,2);
								LCD_Send_String("Last Tranfer: ");
								LCD_Gotoxy(13,2);
								if(LastTranferStatus == 0xFF) {
										LCD_Send_String(" N/A");
								}  
								if(LastTranferStatus == 0) {
										LCD_Send_String(" Error ");
								}  
								if(LastTranferStatus == 1) {
										LCD_Send_String("Success");
								}
								LCD_Gotoxy(0,3);
								LCD_Send_String("Baud:9600|Mode:8 Bit");
						}
				}
		}	
}

void GPIO_Test_Config() {
	GPIO_InitTypeDef    GPIO_InitStructure;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOC, ENABLE ); 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	

}
void DMA1_Stream5_IRQHandler(void) {
	if(DMA_GetFlagStatus(DMA1_Stream5,DMA_FLAG_TCIF5) != RESET) {
		DMA_ClearFlag(DMA1,DMA_FLAG_TCIF5);
		if(buffercounter < 15) {
//			if((UART2_DMA_Val[0] >= 0x30) || (UART2_DMA_Val[0] <= 0x39)) {
//				ReceivedData[buffercounter] = UART2_DMA_Val[0]-0x30;
//			}
			/*else*/ ReceivedData[buffercounter] = UART2_DMA_Val[0];
		}
		buffercounter++;
		if(UART2_DMA_Val[0] == '*') {
			buffercounter = 0;
			UART2_Rx_Flag = 1;
		}
		if(UART2_Rx_Flag) {
			UART2_Rx_Flag = 0;
			//Check sum for error or check connection
			if((ReceivedData[0] + ReceivedData[7] + ReceivedData[12]) != ReceivedData[13]) {
					if((ReceivedData[0]== 50) & (ReceivedData[7]==0xFF) & (ReceivedData[12] ==0xFF) & (ReceivedData[13]== 0xFF)) {
							//Check connection
							USART_SendData(USART2,50);
							ReceivedData[0] = 0xFF;
					}
					else {
						//Error connection
						LastTranferStatus = 0;
						if(Display == 2) {
								LCD_Gotoxy(13,2);
								LCD_Send_String(" Error ");
						}
					
						//Buzzer Alarm
						GPIO_SetBits(GPIOC,GPIO_Pin_13);
						Delay_ms(50);
						GPIO_ResetBits(GPIOC,GPIO_Pin_13);
						Delay_ms(50);
						GPIO_SetBits(GPIOC,GPIO_Pin_13);
						Delay_ms(50);
						GPIO_ResetBits(GPIOC,GPIO_Pin_13);
						Delay_ms(50);
						GPIO_SetBits(GPIOC,GPIO_Pin_13);
						Delay_ms(50);
						GPIO_ResetBits(GPIOC,GPIO_Pin_13);
						Delay_ms(50);
				 }
			}
			else {
					
					//No Error connection
					LastTranferStatus = 1;
					if(Display == 2) {
					LCD_Gotoxy(13,2);
					LCD_Send_String("Success");
					}
					//Sample time change
					if(ReceivedData[1] == 1) {
							//Change sample time
							Sample_Time = ReceivedData[2];
							if(Display == 2) {
								LCD_Gotoxy(13,1);
								LCD_Number(Sample_Time,2);
							}
							TIM_Cmd(TIM5,DISABLE);
							switch (ReceivedData[2]) {
								case 5: {
									New_Prescaler = 10000;
									New_Period = 50000;
									break;
								}
								case 10: {
									New_Prescaler = 20000;
									New_Period = 50000;
									break;
								}
								case 30: {
									New_Prescaler = 60000;
									New_Period = 50000;
									break;
								}	
								case 60: {
									New_Prescaler = 60000;
									New_Period = 100000;
									break;
								}	
								default: break;
						}
							TIM5-> CNT = 0;
							TIM5 -> ARR = New_Period;
							TIM5->PSC =New_Prescaler;
							TIM_Cmd(TIM5,ENABLE);
					}
					//LM35a Control
					if(ReceivedData[3] == 1) {
							//New Cmd for LM35a
							LM35a_Alarm = ReceivedData[4];
							LM35a_Lim_Temp_H = ReceivedData[5];
							LM35a_Lim_Temp_L = ReceivedData[6];
					}
					//LM35b Control
					if(ReceivedData[8] == 1) {
						//New Cmd for LM35a
							LM35b_Alarm = ReceivedData[9];
							LM35b_Lim_Temp_H = ReceivedData[10];
							LM35b_Lim_Temp_L = ReceivedData[11];
					}
			}
		
			
			for(uint8_t i  = 0; i < 15; i++) {
				ReceivedData[i] = 0xFF;
			}
		}
	}
}
void TIM5_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
				
				//LM35a
				tam = 0;
        float ADC_Val=0;
				float tam1;
				uint8_t i,j;
				for(i = 0; i < 25; i++) {
						SubArr[i] = (float)ADC1_Channel5_Val[0]*300/4095;
						//ADC_Val+= Arr[i];
						Delay_ms(20);
				}
				//Soft 
				for(i = 0; i < 25 - 1; i++) {
					for(j = i + 1; j < 25; j++) {
						if(SubArr[i] > SubArr[j]) {
							tam = SubArr[i];
							SubArr[i] = SubArr[j];
							SubArr[j] = tam;
						}
					}
				}
				//Take medium Value
				for(i = 0; i < 15; i++) {
					Arr[i] = SubArr[i+5];
					ADC_Val+= Arr[i];
				}
				tam = 0;
				LM35_Val = ADC_Val/15; //Mean
				if(LM35a_Pre_Val == 0) {
						LM35a_Pre_Val = LM35_Val;
				}
				if(fabs(LM35a_Pre_Val-LM35_Val)<= 5) {
					LM35a_Dis = 0;
					LM35a_Pre_Val = LM35_Val;
					LM35a_LastCorrectVal = LM35_Val;
					LM35a_Temp_MSB = (uint8_t)LM35_Val; //So truoc dau phay
					for(i = 0;i < 15; i++) {
							Arr[i] = fabs(Arr[i] - LM35_Val);
							Arr[i]*=Arr[i];
							tam += Arr[i];		
					}
					tam = sqrt(tam/14); //sigma
					tam = tam/sqrt(15); //alpha
					tam*=2; //2 alpha with truth probability 95.4%
					//Mean value

					tam1 = LM35_Val;
					tam1 = (tam1 - (uint16_t)tam1)*100;
					LM35a_Temp_LSB = (uint8_t)tam1;

					//Error
					tam*=100;
					tam1 = (uint16_t)(tam)/100;
					LM35a_TempErr_MSB = (uint8_t)tam1;

					tam1 = (uint16_t)tam%100;
					LM35a_TempErr_LSB = (uint8_t)tam1;

					
					if(Display == 0) {
						LCD_Gotoxy(7,0);
						LCD_Send_String("             ");
						LCD_Gotoxy(7,0);
						LCD_Number(LM35a_Temp_MSB,2);
						LCD_Send_String(".");
						LCD_Gotoxy(10,0);
						LCD_Number(LM35a_Temp_LSB,2);
						LCD_Write_Custom_Char(12,0,0);
						LCD_Gotoxy(13,0);
						LCD_Number(LM35a_TempErr_MSB,1);
						LCD_Send_String(".");
						LCD_Gotoxy(15,0);
						LCD_Number(LM35a_TempErr_LSB,2);
						LCD_Gotoxy(18,0);
						LCD_Data_Write(0xDF);
						LCD_Send_String("C");
						
						LCD_Gotoxy(7,1);
						if(LM35a_Alarm == 1) {
									LCD_Send_String(" ");
									LCD_Number(LM35a_Lim_Temp_H,2);
								} else {
									LCD_Send_String("OFF");
								}
						LCD_Gotoxy(17,1);
						if(LM35a_Alarm == 1) {
									LCD_Send_String(" ");
									LCD_Number(LM35a_Lim_Temp_L,2);
								} else {
									LCD_Send_String("OFF");
								}
						
					}

					if(LM35a_Alarm == 1) {
							if(LM35_Val < LM35a_Lim_Temp_L) {
									//Led Alarm
									GPIO_SetBits(GPIOA,GPIO_Pin_7);
									//Buzzer Alarm
									GPIO_SetBits(GPIOC,GPIO_Pin_13);
									//Alarm byte send to PC
									LM35a_Alarm_L = 1;
							}
							if(LM35_Val > LM35a_Lim_Temp_H) {
									//Led Alarm
									GPIO_SetBits(GPIOA,GPIO_Pin_8);
									//Buzzer Alarm
									GPIO_SetBits(GPIOC,GPIO_Pin_13);
									//Alarm byte send to PC
									LM35a_Alarm_H = 1;
							}
							if((LM35_Val < LM35a_Lim_Temp_H)&&(LM35_Val > LM35a_Lim_Temp_L)) {
									//Led off
									GPIO_ResetBits(GPIOA,GPIO_Pin_7|GPIO_Pin_8);
									GPIO_ResetBits(GPIOC,GPIO_Pin_13);
									LM35a_Alarm_H = LM35a_Alarm_L = 0;
							}
						}
					}	
						else {
							if(((LM35a_LastCorrectVal - 2) > LM35_Val) || ((LM35a_LastCorrectVal + 2) < LM35_Val)) {
								LM35a_Dis = 1;
								if(Display == 0) {
								LCD_Gotoxy(7,0);
								LCD_Send_String("DISCONNECTED ");
								}
							}
						}
				ADC_Val = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////				
				//LM35b
				tam = 0;
				for(i = 0; i < 25; i++) {
						SubArr[i] = (float)ADC1_Channel5_Val[1]*300/4095;
						//ADC_Val+= Arr[i];
						Delay_ms(20);
				}
				//Soft 
				for(i = 0; i < 25 - 1; i++) {
					for(j = i + 1; j < 25; j++) {
						if(SubArr[i] > SubArr[j]) {
							tam = SubArr[i];
							SubArr[i] = SubArr[j];
							SubArr[j] = tam;
						}
					}
				}
				//Take medium Value
				for(i = 0; i < 15; i++) {
					Arr[i] = SubArr[i+5];
					ADC_Val+= Arr[i];
				}
				tam = 0;
				LM35_Val = ADC_Val/15; //Mean
				if(LM35b_Pre_Val == 0) {
						LM35b_Pre_Val = LM35_Val;
						
				}
				if(fabs(LM35b_Pre_Val-LM35_Val)<= 5) {
					LM35b_Dis = 0;
					LM35b_Pre_Val = LM35_Val;
					LM35b_LastCorrectVal = LM35_Val;
					LM35b_Temp_MSB = (uint8_t)LM35_Val; //So truoc dau phay
					for(i = 0;i < 15; i++) {
							Arr[i] = fabs(Arr[i] - LM35_Val);
							Arr[i]*=Arr[i];
							tam += Arr[i];		
					}
					tam = sqrt(tam/14); //sigma
					tam = tam/sqrt(15); //alpha
					tam*=2; //2 alpha with truth probability 95.4%
					//Mean value
					tam1 = LM35_Val;
					tam1 = (tam1 - (uint16_t)tam1)*100;
					LM35b_Temp_LSB = (uint8_t)tam1;
					
					//Error
					tam*=100;
					tam1 = (uint16_t)(tam)/100;
					LM35b_TempErr_MSB = (uint8_t)tam1;

					tam1 = (uint16_t)tam%100;
					LM35b_TempErr_LSB = (uint8_t)tam1;
					
					if(Display == 0) {
						LCD_Gotoxy(7,2);
						LCD_Send_String("             ");
						LCD_Gotoxy(7,2);
						LCD_Number(LM35b_Temp_MSB,2);
						LCD_Send_String(".");
						LCD_Gotoxy(10,2);
						LCD_Number(LM35b_Temp_LSB,2);
						LCD_Write_Custom_Char(12,2,0);
						LCD_Gotoxy(13,2);
						LCD_Number(LM35b_TempErr_MSB,1);
						LCD_Send_String(".");
						LCD_Gotoxy(15,2);
						LCD_Number(LM35b_TempErr_LSB,2);
						LCD_Gotoxy(18,2);
						LCD_Data_Write(0xDF);
						LCD_Send_String("C");
						
						LCD_Gotoxy(7,3);
						if(LM35b_Alarm == 1) {
									LCD_Send_String(" ");
									LCD_Number(LM35b_Lim_Temp_H,2);
								} else {
									LCD_Send_String("OFF");
									
								}
						LCD_Gotoxy(17,3);
						if(LM35b_Alarm == 1) {
									LCD_Send_String(" ");
									LCD_Number(LM35b_Lim_Temp_L,2);
								} else {
									LCD_Send_String("OFF");
									
								}
						
					}
					if(LM35b_Alarm == 1) {
							if(LM35b_Temp_MSB < LM35b_Lim_Temp_L) {
									//Led Alarm
									GPIO_SetBits(GPIOA,GPIO_Pin_9);
									//Buzzer Alarm
									GPIO_SetBits(GPIOC,GPIO_Pin_13);
									//Alarm byte send to PC
									LM35b_Alarm_L = 1;
							}
							if(LM35b_Temp_MSB > LM35b_Lim_Temp_H) {
									//Led Alarm
									GPIO_SetBits(GPIOA,GPIO_Pin_10);
									//Buzzer Alarm
									GPIO_SetBits(GPIOC,GPIO_Pin_13);
									//Alarm byte send to PC
									LM35b_Alarm_H = 1;
							}
							if((LM35b_Temp_MSB < LM35b_Lim_Temp_H)&&(LM35b_Temp_MSB > LM35b_Lim_Temp_L)) {
									//Led off
									GPIO_ResetBits(GPIOA,GPIO_Pin_9|GPIO_Pin_10);
									GPIO_ResetBits(GPIOC,GPIO_Pin_13);
									//Alarm bytes off
									LM35b_Alarm_L = LM35b_Alarm_H = 0;
							}
						}
					}
						else {
							if(((LM35b_LastCorrectVal - 2) > LM35_Val) || ((LM35b_LastCorrectVal + 2) < LM35_Val)) {
								LM35b_Dis = 1;
								if(Display == 0) {
									LCD_Gotoxy(7,2);
									LCD_Send_String("DISCONNECTED ");
								}
							}
						}
			
				
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////			
				//DHT11
				DHT11_Start();
				Presence = DHT11_CheckRespose();
				if(Presence == 0xFF) {
						if(Display == 1) {
								LCD_Gotoxy(6,1);
								LCD_Send_String("NA");
								LCD_Send_String(".");
								LCD_Send_String("NA");
								
								LCD_Gotoxy(6,2);
								LCD_Send_String("NA");
								LCD_Send_String(".");
								LCD_Send_String("NA");
								
								LCD_Gotoxy(15,3);
								LCD_Send_String("OFF");
								}
				}
				
				else if(Presence == 1) {
					Rh_Byte1 = DHT11_Read();
					Rh_Byte2 = DHT11_Read();
					Temp_Byte1 = DHT11_Read();
					Temp_Byte2 = DHT11_Read();
					SUM = DHT11_Read();
				
					TEMP = Temp_Byte1;
					RH = Rh_Byte1;
					
					if(Display == 1) {
							LCD_Gotoxy(6,1);
							LCD_Number(Temp_Byte1,2);
							LCD_Send_String(".");
							LCD_Number(Temp_Byte2*10,2);
							LCD_Send_String(" ");
							LCD_Gotoxy(6,2);
							LCD_Number(Rh_Byte1,2);
							LCD_Send_String(".");
							LCD_Number(Rh_Byte2,2);
							LCD_Send_String(" ");
							LCD_Gotoxy(15,3);
							LCD_Send_String("ON ");
					}
				}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					
				//Send data to PC
				//0: 0x2D
				if(LM35a_Dis == 0) {
					SendData[1] = LM35a_Temp_MSB;
					SendData[2] = LM35a_Temp_LSB;
					SendData[3] = LM35a_TempErr_MSB;
					SendData[4] = LM35a_TempErr_LSB;
					SendData[5] = LM35a_Alarm_H;
					SendData[6] = LM35a_Alarm_L;
				}
				else {
					SendData[1] = 0xFF;
					SendData[2] = 0xFF;
					SendData[3] = 0xFF;
					SendData[4] = 0xFF;
					SendData[5] = 0;
					SendData[6] = 0;
				}
				//7: 0x2D
				
				if(LM35b_Dis == 0) {
					SendData[8] = LM35b_Temp_MSB;
					SendData[9] = LM35b_Temp_LSB;
					SendData[10] = LM35b_TempErr_MSB;
					SendData[11] = LM35b_TempErr_LSB;
					SendData[12] = LM35b_Alarm_H;
					SendData[13] = LM35b_Alarm_L;
				}
				else {
					SendData[8] = 0xFF;
					SendData[9] = 0xFF;
					SendData[10] = 0xFF;
					SendData[11] = 0xFF;
					SendData[12] = 0;
					SendData[13] = 0;
				}
				//14: 0x2D
				//15: Check sum = 0x87
				//16: * end 
				for(i = 0; i < 17; i++) {
					USART_SendChar(SendData[i]);
				}
    }
}

void Check_Connection() {
		
	
}
