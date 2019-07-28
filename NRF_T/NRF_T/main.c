
#define F_CPU 2000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "nrf1.h"
#define MOSI 5
#define SCK 7
#define SS 4
#define CE 3
#define MISO 6
#define USart_baud 9600
#define baud_prescale ((F_CPU/(USart_baud*16UL))-1)
char RWdata(char value_send);
void initialize(void);
uint8_t add_value[5];
void setnrf(uint8_t registers,uint8_t values_to_put);
void transferstatusdata(uint8_t values);
uint8_t *send_data(uint8_t RW,uint8_t reg,uint8_t *reg_value,uint8_t length);
uint8_t getvalue(uint8_t read_status);
void send_chunck_of_data(uint8_t ,uint8_t);
void reset(void);
int main(void)
{
	 DDRA|=(1<<1)|(1<<2);
	  UBRRH=baud_prescale>>8;
	  UBRRL=baud_prescale;
	  UCSRC |=(1<<URSEL)|(3<<UCSZ0);
	  UCSRB |=(1<<TXEN)|(1<<RXEN);
	uint8_t datas_to_go=0x41;
	 DDRB|=(1<<SS)|(1<<CE)|(1<<MOSI)|(1<<SCK);
	 DDRB&=~(1<<MISO);
    SPCR|=(1<<SPE)|(1<<MSTR);
	  SPCR&=~(1<<SPIE);
	SPCR&=~(1<<DORD);
	SPCR&=~(1<<CPOL);
	SPCR&=~(1<<SPR1);
	PORTB|=(1<<SS);
	PORTB&=~(1<<CE);
	initialize();
    while (1) 
    {
		     reset();
		    _delay_ms(10);
		    transferstatusdata(getvalue(CONFIG));
		    _delay_ms(10);
		    send_chunck_of_data(datas_to_go,1);
	    	_delay_ms(500);
			PORTB&=~(1<<SS);
			_delay_ms(10);
			RWdata(FLUSH_TX);
			_delay_ms(10);
			PORTB|=(1<<SS);
			_delay_ms(10);
			PORTB&=~(1<<SS);
			_delay_ms(10);
			RWdata(FLUSH_RX);
			_delay_ms(10);
			PORTB|=(1<<SS);
			datas_to_go++;
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
	setnrf(SETUP_RETR,0x3F);
	_delay_ms(10);
	setnrf(RF_SETUP,0x27);
	_delay_ms(10);
	setnrf(RF_CH,0X09);
	_delay_ms(10);
	setnrf(FEATURE,0x05);
	_delay_ms(10);
	setnrf(DYNPD,0x01);
	_delay_ms(10);
	PORTB&=~(1<<SS);
	_delay_ms(10);
	RWdata(TX_ADDR+W_REGISTER);
	_delay_ms(10);
	for (char i=0;i<5;i++)
	{
		RWdata(0xEE);
		_delay_ms(10);
	}
	PORTB|=(1<<SS);
	_delay_ms(10);
	_delay_ms(10);
	PORTB&=~(1<<SS);
	_delay_ms(10);
	RWdata(RX_ADDR_P0+W_REGISTER);
	_delay_ms(10);
	for (char i=0;i<5;i++)
	{
		RWdata(0xEE);
		_delay_ms(10);
	}
	PORTB|=(1<<SS);
	_delay_ms(10);
	setnrf(RX_PW_P0,0x05);
	_delay_ms(10);
	setnrf(CONFIG,0x7A);
	_delay_ms(100);
}
char RWdata(char value_send)
{
	
	SPDR=value_send;
	while(!(SPSR&(1<<SPIF)));
	return SPDR;
}
uint8_t *send_data(uint8_t RW,uint8_t reg,uint8_t *reg_value,uint8_t length)
{
	if (RW==1)
	{
		reg=(reg+W_REGISTER);
	}
	else
	{
		reg=(reg+R_REGISTER);
	}
	_delay_ms(10);
	PORTB&=~(1<<SS);
	_delay_ms(10);
	RWdata(reg);
	_delay_ms(10);
	static uint8_t value_obtained[1];//static to be written to return 
	for (char i=0;i<length;i++)
	{
		if(RW==0&reg!=W_TX_PAYLOAD)
		{
			value_obtained[i]=RWdata(NOP);//sending dummy value to obtain data from the register as we need to send address bit to obtain data
	     	_delay_ms(10);
		}
		else
		{
			RWdata(reg_value[i]);
			_delay_ms(10);
		}
	}
	PORTB|=(1<<SS);
	_delay_ms(10);
	return value_obtained;
}
uint8_t getvalue(uint8_t read_reg)
{
	_delay_ms(10);
	PORTB&=~(1<<SS);
	_delay_ms(10);
	RWdata(R_REGISTER+read_reg);
	_delay_ms(10);
	uint8_t value_of_data_return=RWdata(NOP);
	_delay_ms(10);
	PORTB|=(1<<SS);
	_delay_ms(10);
	return value_of_data_return;
}
void send_chunck_of_data(uint8_t data_to_send,uint8_t lengths)
{
	_delay_ms(10);
	PORTB&=~(1<<SS);
	_delay_ms(10);
	RWdata(FLUSH_TX);
	_delay_ms(10);
	PORTB|=(1<<SS);
	_delay_ms(10);
	PORTB&=~(1<<SS);
	_delay_ms(10);
	RWdata(0xB0);
    _delay_ms(10);
	for(uint8_t i=0;i<10;i++)
	{
		RWdata(0x41+i);
		_delay_ms(10);
	}
	PORTB|=(1<<SS);
	_delay_ms(10);
	PORTB|=(1<<CE);
    _delay_ms(20);
	PORTB&=~(1<<CE);
	_delay_ms(10);
}
void reset()
{
		_delay_ms(10);
		PORTB&=~(1<<SS);
		_delay_ms(10);
		RWdata((W_REGISTER|STATUS));
		_delay_ms(10);
		RWdata(0x70);
		_delay_ms(10);
		PORTB|=(1<<SS);
        _delay_ms(10);
}
void transferstatusdata(uint8_t values)
{
	char nos[3];
	char j=0;
	itoa(values,nos,10);
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
	_delay_ms(10);
	RWdata((W_REGISTER+registers));
	_delay_ms(10);
	RWdata(values_to_put);
	_delay_ms(10);
	PORTB|=(1<<SS);
	_delay_ms(10);
}