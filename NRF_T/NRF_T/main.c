
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
 char tempr_animal[3];
 char pulse_animal[3];
 char value_of_tempr[2];
 char value_of_humidity[3];
 char location[28];
        uint8_t calculate_heartbeat=0;
		int pulse_rate=0;
	    uint16_t temperature_in_voltage_format;
		uint16_t heartbeat_in_voltage_format;
	 	uint8_t thelow;
	 	uint8_t tempr_of_animal;
	 	uint8_t temperature=0;
	 	uint8_t temperature1=0;
	 	uint8_t humidity=0;
	 	uint8_t humidity1=0;
	 	uint8_t checksum=0;
	 	uint8_t sum=0;
	 	uint8_t arrange_array=0;
		 uint8_t ther=0;
		 uint8_t max_peak=0;
void sendtranfer_using_usart(uint16_t heart);
 void getlocation(void);
void setnrf(uint8_t registers,uint8_t values_to_put);
void transferstatusdata(uint8_t values);
uint8_t getvalue(uint8_t read_status);
void send_chunck_of_data(char *,uint8_t);
void reset(void);
unsigned char return_char(void);
int main(void)
{
     char animalId[]="iHUM001";
	char contain_both_humidity_tempr[32];
	 DDRA&=~(1<<1);
	 ADCSRA |=(1<<ADPS2);//this bit is set when we want to divide CLK frequency by 8
	 ADMUX |=(1<<REFS0)|(1<<ADLAR);//REFSO set ref voltage to VCC and ADLAR is use for left shifting values in ADCH and ADCL register
	 ADCSRA |=(1<<ADEN); //this is use for enabling ADC
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
		
			  arrange_array=0;
			  pulse_rate=0;
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
		      if(sum>=0 && sum<=3)
		      {
				     for(uint8_t i=0;i<sizeof(animalId);i++)
			      {
					  
				      contain_both_humidity_tempr[arrange_array]=animalId[i];
					  arrange_array++;
			      }
			      itoa(temperature,value_of_tempr,10);
				    _delay_ms(10);
			      contain_both_humidity_tempr[arrange_array]=0x73;
				  arrange_array++;
			      for(uint8_t i=0;i<2;i++)
			      {
				      contain_both_humidity_tempr[arrange_array]=value_of_tempr[i];
					  arrange_array++;
			      }
			      itoa(humidity,value_of_humidity,10);
			      _delay_ms(10);
			      contain_both_humidity_tempr[arrange_array]=0x68;
				  arrange_array++;
			      for(uint8_t i=0;i<3;i++)
			      {
					  
				      contain_both_humidity_tempr[arrange_array]=value_of_humidity[i];
					  arrange_array++;
			      }
		      }
		      _delay_ms(1350);
			  ADMUX|=(1<<MUX0);
			  ADMUX&=~(1<<MUX1);
			  _delay_ms(10);
			  ADCSRA|=(1<<ADSC);
			 _delay_ms(20);
			 ADCSRA&=~(1<<ADSC);
			  thelow=ADCL;// done so that lag doesn't occur while running as we need to access ADCH and ADCL at same time
			  temperature_in_voltage_format= (ADCH<<2)|(thelow>>6);//this is use to get 10 bit data from ADC
			  tempr_of_animal=temperature_in_voltage_format/2;
			  itoa(temperature_in_voltage_format,tempr_animal,10);
			  _delay_ms(10);
			   contain_both_humidity_tempr[arrange_array]=0x74;
			    arrange_array++; 
			  for (uint8_t i=0;i<3;i++)
			  {
				   contain_both_humidity_tempr[arrange_array]=tempr_animal[i];
				    arrange_array++; 
			  }
			    ADMUX&=~(1<<MUX0);
			    ADMUX|=(1<<MUX1);
			    _delay_ms(10);
				 TCNT1H=0x1B;
				 TCNT1L=0x1D;
				 TCCR1A|=0x00;
				 TCCR1B|=(1<<CS12)|(1<<CS10);
				  ADCSRA|=(1<<ADSC);
				 _delay_ms(20);
				while((TIFR&(1<<TOV1))==0)
				{
					 thelow=ADCL;// done so that lag doesn't occur while running as we need to access ADCH and ADCL at same time
					 heartbeat_in_voltage_format= (ADCH<<2)|(thelow>>6);//this is use to get 10 bit data from ADC
					 sendtranfer_using_usart(heartbeat_in_voltage_format);
							  if (heartbeat_in_voltage_format>=255)
							  {
								  pulse_rate++;
								  _delay_ms(500);
							  }
					
					 ADCSRA|=(1<<ADSC);
					 _delay_ms(10);
				}
				TIFR|=(1<<TOV1);
				ADCSRA&=~(1<<ADSC);
				pulse_rate=pulse_rate*2;
				 itoa(pulse_rate,pulse_animal,10);
				 _delay_ms(10);
			         contain_both_humidity_tempr[arrange_array]=0x70;
					 arrange_array++;
					 for (uint8_t i=0;i<3;i++)
					 {
						 contain_both_humidity_tempr[arrange_array]=pulse_animal[i];
						 arrange_array++;
					 }
			   _delay_ms(10);
		        reset();
		      _delay_ms(10);
		      send_chunck_of_data(contain_both_humidity_tempr,arrange_array);
		      _delay_ms(50);
			  flush_every();
			   _delay_ms(10);
			   getlocation();
			   _delay_ms(10);
			   reset();
			   _delay_ms(10);
			   send_chunck_of_data(location,28);
			   _delay_ms(50);
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
	setnrf(RF_SETUP,0x27);
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
		RWdata(data_to_send[i]);
				_delay_ms(10);
	}
	PORTB|=(1<<SS);
	_delay_ms(10);
	PORTB|=(1<<CE);
    _delay_ms(30);
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
void getlocation()
{
	char i=0;
	char j=1;
	location[0]=0x67;
		char takeallvalue=0;
		while(takeallvalue<=3)
		{
			i=return_char();
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
										while (takeallvalue<=3)
										{
											i=return_char();
											if(i==0x2C)
											{
												takeallvalue++;
												if (takeallvalue==2)
												{
													location[j]=i;
													j++;
												}
											}
											else
											{
												location[j]=i;
												j++;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		location[j]=0x0D;
		j++;
		location[j]=0x0A;
}
unsigned char return_char()
{
	while (!(UCSRA&(1<<RXC)));//wait until UDR register contain data received from other device
	return UDR;// return value obtained from other device through Rx pin
}
void sendtranfer_using_usart(uint16_t heart)
{
	char datas[4];
	itoa(heart,datas,10);
     for (uint8_t i=0;i<4;i++)
     {
		 while(!(UCSRA&(1<<UDRE)));
		 UDR=datas[i];
     }
	 while(!(UCSRA&(1<<UDRE)));
		 UDR=0x0D;
		 while(!(UCSRA&(1<<UDRE)));
		 UDR=0x0A;
}