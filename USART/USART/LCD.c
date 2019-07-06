/*
 * LCD.c
 *
 * Created: 6/20/2019 7:54:10 PM
 *  Author: pesha
 */ 
#include "LCD.h"
void initialize(void)
{	
	DDRA |=(1<<read_write)|(1<<registor_select)|(1<<enablelcd);
	_delay_ms(15);
	send_command(0x01);//clear screen command as command are in binary or hex format
	_delay_ms(2);
	send_command(0x38);//for lcd on 8 bit mode
	_delay_us(50);
	send_command(0b00001110);//control display and cursor
	/*
	4 bit to use this function this bit should be high
	3 bit to tell if display should be on or off
	2 bit to display cursor
	1 bit in order to blink cursor
	*/
	_delay_us(50);
	send_command(0x80);
	_delay_us(50);
	
}
void checking(void)
{
	controllcd |=1<<enablelcd;
	asm volatile("nop");
	asm volatile("nop");
	controllcd &=~(1<<enablelcd);
}
void send_command(unsigned char command)
{
	check_if_busy();
	lcdlight=command;
	controllcd &=~(1<<read_write|1<<registor_select);
	checking();
	lcdlight=0;
}
void send_character( char character)
{
	check_if_busy();
	lcdlight=character;
	controllcd &=~(1<<read_write);
	controllcd |=(1<<registor_select);
	checking();
	lcdlight=0;
}
void check_if_busy(void)
{
	DDRC=0;
	controllcd |=(1<<read_write);
	controllcd &=~(1<<registor_select);
	while( lcdlight >= 0x80)
	{
		checking();
	}
	DDRC=0xff;
}
void printstring(unsigned char *stringpointer)
{
	while(*stringpointer>0)
	{
		send_character(*stringpointer);
		stringpointer++;
	}
}
