#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
extern void lcd_backlight(char on);   //not in lcd.h

#include <stdlib.h>       //for itoa()
#include <stdbool.h>
#include <stdio.h>
#define DHT11_PIN 6

uint8_t c=0,I_RH,D_RH,I_Temp,D_Temp,CheckSum;


// Thermometer Connections (At your choice)
#define THERM_PORT PORTC
#define THERM_DDR DDRC
#define THERM_PIN PINC
#define THERM_DQ PC7

#define THERM_INPUT_MODE() THERM_DDR&=~(1<<THERM_DQ)
#define THERM_OUTPUT_MODE() THERM_DDR|=(1<<THERM_DQ)
#define THERM_LOW() THERM_PORT&=~(1<<THERM_DQ)
#define THERM_HIGH() THERM_PORT|=(1<<THERM_DQ)

#define THERM_CMD_CONVERTTEMP 0x44
#define THERM_CMD_RSCRATCHPAD 0xbe
#define THERM_CMD_WSCRATCHPAD 0x4e
#define THERM_CMD_CPYSCRATCHPAD 0x48
#define THERM_CMD_RECEEPROM 0xb8
#define THERM_CMD_RPWRSUPPLY 0xb4
#define THERM_CMD_SEARCHROM 0xf0
#define THERM_CMD_READROM 0x33
#define THERM_CMD_MATCHROM 0x55
#define THERM_CMD_SKIPROM 0xcc
#define THERM_CMD_ALARMSEARCH 0xec


//char disp[16]="0000000000000001";
//char result[8] = "00000001";
char ds18b20_temp[14];
char dht11_temp_main[5];
char dht11_temp_fraction[5];
char dht11_hum_main[5];
char dht11_hum_fraction[5];
char pulse_bpm[4];
int noOfPulse = 0;
uint16_t thresh=550;
int bpm_count=0;
bool isPulseDetected = false;
bool isReadyForSms = false;


