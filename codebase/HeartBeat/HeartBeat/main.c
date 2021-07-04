#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
extern void lcd_backlight(char on);   //not in lcd.h

#include <stdlib.h>       //for itoa()
#include <time.h>


char disp[16]="0000000000000001";
char result[8] = "00000001";

void ADC_Init(){
	DDRA=0x0;			/* Make ADC port as input */
	ADMUX = 0b01100000;
	ADCSRA = 0b10000101;
}


uint16_t ADC_Read(char channel){
	uint16_t lower;
	uint16_t upper;
	uint16_t result;
	float voltage;
	
	ADCSRA |= (1 << ADSC);
	while(ADCSRA & (1 << ADSC)){;}

	lower = (ADCL>>6);
	upper = (ADCH<<2);
	result = upper | lower;
	voltage = result * 5.0 / 1024;
	
	return result;
}


int main(void)
{
    lcd_init(LCD_ON_DISPLAY);
    lcd_backlight(0);
    _delay_ms(200);
    lcd_backlight(1);
    _delay_ms(200);
	lcd_clrscr();
	
	//clock_t startTime = clock() / (CLOCKS_PER_SEC / 1000);
	//clock_t currTime = startTime;
	
	//long int startTime = (long int)startTime_t;
	//long int currTime = (long int)currTime_t;
	
	DDRD = 0xFF;  // #
	DDRC = 0xFF;  //for lcd
	DDRA = 0x00;  //Analog input
	    
	ADC_Init();
	

	for(;;){
	lcd_gotoxy(0, 0);
	lcd_puts("Pulse Meter:");		
	lcd_gotoxy(13, 0);
	lcd_puts("S");
	
	_delay_ms(2000);
	
	lcd_gotoxy(13, 0);
	lcd_puts("P");
	
	_delay_ms(5000);
		
	lcd_gotoxy(13, 0);
	lcd_puts("W");
	
	int i = 0;
	uint16_t thresh=550;
	int count=0;
	int counted = 0;
	
	
	double sampling_rate= 0.100 ;//
	int time_limit = 10 ;   //in seconds
	
	int h=0;
	int l=1023;
	
	char val[4]; // 0 - 255 value
	for(i = 0; i < 40; i++){
			
		char temp[11]="";
		char ccount[3];

		uint16_t a=ADC_Read(0);
		//if(a>h)h=a; //max peak
		//if(a<l)l=a; //min peak
		
		if(a>thresh && counted == 0){
			count+=1; //peak counting
			counted = 1;
		}
		else{
			counted = 0;
		}
		
		/*itoa(count,ccount,10);
		itoa((int)(a),val,10);
		
		strcat(temp,val);
		strcat(temp," - ");
		strcat(temp,ccount);

		lcd_gotoxy(0, 1);
		lcd_puts(temp);*/
		
		//_delay_ms(1000);

		_delay_ms(200);
	}
	//displaying statistics of the waveform
	/*char peaks[10]="H- ";
	itoa(h,val,10);
	strcat(peaks,val);
	strcat(peaks,"; L- ");
	itoa(l,val,10);
	strcat(peaks,val);
	
	lcd_gotoxy(0, 1);
	lcd_puts(peaks);*/
	
	//lcd_clrscr();
	//displaying the heart rate
	lcd_clrscr();
	char bpm[10]="H.Rate - ";
	itoa(count*6,val,10);
	strcat(bpm,val);
	strcat(bpm," bpm");
	
	lcd_gotoxy(0, 1);
	lcd_puts(bpm);
	}
		 
}

