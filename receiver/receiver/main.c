#define F_CPU 2000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#define USart_baud 9600
#define baud_prescale ((F_CPU/(USart_baud*16UL))-1)
#define CONTROL_reg DDRA
#define OUTPUT_reg PORTA
#define INPUT_pin PINA
char datacheck=0;
char dataget=0;
char checksum_of_value=1;

void checksum(void);
void read(void);
void transfer(void);
int main(void)
{  
	UBRRH=baud_prescale>>8;
	UBRRL=baud_prescale;
	UCSRC |=(1<<URSEL)|(3<<UCSZ0);
	UCSRB |=(1<<TXEN)|(1<<RXEN);
	DDRC=0x00;	 
	PORTC=0x00;
	uint16_t checkvalue=0x0000;
	while (1) 
    {
		        while (!(PINC&(1<<4)));//as when data is receive pinC0 will be high soo checking that 
		         read();//this function is responsible of setting every bits of data 
			    _delay_ms(300);//delay is more than 200 but less or near to 400 as after sending 1st low bits data there is delay of 250 ms and deactive is for about 250 ms 
                  if(datacheck==0xA0)
				  {
			     datacheck=0x00;
			     while (!(PINC&(1<<4)));//as when data is receive pinC0 will be high soo checking that 
		         read();//this function is responsible of setting every bits of data 
			    _delay_ms(300);//delay is more than 200 but less or near to 400 as after sending 1st low bits data there is delay of 250 ms and deactive is for about 250 ms 
		        dataget|=(datacheck>>4);//shifting higher bits into lower bits
				datacheck=0x00;
			    while (!(PINC&(1<<4))); 
				  read(); 	
		    	 _delay_ms(300);
				  dataget|=datacheck;
				  datacheck=0x00; 
				  while (!(PINC&(1<<4))); 
				  read(); 	
		    	 _delay_ms(300);
				 checksum();
				 if(((datacheck>>4)&checksum_of_value)==0)
				 {
					 transfer();
				 }  
				  dataget=0x00; 
				  }
				 else
				 {
					 _delay_ms(500);
				 }
				 
    }
	 datacheck=0x00;
}

void read()
{
  if((PINC&(1<<PINC0)))
  {
	  
	  datacheck|=(1<<4);
  }
  if((PINC&(1<<PINC1)))
  {
	  
	  datacheck|=(1<<5);
  }

  if((PINC&(1<<PINC2)))
  {
	  
	  datacheck|=(1<<6);
  }
  if((PINC&(1<<PINC3)))
  {
	  datacheck|=(1<<7);
  }
}
void transfer()
{
	while(!(UCSRA&(1<<UDRE)));
	UDR=dataget;
}
void   checksum()
{
	char value=dataget;
	char lb=(value&0x0F);
	char ub=((value>>4)&0x0F);
	checksum_of_value=lb+ub;
	checksum_of_value=(checksum_of_value&0xF);
	checksum_of_value=~checksum_of_value;
}