void ADC_Init(){
	DDRA=0x0;
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


void uart_init(void){
	int UBBRValue = 12;
	UBRRH = (unsigned char) (UBBRValue >> 8);
	UBRRL = (unsigned char) UBBRValue;
	UCSRA = 0x02;
	UCSRB = (1 << RXEN) | (1 << TXEN);    //Enable the receiver and transmitter
	UCSRC = 0b10000110;	
}

void uart_send(char data){
	while((UCSRA & (1<<UDRE)) == 0);
	UDR = data;
}

void sendToArduino(){
	char sms[200] = "Temperature: ";
	strcat(sms, dht11_temp_main);
	strcat(sms, ".");
	strcat(sms, dht11_temp_fraction);
	strcat(sms, "C\n");
	strcat(sms, "Humidity: ");
	strcat(sms, dht11_hum_main);
	strcat(sms, ".");
	strcat(sms, dht11_hum_fraction);
	strcat(sms, "%\n");
	strcat(sms, "Body Temperature: ");
	strcat(sms, ds18b20_temp);
	strcat(sms, "C\n");
	strcat(sms, "Pulse Rate: ");
	strcat(sms, pulse_bpm);
	strcat(sms, " bpm\n");
	
	int i = 0;
	while (sms[i] != 0x00)
	{
		uart_send(sms[i]);
		//_delay_ms(500);
		i++;
	}
	
	_delay_ms(1000);			
}


uint8_t therm_reset(){
	uint8_t i;
	//Pull line low and wait for 480uS
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(480);
	//Release line and wait for 60uS
	THERM_INPUT_MODE();
	_delay_us(60);
	//Store line value and wait until the completion of 480uS period
	i=(THERM_PIN & (1<<THERM_DQ));
	_delay_us(420);
	//Return the value read from the presence pulse (0=OK, 1=WRONG)
	return i;

}



void therm_write_bit(uint8_t bit){
	//Pull line low for 1uS
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(1);
	//If we want to write 1, release the line (if not will keep low)
	if(bit) THERM_INPUT_MODE();
	//Wait for 60uS and release the line
	_delay_us(60);
	THERM_INPUT_MODE();}


uint8_t therm_read_bit(void){
	uint8_t bit=0;
	//Pull line low for 1uS
	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(1);
	//Release line and wait for 14uS
	THERM_INPUT_MODE();
	_delay_us(14);
	//Read line value
	if(THERM_PIN&(1<<THERM_DQ)) bit=1;
	//Wait for 45uS to end and return read value
	_delay_us(45);
	return bit;
}



uint8_t therm_read_byte(void){
	uint8_t i=8, n=0;
	while(i--){
		//Shift one position right and store read value
		n>>=1;
		n|=(therm_read_bit()<<7);
	}
	return n;

}


void therm_write_byte(uint8_t byte){
	uint8_t i=8;
	while(i--){
		//Write actual bit and shift one position right to make the next bit ready
		therm_write_bit(byte&1);
		byte>>=1;
	}
}

#define THERM_DECIMAL_STEPS_12BIT 0.0625
void therm_read_temperature(){
	// Buffer length must be at least 12bytes long! ["+XXX.XXXX C"]
	//char buffer[14];
	uint8_t temperature[2];
	int8_t digit;
	uint16_t decimal;
	
	
	//Reset, skip ROM and start temperature conversion
	therm_reset();
	therm_write_byte(THERM_CMD_SKIPROM);
	therm_write_byte(THERM_CMD_CONVERTTEMP);
	//Wait until conversion is complete
	while(!therm_read_bit());
	//Reset, skip ROM and send command to read
	therm_reset();
	therm_write_byte(THERM_CMD_SKIPROM);
	therm_write_byte(THERM_CMD_RSCRATCHPAD);
	
	//Read (only 2 first bytes)
	temperature[0]=therm_read_byte();
	temperature[1]=therm_read_byte();
	therm_reset();
	
	digit=temperature[0]>>4;
	digit|=(temperature[1]&0x7)<<4;

	decimal=temperature[0]&0xff;
	sprintf(ds18b20_temp, "%d.%d", digit, (int)decimal/10);
	
	lcd_gotoxy(3,1);
	lcd_puts(ds18b20_temp);
	lcd_gotoxy(8,1);
	lcd_puts(" ");
}



void Request()
{
	DDRD |= (1<<DHT11_PIN);
	PORTD &= ~(1<<DHT11_PIN);
	_delay_ms(20);
	PORTD |= (1<<DHT11_PIN);
}

void Response()
{
	DDRD &= ~(1<<DHT11_PIN);
	while(PIND & (1<<DHT11_PIN));
	while((PIND & (1<<DHT11_PIN))==0);
	while(PIND & (1<<DHT11_PIN));
}


uint8_t Receive_data()
{
	for (int q=0; q<8; q++)
	{
		while((PIND & (1<<DHT11_PIN)) == 0);
		_delay_us(30);
		if(PIND & (1<<DHT11_PIN))
		c = (c<<1)|(0x01);
		else
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

	lcd_clrscr();
	lcd_gotoxy(0,0);
	lcd_puts("H=");
	lcd_gotoxy(9,1);
	lcd_puts("T=");
	
	
	lcd_gotoxy(0,1);
	lcd_puts("BT=");
	
	lcd_gotoxy(9, 0);
	lcd_puts("BPM=");
	
	
	DDRD = 0xFF;  
	DDRC = 0xFF;  
	DDRA = 0x00;
		
	ADC_Init();
	uart_init();
	
	while(1)
	{
		/*noOfPulse++;
		char val[5];
		itoa(noOfPulse, val, 10);
		lcd_gotoxy(7,0);
		lcd_puts(val);*/		
		
		Request();				
		Response();				
		I_RH=Receive_data();	
		D_RH=Receive_data();	
		I_Temp=Receive_data();	
		D_Temp=Receive_data();	
		CheckSum=Receive_data();
		    
		if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum)
		{
			//lcd_gotoxy(0,0);
			//lcd_puts("Error");
		}
		    
		else
		{
			itoa(I_RH,dht11_hum_main,10);
			lcd_gotoxy(2,0);
			lcd_puts(dht11_hum_main);
			lcd_puts(".");
			    
			itoa(D_RH,dht11_hum_fraction,10);
			lcd_puts(dht11_hum_fraction);
			lcd_puts("%");

			itoa(I_Temp,dht11_temp_main,10);
			lcd_gotoxy(11,1);
			lcd_puts(dht11_temp_main);
			lcd_puts(".");
			    
			itoa(D_Temp,dht11_temp_fraction,10);
			lcd_puts(dht11_temp_fraction);
			lcd_puts("C");
		}
		    
		_delay_ms(50);
		
		therm_read_temperature();
		_delay_ms(50);		
		
		noOfPulse++;
		if(noOfPulse % 23 == 0){
			noOfPulse = 0;
			if(bpm_count > 16){
				bpm_count = 16;
			}
			itoa(bpm_count*6,pulse_bpm,10);
		
			lcd_gotoxy(13,0);
			lcd_puts(pulse_bpm);
			bpm_count=0;
			isPulseDetected = false;
			//_delay_ms(50);			
		}
		else{
			uint16_t adc_volt = ADC_Read(0);
			
			if(adc_volt > thresh && !isPulseDetected){
				bpm_count+=1;
				isPulseDetected = true;
			}
			else{
				isPulseDetected = false;
			}
			_delay_ms(50);			
		}
		
		if(noOfPulse % 10 == 0){
			isReadyForSms = true;
		}
		if(isReadyForSms){
			lcd_gotoxy(7,0);
			lcd_puts("Y");
			isReadyForSms = false;
			sendToArduino();
		}
		else{
			lcd_gotoxy(7,0);
			lcd_puts("N");
		}		
		
		_delay_ms(50);
				
	}
}

