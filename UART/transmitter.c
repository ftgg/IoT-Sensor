#include <msp430.h>
#include "uart.h"


int main(void) {
	WDTCTL = WDTPW | WDTHOLD;					// Stop watchdog timer

	uart_default_init();

	char i = 0;
	while (1) {
		uart_putc(i++);
		volatile int i = 10000;					// SW Delay
		do i--;
		while(i != 0);
	}

	return 0;
}
