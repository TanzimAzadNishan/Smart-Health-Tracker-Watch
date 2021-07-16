
#define F_CPU 1000000

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <util/delay.h>
#include <math.h>  //include libm

#include "mpu6050/mpu6050.h"

#define MPU6050_GETATTITUDE 0

#include "i2chw/lcd.h"
extern void lcd_backlight(char on);   //not in lcd.h


void one(int16_t ax)
{
	if(ax < -12000) PORTA = 0b10000000;
	else if(ax < -8192) PORTA = 0b01000000;
	else if(ax < -4096) PORTA = 0b00100000;
	else if(ax < 0) PORTA = 0b00010000;
	else if(ax < 4096) PORTA = 0b00001000;
	else if(ax < 8192) PORTA = 0b00000100;
	else if(ax < 12000) PORTA = 0b00000010;
	else PORTA = 0b00000001;
}


void twoA(int16_t ax)
{
	if(ax < -10000) PORTD = (0b01000000);
	else if(ax < -6000) PORTD = (0b00100000);
	else if(ax < 0) PORTD = (0b0001000);
	else PORTD = (0b00001000);
}


void twoB(int16_t ax)
{
	if(ax < -10000) PORTA = (0b00001000);
	else if(ax < -6000) PORTA = (0b00000100);
	else if(ax < 6000) PORTA = (0b00000010);
	else PORTA = (0b00000001);
}



