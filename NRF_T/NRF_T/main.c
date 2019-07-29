
#define F_CPU 2000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "nrf1.h"
#include "dht11.h"
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
void flush_every(void);
char animalId[]="CAT00001";

void setnrf(uint8_t registers,uint8_t values_to_put);
void transferstatusdata(uint8_t values);
uint8_t getvalue(uint8_t read_status);
void send_chunck_of_data(char *,uint8_t);
void reset(void);
int main(void)
{

	uint8_t temperature=0;
	uint8_t temperature1=0;
	uint8_t humidity=0;
	uint8_t humidity1=0;
	uint8_t checksum=0;
	uint8_t sum=0;
	uint8_t hh=0;
	 DDRA|=(1<<1)|(1<<2);
	  UBRRH=baud_prescale>>8;
	  UBRRL=baud_prescale;
	  UCSRC |=(1<<URSEL)|(3<<UCSZ0);
	  UCSRB |=(1<<TXEN)|(1<<RXEN);
	
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
		unsigned char value_of_tempr[2];
        unsigned char value_of_humidity[3];
		unsigned char *contain_both_humidity_tempr;
		      reset();
		      _delay_ms(10);
		      send_chunck_of_data(animalId,sizeof(animalId));
		      _delay_ms(150);
		      flush_every();
		      _delay_ms(10);
		      DDRC|=(1<<PINC0);
		      PORTC|=(1<<PINC0);
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
			      itoa(temperature,value_of_tempr,10);
			      _delay_ms(10);
			      contain_both_humidity_tempr[0]=0x74;
			      contain_both_humidity_tempr[1]=0x65;
			      for(uint8_t i=0;i<2;i++)
			      {
				      contain_both_humidity_tempr[2+i]=value_of_tempr[i];
			      }
			      itoa(humidity,value_of_humidity,10);
			      _delay_ms(10);
			      contain_both_humidity_tempr[4]=0x68;
			      contain_both_humidity_tempr[5]=0x75;
			      for(uint8_t i=1;i<5;i++)
			      {
				      contain_both_humidity_tempr[5+i]=value_of_humidity[i-1];
			      }
		      }
		      _delay_ms(1300);
		      reset();
		      _delay_ms(10);
		      send_chunck_of_data(contain_both_humidity_tempr,9);
		      _delay_ms(150);
		      flush_every();
		      _delay_ms(10);
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
	setnrf(RF_SETUP,0x24);
	_delay_ms(10);
	setnrf(RF_CH,0X09);
	_delay_ms(10);
	setnrf(FEATURE,0x06);
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
void send_chunck_of_data(char *data_to_send,uint8_t lengths)
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
	RWdata(0xA0);
    _delay_ms(10);
	for(uint8_t i=0;i<lengths;i++)
	{
		RWdata(*data_to_send);
		_delay_ms(10);
		data_to_send++;
	}
	PORTB|=(1<<SS);
	_delay_ms(10);
	PORTB|=(1<<CE);
    _delay_ms(20);
	PORTB&=~(1<<CE);
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
void flush_every()
{
	PORTB&=~(1<<SS);
			_delay_ms(5);
			RWdata(FLUSH_TX);
			PORTB|=(1<<SS);
			_delay_ms(5);
			PORTB&=~(1<<SS);
			_delay_ms(5);
			RWdata(FLUSH_RX);
			PORTB|=(1<<SS);
	
}