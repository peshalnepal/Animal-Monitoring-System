#define F_CPU 2000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include "nrf1.h"
#define MOSI 5
#define MISO 6
#define SCK 7
#define SS 4
#define CE 3
#define USart_baud 9600
#define baud_prescale ((F_CPU/(USart_baud*16UL))-1)
uint8_t add_value[5];
void transferdata(uint8_t value);
void transferstatusdata(uint8_t valuesss);
uint8_t readdata(void);
void initialize(void);
static uint8_t dataobtained;
uint8_t dummy;
uint8_t *writedata(uint8_t RW,uint8_t reg,uint8_t *data_to_send,uint8_t length);
void sendackpackage(void);
uint8_t send_to_nrf(uint8_t values);
uint8_t GetREG(uint8_t reg);
void setnrf(uint8_t registers,uint8_t values_to_put);
int main(void)
{
 
  UBRRH=baud_prescale>>8;
	  UBRRL=baud_prescale;
	  UCSRC |=(1<<URSEL)|(3<<UCSZ0);
	   UCSRB |=(1<<TXEN)|(1<<RXEN);
	   DDRB|=(1<<SS)|(1<<CE)|(1<<MOSI)|(1<<SCK);
	   DDRB&=~(1<<MISO);
	   SPCR|=(1<<SPE)|(1<<MSTR)|(1<<SPR0);
	   _delay_ms(10);
	   SPCR&=~(1<<SPIE);
	SPCR&=~(1<<DORD);
	SPCR&=~(1<<CPOL);
	SPCR&=~(1<<SPR1);
	   PORTB|=(1<<SS);
	   _delay_ms(10);
	   PORTB&=~(1<<CE);
	   initialize();
    while (1) 
    {
         _delay_ms(100);
		setnrf(STATUS,0x07);
		_delay_ms(10);
		PORTB|=(1<<CE);
		_delay_ms(5);
		while ((GetREG(STATUS)&(1<<6))==0);
			setnrf(STATUS,0x07);
			PORTB&=~(1<<SS);
			_delay_ms(5);
			send_to_nrf(R_RX_PL_WID);
			 uint8_t length_of_array=readdata();
			_delay_ms(5);
			PORTB|=(1<<SS);
			_delay_ms(5);
			PORTB&=~(1<<SS);
			_delay_ms(5);
			send_to_nrf(R_RX_PAYLOAD);
			for (uint8_t i=0;i<length_of_array;i++)
			{
				dataobtained=readdata();
				transferdata(dataobtained);
			}
	        PORTB|=(1<<SS);
		_delay_ms(5);
		 PORTB&=~(1<<SS);
		_delay_ms(5);
		send_to_nrf(FLUSH_RX);
		PORTB|=(1<<SS);
		_delay_ms(5);
		PORTB&=~(1<<SS);
		send_to_nrf(FLUSH_TX);
		PORTB|=(1<<SS);
		sendackpackage();
    }
}
void initialize()
{
	_delay_ms(100);
   setnrf(EN_AA,0x01);
	_delay_ms(10);
	setnrf(EN_RXADDR,0x01);
	_delay_ms(10);
	setnrf(SETUP_AW,0x03);
	_delay_ms(10);
	setnrf(SETUP_RETR,0xFF);
	_delay_ms(10);
	setnrf(RF_SETUP,0x27);
	_delay_ms(10);
	setnrf(RF_CH,0x09);
	_delay_ms(10);
	setnrf(FEATURE,0x06);
	_delay_ms(10);
	setnrf(DYNPD,0x01);
	_delay_ms(10);
	PORTB&=~(1<<SS);
	 _delay_ms(10);
	send_to_nrf(W_REGISTER+TX_ADDR);
	_delay_ms(10);
	 for (char i=0;i<5;i++)
	{
			send_to_nrf(0xEE);
			_delay_ms(10);
	}
	PORTB|=(1<<SS);
	_delay_ms(10);
	PORTB&=~(1<<SS);
	 _delay_ms(10);
	send_to_nrf(W_REGISTER+RX_ADDR_P0);
	_delay_ms(10);
	 for (char i=0;i<5;i++)
	{
			send_to_nrf(0xEE);
			_delay_ms(10);
	}
	PORTB|=(1<<SS);
	setnrf(RX_PW_P0,0x05);
	_delay_ms(10);
	setnrf(CONFIG,0x7B);
	_delay_ms(100);
}
uint8_t send_to_nrf(uint8_t values)
{
	SPDR=values;
	while(!(SPSR&(1<<SPIF)));
	return SPDR;
}
uint8_t readdata()
{
	 SPDR=NOP;
	while(!(SPSR&(1<<SPIF)));
	return SPDR;
}
uint8_t GetREG(uint8_t reg)
{
	
	_delay_ms(10);
	PORTB&=~(1<<SS);
	_delay_ms(10);
	send_to_nrf(R_REGISTER+reg);
	_delay_ms(10);
	uint8_t value_of_data_return=send_to_nrf(dummy);
	_delay_ms(10);
	PORTB|=(1<<SS);
	_delay_ms(10);
	return value_of_data_return;
}
void transferstatusdata(uint8_t valuesss)
{
	char nos[3];
		char j=0;
		itoa(valuesss,nos,10);
		_delay_ms(20);
		while(j<3)
		{
			while(!(UCSRA&(1<<UDRE)));
			UDR=nos[j];
			_delay_ms(10);
			j++;
		}	
}
void setnrf(uint8_t registers,uint8_t values_to_put)
{
	_delay_ms(10);
	PORTB&=~(1<<SS);
	_delay_ms(5);
	send_to_nrf((W_REGISTER+registers));
	send_to_nrf(values_to_put);
	_delay_ms(5);
	PORTB|=(1<<SS);
	_delay_ms(10);
}
void transferdata(uint8_t value)
{
	
		while(!(UCSRA&(1<<UDRE)));
		UDR=value;
		_delay_ms(10);
}
uint8_t *writedata(uint8_t RW,uint8_t reg,uint8_t *data_to_send,uint8_t length)
{
	if (RW==1)
	{
		reg=reg+W_REGISTER;
	}
	_delay_ms(10);
	PORTB&=~(1<<SS);
	_delay_ms(10);
	send_to_nrf(reg);
	_delay_ms(10);
	static uint8_t value_return[5];
	for (uint8_t i=0;i<length;i++)
	{
		if (RW==0&reg!=W_TX_PAYLOAD)
		{
			value_return[i]=send_to_nrf(NOP);
		}
		else
		{
			send_to_nrf(data_to_send[i]);
		}
	}
	PORTB|=(1<<SS);
	return value_return;
}
void sendackpackage()
{
	_delay_ms(10);
	setnrf(STATUS,0x70);
	PORTB|=(1<<CE);
	PORTB&=~(1<<SS);
	send_to_nrf(W_ACK_PAYLOAD+0x00);
	send_to_nrf(0x11);
	_delay_ms(10);
	PORTB|=(1<<SS);
	PORTB&=~(1<<CE);
	_delay_ms(10);
}
