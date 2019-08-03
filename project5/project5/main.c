//this is code for reading data from DHT11 module using single wire protocol
#define  F_CPU 2000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#define inputport   PORTA //setting input port to port A
#define USARTBAUD 9600
#define BAUDRATE ((F_CPU/(USARTBAUD*16UL))-1)
void start_conversion(void);
void response(void);
uint8_t receiving_data(void);
void transfering(int);
 char tempr_char_Form[3];
int main(void)
{

	uint8_t temperature=0;
	uint8_t temperature1=0;
	uint8_t humidity=0;
	uint8_t humidity1=0;
	uint8_t checksum=0;
	uint8_t sum=0;
    UBRRH=BAUDRATE>>8;
	UBRRL=BAUDRATE;
	UCSRC|=(1<<URSEL)|(3<<UCSZ0);
	UCSRB|=(1<<TXEN);
    while (1) 
    {
		char value_of_tempr[4];
		DDRA|=(1<<PINA0);
        inputport|=(1<<PINA0);
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
	    	transfering(temperature);
		}
		_delay_ms(1500);
	   }
}

void start_conversion()
{
	DDRA|=(1<<PINA0);//this will make pinA0 as output 
	inputport &=~(1<<PINA0);//this will make output low along with delay for than 18 ms 
	_delay_ms(18);
	inputport |=(1<<PINA0);//this is responsible for making output of PINA0 high
	_delay_us(20);
}
void response()
{
	DDRA &=~(1<<PINA0);//this is responsible for making PinA0 input pin
	PORTA|=(1<<PINA0);//this is making pinA0 internally high 
	while(PINA&0x01);//loop until pinA0 goes low
	while ( !(PINA&0X01));//loop until PinA0 goes high
	while(PINA&0x01);//loop until pinA0 goes low
}
uint8_t receiving_data()
{
	char i;
	uint8_t data=0x00;//responsible for storing 8 bit data
	for (i=0;i<8;i++)
	{
			while (!(PINA&0x01));//wait until signal coming in pinA0 is low
			_delay_us(33);//wait for 33 us as for logical zero signal is high for around 28 us and for logical one it is high for around 70 us 
			if(PINA&0x01)//if signal is still high after 33 us then set last bit of data high by left shifting it
			{
				data=(data<<1)|0x01;
			}
			else
			{
				data=(data<<1)&0xfe;
			}
			while(PINA&0x01);//wait until signal goes low
	}
	return data;//return 8 bit data
}
void transfering(int tempr)
{
char n=0;
 itoa(tempr,tempr_char_Form,10);
 while (n<2)
 {
	 while((UCSRA&(1<<UDRE))==0);
	 UDR=tempr_char_Form[n];
	 n++;
 }
 
}