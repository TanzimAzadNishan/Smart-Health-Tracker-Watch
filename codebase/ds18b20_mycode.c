#define F_CPU 1000000
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
extern void lcd_backlight(char on);   //not in lcd.h

#include <stdlib.h>       //for itoa()


#define THERM_PORT PORTC
#define THERM_DDR DDRC
#define THERM_PIN PINC
#define THERM_DQ PC7


#define THERM_CMD_CONVERTTEMP 0x44
#define THERM_CMD_RSCRATCHPAD 0xbe
#define THERM_CMD_SKIPROM 0xcc
#define THERM_DECIMAL_STEPS_12BIT 0.0625


uint8_t therm_reset(){
	uint8_t i;
	
	THERM_PORT = THERM_PORT & ~(1<<THERM_DQ);
	THERM_DDR = THERM_DDR | (1<<THERM_DQ);
	_delay_us(480);

	THERM_INPUT_MODE();
	_delay_us(60);

	i=(THERM_PIN & (1<<THERM_DQ));
	_delay_us(420);
	
	//Return the value read from the presence pulse (0=OK, 1=WRONG)
	return i;

}


void therm_write_bit(uint8_t bit){

	THERM_LOW();
	THERM_OUTPUT_MODE();
	_delay_us(1);
	
	//If we want to write 1, release the line (if not will keep low)
	if(bit){
		THERM_DDR = THERM_DDR & ~(1<<THERM_DQ);
	}

	_delay_us(60);
	THERM_DDR = THERM_DDR & ~(1<<THERM_DQ);
}



uint8_t therm_read_bit(void){
	uint8_t bit=0;
	
	//Pull line low for 1uS
	THERM_PORT= THERM_PORT & ~(1<<THERM_DQ);
	THERM_DDR = THERM_DDR | (1<<THERM_DQ);
	_delay_us(1);
	
	//Release line and wait for 14uS
	THERM_DDR = THERM_DDR & ~(1<<THERM_DQ);
	_delay_us(14);
	
	//Read line value
	if(THERM_PIN&(1<<THERM_DQ)){ 
		bit=1;
	}
	
	//Wait for 45uS to end and return read value
	_delay_us(45);
	return bit;
}


uint8_t therm_read_byte(){
	uint8_t i=8, n=0;
	while(i--){
		//Shift one position right and store read value
		n = n>>1;
		n = n | (therm_read_bit()<<7);
	}
	return n;
}


void therm_write_byte(uint8_t byte){
	uint8_t i=8;
	while(i--){
		//Write actual bit and shift one position right to make the next bit ready
		therm_write_bit(byte & 1);
		byte = byte>>1;
	}
}



void therm_read_temperature(){
	// Buffer length must be at least 12bytes long! ["+XXX.XXXX C"]
	char buffer[14];
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
	
	//Store temperature integer digits and decimal digits
	digit=temperature[0]>>4;
	digit|=(temperature[1]&0x7)<<4;
	
	//Store decimal digits
	decimal=temperature[0]&0xff;
	
	//Format temperature into a string [+XXX.XXXX C]
	sprintf(buffer, "%+d.%04u C", digit, decimal);
}

















