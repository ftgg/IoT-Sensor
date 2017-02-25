#include <msp430.h>
#include "uart.h"
#include <string.h>


void writeToLede (char data){
	PJOUT = data & 0x0F;
	P3OUT = data & 0xF0;
	return;
}


int main_t(void) {
	WDTCTL = WDTPW | WDTHOLD;					// Stop watchdog timer
	char device = 0;
	uart_default_init(device);

	PJDIR = 0x0F;
	P3DIR = 0xF0;


	char * buff;
	while (1) {
		char c;
		c = uart_getc(device);
		writeToLede(c);
		uart_putc(device, c);
		//uart_readLine(device, &buff);
		//size_t len = strlen(buff);
		//uart_write(device, buff, len);
	}

	return 0;
}
