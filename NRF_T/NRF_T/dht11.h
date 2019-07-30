/*
 * dht11.h
 *
 * Created: 7/28/2019 11:47:25 PM
 *  Author: pesha
 */ 


#ifndef DHT11_H_
#define DHT11_H_
#define  F_CPU 2000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#define inputport   PORTC //setting input port to port A
void start_conversion(void);
void response(void);
uint8_t receiving_data(void);
#endif /* DHT11_H_ */