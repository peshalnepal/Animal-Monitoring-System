#define  F_CPU 2000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#define  BAUD_RATE 9600
#define baude_prescale ((F_CPU/(BAUD_RATE*16UL))-1)
int checksum=0;
int main(void)
{
 DDRC=0xFF;
char value[9]="Madhavsir";
    while (1) 
    {
		char passon;
		for (uint8_t i=0;i<9;i++)
		{
			passon=value[i];
			PORTC&=~(1<<PINC4);//setting this bit will make encoder enable as it will be active in active low
			PORTC=passon;//as we can send 1 nibble of data so dividing 8 bit into 4 bits and sending 
			_delay_ms(250);
			PORTC|=(1<<PINC4);//this cause the encoder to be disable 
			_delay_ms(250);
			PORTC&=~(1<<PINC4);
			PORTC=(passon>>4);//sending higher 4 bit
			_delay_ms(250);
			PORTC|=(1<<PINC4);
			_delay_ms(250);
		}
		
    }
}

