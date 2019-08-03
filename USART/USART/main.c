#define  F_CPU 2000000UL
#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE ((F_CPU / (USART_BAUDRATE * 16UL))-1)//obtain 16 bit value for UBBR register so that we can obtain baud rate of 9600
unsigned char latitude[15],longitude[15];
char i,j;
unsigned char return_char(void);
void transfer_char();
int main(void)
{
   UBRRL= (unsigned char)BAUD_PRESCALE;//lower bits of value of data to be store so that baud rate can be 9600
   UBRRH= (unsigned char)(BAUD_PRESCALE >> 8);//upper bits of value of data to be store so that baud rate can be 9600
   UCSRB|=(1<<TXEN)|(1<<RXEN);//enabling TX and RX 
   UCSRC|=(1<<URSEL)|(3 << UCSZ0);//transmitting 8 bits of data
    while (1) 
    {
	uint8_t	j=0;
uint8_t	i=return_char();
	if (i==0x24)//check if variable i is "$"
	{
		i=return_char();
		if (i==0x47)//check if variable i is "G"
	{
		i=return_char();
		if (i==0x50)//check if variable i is "P"
	{
		i=return_char();
		if (i==0x47)//check if variable i is "G"
	{
		i=return_char();
		if (i==0x47)//check if variable i is "G"
	{
		i=return_char();
		if (i==0x41)//check if variable i is "A"
	{
		i=return_char();
		if (i==0x2C)//check if variable i is ","
	{
		i=return_char();
		while(i!=0x2C)
		{
			i=return_char();
		}
		while (j<15)
		{
			i=return_char();
			if(i!=0x2C)
			{
				latitude[j]=i;//obtain the latitude value which is in character format
			}
			else
			{
				j++;
				break;
			}
			j++;
		}
		latitude[j]=return_char();//obtain direction either it is north or south
		j=0;
		return_char();
		while (j<15)
		{
			i=return_char();
			if(i!=0x2C)
			{
				longitude[j]=i;//obtain the longitude value which is in character format
			}
			else
			{
				j++;
				break;
			}
			j++;
		}
		longitude[j]=return_char();//obtain direction either it is east or west
		transfer_char();//transfer the obtain value via TX pin
	}
	}
	}
	}
	}
	}
	}
	}
}
unsigned char return_char()
{
	while (!(UCSRA&(1<<RXC)));//wait until UDR register contain data received from other device
	return UDR;// return value obtained from other device through Rx pin
}
void transfer_char()
{
			for (j=0;j<15;j++)
			{
				while(!(UCSRA&(1<<UDRE)));//wait until UDR buffer is empty so that new data to be transmitted can be loaded
			
				
					UDR=latitude[j];//load data in UDR buffer so that we can trasmit that data
					if (latitude[j]==0x4E)
				{
					while(!(UCSRA&(1<<UDRE)));
					UDR=0X0A;
					break;
				}
			}
			for (j=0;j<15;j++)
			{
				while(!(UCSRA&(1<<UDRE)));
			
				
					UDR=longitude[j];
					if (longitude[j]==0x45)
				{
					while(!(UCSRA&(1<<UDRE)));
					UDR=0X0A;
					break;
				}
			}
}