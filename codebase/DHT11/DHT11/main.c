
#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
extern void lcd_backlight(char on);   //not in lcd.h

#include <stdlib.h>       //for itoa()
#include <stdio.h>
#define DHT11_PIN 6

uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;


void Request()						/* Microcontroller send start pulse or request */
{
	DDRD |= (1<<DHT11_PIN);
	PORTD &= ~(1<<DHT11_PIN);		/* set to low pin */
	_delay_ms(20);					/* wait for 20ms */
	PORTD |= (1<<DHT11_PIN);		/* set to high pin */
}

void Response()						/* receive response from DHT11 */
{
	DDRD &= ~(1<<DHT11_PIN);
	while(PIND & (1<<DHT11_PIN));
	while((PIND & (1<<DHT11_PIN))==0);
	while(PIND & (1<<DHT11_PIN));
}

uint8_t Receive_data()							/* receive data */
{	
	for (int q=0; q<8; q++)
	{
		while((PIND & (1<<DHT11_PIN)) == 0);	/* check received bit 0 or 1 */
		_delay_us(30);
		if(PIND & (1<<DHT11_PIN))				/* if high pulse is greater than 30ms */
		c = (c<<1)|(0x01);						/* then its logic HIGH */
		else									/* otherwise its logic LOW */
		c = (c<<1);
		while(PIND & (1<<DHT11_PIN));
	}
	return c;
}

int main(void)
{
	lcd_init(LCD_ON_DISPLAY);
	lcd_backlight(0);
	_delay_ms(200);
	lcd_backlight(1);
	_delay_ms(200);
		
	char data[5];

	lcd_clrscr();				/* clear LCD */
	lcd_gotoxy(0,0);			/* enter column and row position */
	lcd_puts("Humidity =");
	lcd_gotoxy(0,1);
	lcd_puts("Temp = ");
	
    while(1)
	{	
		Request();				/* send start pulse */
		Response();				/* receive response */
		I_RH=Receive_data();	/* store first eight bit in I_RH */
		D_RH=Receive_data();	/* store next eight bit in D_RH */
		I_Temp=Receive_data();	/* store next eight bit in I_Temp */
		D_Temp=Receive_data();	/* store next eight bit in D_Temp */
		CheckSum=Receive_data();/* store next eight bit in CheckSum */
		
		if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
		{
			//lcd_gotoxy(0,0);
			//lcd_puts("Error");
		}
		
		else
		{	
			itoa(I_RH,data,10);
			lcd_gotoxy(11,0);
			lcd_puts(data);
			lcd_puts(".");
			
			itoa(D_RH,data,10);
			lcd_puts(data);
			lcd_puts("%");

			itoa(I_Temp,data,10);
			lcd_gotoxy(6,1);
			lcd_puts(data);
			lcd_puts(".");
			
			itoa(D_Temp,data,10);
			lcd_puts(data);
			//lcddata(0xDF);
			lcd_puts("C ");
			
			//itoa(CheckSum,data,10);
			//lcd_puts(data);
			//lcd_puts(" ");
		}
				
	_delay_ms(500);
	}	
}