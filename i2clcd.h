#ifndef __I2CLCD_H
#define __I2CLCD_H

#include "stm32f4xx.h"
//Define
#define PCF8574_Address      			0x7E
#define I2C_Chanel      					I2C1
#define SET_CGRAM_ADDRESS	 	    	0x40
#define SET_DDRAM_ADDRESS	 	     	0x80

//Khai bao cac ham
void I2C1_Init(void);
void LCD_Write_byte(uint8_t data);
void LCD_Init (void);
void LCD_Data_Write(char data);
void LCD_Control_Write(char data);
void LCD_Send_String (char *str);
void LCD_Clear(void);
void LCD_Gotoxy(uint8_t x, uint8_t y);
void LCD_Number(uint8_t num, uint8_t leng);
void LCD_Create_Custom_Char(uint8_t location, uint8_t* data_bytes);
void LCD_Write_Custom_Char( uint8_t x, uint8_t y, uint8_t location);
void LCD_Clear_Screen(void);
#endif 