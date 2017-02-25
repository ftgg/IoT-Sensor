#include <msp430.h>
#include "uart.h"



void writeToLed (char data){
	PJOUT = data & 0x0F;
	P3OUT = data & 0xF0;
	return;
}


/*
 * FRAM                    : origin = 0xC200, length = 0x3D80
 */





#pragma NOINIT(uart_fram_write_buffer_0)
unsigned int uart_fram_write_buffer_0[256] ;

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	char data;
	char device = 0;
	PJDIR = 0x0F;
	P3DIR = 0xF0;
/*
	int a = 0;
	for(a = 0; a < 256; a = a + 2){
		(*(unsigned int*)(0xC200 + a)) = 0xFFFF;
		uart_fram_write_buffer_0[a] = (*(unsigned int*)(0xC200 + a));
	}


	uart_fram_write_buffer_0[0] = 7777;


	return 0;
	*/
	if(uart_default_init(device))
		return 1;


	char i = 0;
	while (1) {
		uart_putc(device, 'A');
		//data = uart_getc(device);
		writeToLed(data);
		volatile int i = 10000;					// SW Delay
		do i--;
		while(i != 0);
	}

	return 0;
}
