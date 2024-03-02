#include "i2clcd.h"
//I2C1_SDA: PB9
//I2C1_SCL: PB8
void I2C1_Init(void) {
	GPIO_InitTypeDef 					GPIO_InitStructure;
	I2C_InitTypeDef						I2C_InitStructure; 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);		
	// su dung kenh I2C1 cua STM32
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	//GPIO Configure
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	//Connect AF to GPIO
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_I2C1);
	//I2C Configure
	I2C_DeInit(I2C1);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x01; //Dia chi cua Master
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable; //Disable Ack bit
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;      
	I2C_InitStructure.I2C_ClockSpeed = 100000;	//Toc do truyen									
	I2C_Init(I2C1, &I2C_InitStructure);
	I2C_Cmd(I2C1, ENABLE);
	
	
}
uint8_t I2C1_Receive_Byte(uint8_t address) {
	/* Send START condition */
	uint8_t I2C_DataReceive;
	I2C_GenerateSTART(I2C1, ENABLE);
	I2C_ClearFlag(I2C1,I2C_FLAG_BUSY);
	/* Test on EV5 and clear it */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	/* Send PCF8574 address for write */
	I2C_Send7bitAddress(I2C1, address, I2C_Direction_Receiver);
	/* Test on EV6 and clear it */
	while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	I2C_Cmd(I2C1,ENABLE);
	/* Send the byte to be written */
	I2C_DataReceive = I2C_ReceiveData(I2C1);
	/* Test on EV8 and clear it */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED));
	/* Send STOP condition */
	I2C_GenerateSTOP(I2C1, ENABLE);	
	return  I2C_DataReceive;
}

void I2C_Receive_Multi(uint8_t address,uint8_t *buffer, uint8_t len) {
		
	I2C_GenerateSTART(I2C1, ENABLE);
	/* Test on EV5 and clear it */
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
	/* Send PCF8574 address for write */
	I2C_Send7bitAddress(I2C1, address, I2C_Direction_Receiver);
/* Test on EV6 and clear it */
	while (!I2C_CheckEvent(I2C1,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
	I2C_Cmd(I2C1,ENABLE);

	while(len) {
			if(len==1) {
					I2C_AcknowledgeConfig(I2C1,DISABLE);
					I2C_GenerateSTOP(I2C1,ENABLE);
			}
			if((I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))) {
					*buffer = I2C_ReceiveData(I2C1);
					buffer++;
					len--;
			}
	}
	I2C_AcknowledgeConfig(I2C1,ENABLE);
}

void LCD_Write_byte(uint8_t data) {
	I2C_ClearFlag(I2C1,I2C_FLAG_BUSY);
	I2C_GenerateSTART(I2C_Chanel, ENABLE);
	/* Test on EV5 and clear it */
	while (!I2C_CheckEvent(I2C_Chanel, I2C_EVENT_MASTER_MODE_SELECT));
	/* Send PCF8574 address for write */
	I2C_Send7bitAddress(I2C_Chanel, PCF8574_Address, I2C_Direction_Transmitter);
/* Test on EV6 and clear it */
	while (!I2C_CheckEvent(I2C_Chanel, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
	I2C_Cmd(I2C1,ENABLE); // co hoac khong cung duoc
	/* Send the byte to be written */
	I2C_SendData(I2C_Chanel, data);
	/* Test on EV8_2 and clear it */
	while (!I2C_CheckEvent(I2C_Chanel, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	/* Send STOP condition */
	I2C_GenerateSTOP(I2C_Chanel, ENABLE);		
}

void LCD_Init (void)
{
	/* Set 4-bits interface */
	LCD_Control_Write(0x30);		 
	Delay_ms(10);
	LCD_Control_Write(0x30);		 
	Delay_ms(10);
	LCD_Control_Write(0x30);		 
	Delay_ms(10);
	LCD_Control_Write(0x33);		 
	Delay_ms(10);
	LCD_Control_Write(0x32);
	Delay_ms(50);
	/* Start to set LCD function */
	LCD_Control_Write(0x28);
		Delay_ms(50);	
	/* clear LCD */
	LCD_Control_Write(0x01);
		Delay_ms(60);
	/* wait 60ms */
	
	/* set entry mode */
	LCD_Control_Write(0x06);	Delay_ms(50);
	/* set display to on */	
	LCD_Control_Write(0x0C);	Delay_ms(50);
	/* move cursor to home and set data address to 0 */
	LCD_Control_Write(0x02);	Delay_ms(50);
}
void LCD_Data_Write(char data)
{
	char data_u, data_l;
	uint8_t data_t[4],i=0;
	data_u = data&0xf0;
	data_l = (data<<4)&0xf0;
	data_t[0] = data_u|0x0d;  //en=1, rs=0
	data_t[1] = data_u|0x09;  //en=0, rs=0
	data_t[2] = data_l|0x0d;  //en=1, rs=0
	data_t[3] = data_l|0x09;  //en=0, rs=0
	for(i = 0;i<4;i++)
	{
			 LCD_Write_byte(data_t[i]);
	}

}
void LCD_Control_Write(char data)
{
	char data_u, data_l;
	uint8_t data_t[4],i=0;
	data_u = data&0xf0;
	data_l = (data<<4)&0xf0;
	data_t[0] = data_u|0x04;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x04;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	for(i = 0;i<4;i++)
	{
			 LCD_Write_byte(data_t[i]);
	}

}
void LCD_Send_String (char *str)
{
		while (*str) LCD_Data_Write (*str++);
}
void Delete_LCD(void)
{
		LCD_Control_Write(0x01);
		Delay_ms(20);
}

void LCD_Gotoxy(uint8_t x, uint8_t y) {
	uint8_t address;
	switch(y) {
		case 0: address = 0x00; break;
		case 1: address = 0x40; break;
		case 2: address = 0x14; break;
		case 3: address = 0x54; break;
	}
	address+= x;
	LCD_Control_Write(0x80|address);
}

void LCD_Number(uint8_t num, uint8_t leng) {
	uint8_t i,temp;
	char kitu[leng];
	for(i = 0; i <leng; i++) {
			temp = num%10;
			num = num/10;
			kitu[i] = temp;
	}
	for(i = 0; i< leng; i++) {
			LCD_Data_Write(kitu[leng-1-i]+48);
	}
}


void LCD_Create_Custom_Char(uint8_t location, uint8_t* data_bytes)
{
	uint8_t i;
	// We only have 8 locations 0-7 for custom chars
	location &= 0x07; 
	// Set CGRAM address
	LCD_Control_Write(SET_CGRAM_ADDRESS|(location << 3));
	
	// Write 8 bytes custom char pattern
	for (i = 0; i < 8; i++) 
	{
		LCD_Data_Write(data_bytes[i]);
	}
}

void LCD_Write_Custom_Char( uint8_t x, uint8_t y, uint8_t location)
{
	LCD_Gotoxy(x,y);
	LCD_Data_Write(location);
}

void LCD_Clear(void) {
	LCD_Control_Write(0x01);
	Delay_ms(20);
}
void LCD_Clear_Screen(void) {
	uint8_t i;
	for(i = 0; i < 4; i++) {
			LCD_Gotoxy(i,0);
			LCD_Send_String("                    ");
	}
}