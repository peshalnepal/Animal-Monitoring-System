//this is code for converting analog value of LM35 to digital ofrm
#define F_CPU 2000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#define BAUD_RATE 9600
#define BAUD_PRESCALE ((F_CPU/(BAUD_RATE*16UL))-1)
static volatile char tempr[4];
static volatile uint16_t temperature_in_voltage_format;
static volatile uint8_t thelow;
int main(void)
{
	 DDRA=0x00;	 
	UBRRH=(BAUD_PRESCALE>>8);
	UBRRL=BAUD_PRESCALE;
	UCSRB|=(1<<TXEN);
	UCSRC|=(1<<URSEL)|(3<<UCSZ0);
	ADCSRA |=(1<<ADPS2);//this bit is set when we want to divide CLK frequency by 8
	ADMUX |=(1<<REFS0)|(1<<ADLAR);//REFSO set ref voltage to VCC and ADLAR is use for left shifting values in ADCH and ADCL register
	ADCSRA |=(1<<ADEN); //this is use for enabling ADC 
	MCUCR |=(1<<SM0);//avr dude 
     MCUCR |=(1<<SE);//enabling sleep so that less error can occur
	ADCSRA |=(1<<ADSC);//start conversion
    while (1) 
	{
		thelow=ADCL;// done so that lag doesn't occur while running as we need to access ADCH and ADCL at same time
temperature_in_voltage_format= (ADCH<<2)|(thelow>>6);//this is use to get 10 bit data from ADC
itoa(temperature_in_voltage_format,tempr,10);//convert int value i.e temperature_in_voltage_format into char format tempr 
for (uint8_t count=0;count<4;count++)
{
	while(!(UCSRA&(1<<UDRE)));
	UDR=tempr[count];
}
while(!(UCSRA&(1<<UDRE)));
UDR=0x0D;
while(!(UCSRA&(1<<UDRE)));
UDR=0x0A;
	ADCSRA |=(1<<ADSC);//again start conversion
	_delay_ms(20);
	
}
}