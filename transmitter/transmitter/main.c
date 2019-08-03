#define  F_CPU 2000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "dht11.h"
#define  BAUD_RATE 9600
#define baude_prescale ((F_CPU/(BAUD_RATE*16UL))-1)
void checksum1(char send_value);
void initiate(void);
char value_of_sum=0;
uint8_t temperature=0;
uint8_t temperature1=0;
uint8_t humidity=0;
uint8_t humidity1=0;
uint8_t checksum=0;
uint8_t sum=0;
uint8_t to_be_send=0;
int main(void)
{
 DDRC=0xFF;
 PORTC|=(1<<4);
 DDRA|=(1<<PINA0);
 PORTA|=(1<<PINA0);
 char tempr_in_char[3];
char value[26]="peshal nepal is a good boy";
char passon;
    while (1) 
    {
		initiate();
		_delay_ms(500);
		itoa(to_be_send,tempr_in_char,10);
		
		for (char j=0;j<26;j++)
		{
			passon=value[j];
			PORTC=0x0A;
			PORTC&=~(1<<4);
			_delay_ms(150);
			PORTC=0x00;
			PORTC|=(1<<PINC4);//this cause the encoder to be disable
			_delay_ms(150);
			PORTC=passon;//as we can send 1 nibble of data so dividing 8 bit into 4 bits and sending
			PORTC&=~(1<<4); //setting this bit will make encoder enable as it will be active in active low1
			_delay_ms(150);
            PORTC=0x00;
		    PORTC|=(1<<PINC4);//this cause the encoder to be disable
		    _delay_ms(150);
			PORTC=(passon>>4);
			PORTC&=~(1<<4);
		  	_delay_ms(150);
			 PORTC=0x00;
		    PORTC|=(1<<PINC4);//this cause the encoder to be disable
		    _delay_ms(150);
			checksum1(passon);
			PORTC=value_of_sum;
			PORTC&=~(1<<4);
			_delay_ms(150);
			PORTC=0x00;
			PORTC|=(1<<PINC4);//this cause the encoder to be disable
			_delay_ms(150);
		}
    }
}
void initiate()
{
	start_conversion();
	response();
	humidity=receiving_data();
	humidity1=receiving_data();
	temperature=receiving_data();
	temperature1=receiving_data();
	checksum=receiving_data();
	sum=humidity1+humidity+temperature1+temperature;
	if(checksum>sum)
	{
		sum=checksum-sum;
	}
	else
	{
		sum=sum-checksum;
	}
	if(sum>=0 && sum<=5)
	{
		to_be_send=temperature;
	}
	else
	{
		to_be_send=0x00;
	}
}
void checksum1(char send_value)
{
	char lb=(send_value&0x0F);
	char ub=((send_value>>4)&0x0F);
	value_of_sum=ub+lb;
}
