#include <msp430.h>
#include "uart.h"



void writeToLed (char data){
	PJOUT = data & 0x0F;
	P3OUT = data & 0xF0;
	return;
}

int main(void) {
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	char data;

	PJDIR = 0x0F;
	P3DIR = 0xF0;

	uart_default_init();

	P2SEL0 &= ~BIT5;
	P2SEL1 |= BIT5;

	P2SEL0 &= ~BIT6;
	P2SEL1 |= BIT6;

	while (1) {
		data = uart_getc();
		writeToLed(data);
	}

	return 0;
}

