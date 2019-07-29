/*
 * dht11.c
 *
 * Created: 7/28/2019 11:47:07 PM
 *  Author: pesha
 */ 
#include "dht11.h"
void start_conversion()
{
	DDRC|=(1<<PINC0);//this will make PINC0 as output
	inputport &=~(1<<PINC0);//this will make output low along with delay for than 18 ms
	_delay_ms(18);
	inputport |=(1<<PINC0);//this is responsible for making output of PINC0 high
	_delay_us(20);
}
void response()
{
	DDRC &=~(1<<PINC0);//this is responsible for making PINC0 input pin
	PORTC|=(1<<PINC0);//this is making PINC0 internally high
	while(PINC&0x01);//loop until PINC0 goes low
	while ( !(PINC&0X01));//loop until PINC0 goes high
	while(PINC&0x01);//loop until PINC0 goes low
}
uint8_t receiving_data()
{
	char i;
	uint8_t data=0x00;//responsible for storing 8 bit data
	for (i=0;i<8;i++)
	{
		while (!(PINC&0x01));//wait until signal coming in PINC0 is low
		_delay_us(33);//wait for 33 us as for logical zero signal is high for around 28 us and for logical one it is high for around 70 us
		if(PINC&0x01)//if signal is still high after 33 us then set last bit of data high by left shifting it
		{
			data=(data<<1)|0x01;
		}
		else
		{
			data=(data<<1)&0xfe;
		}
		while(PINC&0x01);//wait until signal goes low
	}
	return data;//return 8 bit data
}