int main(void) {
	
    lcd_init(LCD_ON_DISPLAY);
    lcd_backlight(0);
    _delay_ms(200);
    lcd_backlight(1);
    _delay_ms(200);
	lcd_clrscr();	
	
	DDRA = 0xFF;
	DDRD = 0xFF;
	
	PORTD = 0x00;
	PORTA = 0x00;
	
	GyroAddr = (0x68 << 1);
	GyroAddr = (0x69 << 1);


	#if MPU6050_GETATTITUDE == 0
	int16_t ax = 0;
	int16_t ay = 0;
	int16_t az = 0;
	int16_t gx = 0;
	int16_t gy = 0;
	int16_t gz = 0;
	double axg = 0;
	double ayg = 0;
	double azg = 0;
	double gxds = 0;
	double gyds = 0;
	double gzds = 0;
	#endif

	#if MPU6050_GETATTITUDE == 1 || MPU6050_GETATTITUDE == 2
	long *ptr = 0;
	double qw = 1.0f;
	double qx = 0.0f;
	double qy = 0.0f;
	double qz = 0.0f;
	double roll = 0.0f;
	double pitch = 0.0f;
	double yaw = 0.0f;
	#endif

	//init uart
	//uart_init(UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU));

	//init interrupt
	sei();

	//init mpu6050
	mpu6050_init();
	_delay_ms(50);

	//init mpu6050 dmp processor
	#if MPU6050_GETATTITUDE == 2
	mpu6050_dmpInitialize();
	mpu6050_dmpEnable();
	_delay_ms(10);
	#endif
	
	int which = 0;

	while(1) {
		lcd_gotoxy(0, 0);
		lcd_puts("MPU6050");
		
		if(which)
		{
			GyroAddr = (0x69 << 1);
		}
		else
		{
			GyroAddr = (0x68 << 1);
		}
		
		//#if MPU6050_GETATTITUDE == 0
		mpu6050_getRawData(&ax, &ay, &az, &gx, &gy, &gz);
		//mpu6050_getConvData(&axg, &ayg, &azg, &gxds, &gyds, &gzds);
		//#endif
		
		ax = -ax;
		ay = -ay;
		az = -az;

		//one(ax);
		
		/*if(which) twoA(ax);
		else twoB(ax);*/
				
		
		//twoB(ax);
		
		which = 1 - which;
		
		
		//PORTA = ax & 0xFF;
		
		_delay_ms(50);

		char val[4];
		char output[8]="ax=";
		itoa((int) ax, val, 10);
		strcat(output, val);
		/*strcat(output, "ay=");
		itoa((int) ay, val, 10);
		strcat(output, val);*/
		
		lcd_gotoxy(0, 1);
		lcd_puts(output);
		
		char outy[8]="ay=";
		itoa((int) ay, val, 10);
		strcat(outy, val);
		
		lcd_gotoxy(8, 1);
		lcd_puts(outy);		
		
		char outz[8]="gz=";
		itoa((int) gz, val, 10);
		strcat(outz, val);
		
		lcd_gotoxy(8, 0);
		lcd_puts(outz);		
		
		_delay_ms(1000);	

		//#if MPU6050_GETATTITUDE == 1
		//mpu6050_getQuaternion(&qw, &qx, &qy, &qz);
		//mpu6050_getRollPitchYaw(&roll, &pitch, &yaw);
		//_delay_ms(10);
		//#endif
//
		//#if MPU6050_GETATTITUDE == 2
		//if(mpu6050_getQuaternionWait(&qw, &qx, &qy, &qz)) {
			//mpu6050_getRollPitchYaw(qw, qx, qy, qz, &roll, &pitch, &yaw);
		//}
		//_delay_ms(10);
		//#endif
//
		//#if MPU6050_GETATTITUDE == 0
		//char itmp[10];
		//ltoa(ax, itmp, 10); uart_putc(' '); uart_puts(itmp); uart_putc(' ');
		//ltoa(ay, itmp, 10); uart_putc(' '); uart_puts(itmp); uart_putc(' ');
		//ltoa(az, itmp, 10); uart_putc(' '); uart_puts(itmp); uart_putc(' ');
		//ltoa(gx, itmp, 10); uart_putc(' '); uart_puts(itmp); uart_putc(' ');
		//ltoa(gy, itmp, 10); uart_putc(' '); uart_puts(itmp); uart_putc(' ');
		//ltoa(gz, itmp, 10); uart_putc(' '); uart_puts(itmp); uart_putc(' ');
		//uart_puts("\r\n");
//
		//dtostrf(axg, 3, 5, itmp); uart_puts(itmp); uart_putc(' ');
		//dtostrf(ayg, 3, 5, itmp); uart_puts(itmp); uart_putc(' ');
		//dtostrf(azg, 3, 5, itmp); uart_puts(itmp); uart_putc(' ');
		//dtostrf(gxds, 3, 5, itmp); uart_puts(itmp); uart_putc(' ');
		//dtostrf(gyds, 3, 5, itmp); uart_puts(itmp); uart_putc(' ');
		//dtostrf(gzds, 3, 5, itmp); uart_puts(itmp); uart_putc(' ');
		//uart_puts("\r\n");
//
		//uart_puts("\r\n");
//
		//_delay_ms(1000);
		//#endif
//
		//#if MPU6050_GETATTITUDE == 1 || MPU6050_GETATTITUDE == 2
//
		////quaternion
		//ptr = (long *)(&qw);
		//uart_putc(*ptr);
		//uart_putc(*ptr>>8);
		//uart_putc(*ptr>>16);
		//uart_putc(*ptr>>24);
		//ptr = (long *)(&qx);
		//uart_putc(*ptr);
		//uart_putc(*ptr>>8);
		//uart_putc(*ptr>>16);
		//uart_putc(*ptr>>24);
		//ptr = (long *)(&qy);
		//uart_putc(*ptr);
		//uart_putc(*ptr>>8);
		//uart_putc(*ptr>>16);
		//uart_putc(*ptr>>24);
		//ptr = (long *)(&qz);
		//uart_putc(*ptr);
		//uart_putc(*ptr>>8);
		//uart_putc(*ptr>>16);
		//uart_putc(*ptr>>24);
//
		////roll pitch yaw
		//ptr = (long *)(&roll);
		//uart_putc(*ptr);
		//uart_putc(*ptr>>8);
		//uart_putc(*ptr>>16);
		//uart_putc(*ptr>>24);
		//ptr = (long *)(&pitch);
		//uart_putc(*ptr);
		//uart_putc(*ptr>>8);
		//uart_putc(*ptr>>16);
		//uart_putc(*ptr>>24);
		//ptr = (long *)(&yaw);
		//uart_putc(*ptr);
		//uart_putc(*ptr>>8);
		//uart_putc(*ptr>>16);
		//uart_putc(*ptr>>24);
//
		//uart_putc('\n');
		//#endif

	}

}
