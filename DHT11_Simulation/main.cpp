/*
 * LCDSimt.cpp
 *
 * Created: 6/3/2021 11:21:41 PM
 * Author : Dell
 */ 
#ifndef F_CPU
#define F_CPU 1000000UL
#endif
#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <math.h>
#include "lcd.h"
#define DHT11_PIN 3

int c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;

void Request()
{
	DDRB |= (1<<DHT11_PIN);
	PORTB &= ~(1<<DHT11_PIN);
	_delay_ms(20);
	PORTB |= (1<<DHT11_PIN);
}
void Response()
{
	DDRB &= ~(1<<DHT11_PIN);
	while (PINB & (1<<DHT11_PIN))
	{
	}
	while( (PINB & (1<<DHT11_PIN))==0 );
	while (PINB & (1<<DHT11_PIN));
	{
	}
}

int Receive_Data()
{
	for (int k=0;k<8;k++)
	{
		while( (PINB & (1<<DHT11_PIN))==0 );
		_delay_ms(30);
		if(PINB &  (1<<DHT11_PIN) )
			c=(c<<1) | (0x01);
		else
			c= (c<<1);
		while (PIND & (1<<DHT11_PIN) );
		
	}
	return c;
}

int main(void)
{
	//long int result;
	DDRD = 0xFF;
	char data[5];
	Lcd4_Init();
	Lcd4_Set_Cursor(1,1);
	Lcd4_Write_String("Humidity=");
	Lcd4_Set_Cursor(2,1);
	Lcd4_Write_String("Temp=");
	while (1) 
    {
		Request();
		Response();
		I_RH = Receive_Data();
		D_RH = Receive_Data();
		I_Temp = Receive_Data();
		D_Temp = Receive_Data();
		CheckSum = Receive_Data();
		if( (I_RH+D_RH+I_Temp+D_Temp)!=CheckSum )
		{
			Lcd4_Set_Cursor(1,0);
			Lcd4_Write_String("Error");
		}
		else
		{
			itoa(I_RH,data,10);
			Lcd4_Set_Cursor(11,1);
			Lcd4_Write_String(data);
			Lcd4_Write_String(".");
			
			itoa(D_RH,data,10);
			Lcd4_Write_String(data);
			Lcd4_Write_String("%");
			
			itoa(I_Temp,data,10);
			Lcd4_Set_Cursor(6,1);
			Lcd4_Write_String(data);
			Lcd4_Write_String("C");
			
			itoa(CheckSum,data,10);
			Lcd4_Write_String(data);
			Lcd4_Write_String(" ");
		}
		_delay_ms(10);
    }
	return 0;
}

