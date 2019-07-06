/*
 * LCD.h
 *
 * Created: 6/20/2019 7:52:42 PM
 *  Author: pesha
 */ 


#ifndef LCD_H_
#define LCD_H_


#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#define lcdlight		 PORTC
#define enablelcd		 5
#define controllcd		 PORTA
#define read_write		 7
#define registor_select	 2
void checking(void);
void initialize(void);
void send_command(unsigned char command);
void send_character( char character);
void check_if_busy(void);
void printstring(unsigned char * stringpointer);
#endif /* LCD_H_ */