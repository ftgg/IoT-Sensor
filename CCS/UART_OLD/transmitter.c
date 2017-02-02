#include <msp430.h>
#include "uart.h"


int main_t_OUT(void) {
	WDTCTL = WDTPW | WDTHOLD;					// Stop watchdog timer
	char device = 1;
	uart_default_init(device);

	char i = 0;
	while (1) {
		uart_putc(device, i++);
		volatile int i = 10000;					// SW Delay
		do i--;
		while(i != 0);
	}

	return 0;
}
