/*
 * project1.c
 *
 * Created: 6/10/2019 10:51:32 AM
 * Author : peshal
 */ 

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
   DDRC=0xFF;
   DDRB=0X00;
   DDRD =0XFF;
   PORTB=0x02;
   PORTC=0x01;
    while (1) 
    {
		PORTD=(1<<PINC2);
		if(!(PINB&0x02))
		{
			PORTC =(PORTC<<2);
		}
		else
		{
			if((PORTC==0x01))
			{
				
				PORTC=PORTC;
			}
			else if( PORTC==0x00)
			{
				PORTC=0x01;
				
			}
			else
			{
				PORTC =PORTC>>1;
			}
			
		}
		_delay_ms(250);
    } 
	
}

