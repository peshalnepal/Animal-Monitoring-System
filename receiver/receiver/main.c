#define F_CPU 2000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#define USart_baud 9600
#define baud_prescale ((F_CPU/(USart_baud*16UL))-1)
#define CONTROL_reg DDRA
#define OUTPUT_reg PORTA
#define INPUT_pin PINA
char datacheck,dataget=0;
void read(void);
void transfer(void);
int main(void)
{  
	UBRRH=baud_prescale>>8;
	UBRRL=baud_prescale;
	UCSRC |=(1<<URSEL)|(3<<UCSZ0);
	UCSRB |=(1<<TXEN)|(1<<RXEN);
	DDRC=0x00;	 
	while (1) 
    {
		   
		  while (!(PINC&(1<<0)));//as when data is receive pinC0 will be high soo checking that 
		     read();//this function is responsible of setting every bits of data 
			  _delay_ms(350);//delay is more than 200 but less or near to 400 as after sending 1st low bits data there is delay of 250 ms and deactive is for about 250 ms 
		    dataget=(dataget>>4);//shifting higher bits into lower bits

			 while (!(PINC&(1<<0)));
				read();
			   transfer();//sending data at tx pin
			 _delay_ms(350);   
		     dataget=0x00;
    }
}

void read()
{
  
  if((PINC&(1<<PINC4)))
  {
	  
	  dataget|=(1<<PINC4);
  }
  
  if((PINC&(1<<PINC5)))
  {
	  
	  dataget|=(1<<PINC5);
  }

  if((PINC&(1<<PINC6)))
  {
	  
	  dataget|=(1<<PINC6);
  }
  if((PINC&(1<<PINC7)))
  {
	  
	  dataget|=(1<<PINC7);
  }
}
void transfer()
{
	while(!(UCSRA&(1<<UDRE)));
	UDR=dataget;
}