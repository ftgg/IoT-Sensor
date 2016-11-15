/*
 * myUART.c
 *
 */

#include <stddef.h>
#include "uart.h"
#include <msp430.h>

struct uart_interface {
	unsigned int * usci_ctrl_w;
	unsigned int * bitrate_ctrl_w;
	unsigned int * modulation_ctrl_w;
	unsigned int * interrupt_enable;

}typedef uart_interface;

int uart_default_init() {
	uart_baud_config baud = { UCSSEL1, 3, 0x92, 0, 0 };

	uart_config uconf;
	uconf.baud_rate = &baud;
	uconf.break_char_mode = 0; //~UCBRKIE;
	uconf.char_err_rx_int = 0; //UCRXEIE;
	uconf.endian = 0; //~UCMSB;
	uconf.nr_data_bits = 0; //~UC7BIT;
	uconf.nr_stop_bits = 0; //~UCSPB;
	uconf.parity = 0; //~UCPEN;
	uconf.parity_mode = 0; //~UCPAR;
	uconf.sleep_mode = 0; //~UCDORM;
	uconf.sync_mode = 0; //~UCSYNC;
	uconf.transmit_break = 0; //~UCTXBRK;
	uconf.tx_adr_marker = 0; //~UCTXADDR;
	uconf.usc_mode_async = UCMODE_0;

	P2SEL0 &= ~BIT5;
	P2SEL1 |= BIT5;

	P2SEL0 &= ~BIT6;
	P2SEL1 |= BIT6;

	uart_init(&uconf);
}

int get_uart_interface(char index, uart_interface * ui) {
	const struct uart_interface interface_list[] = { { UCA0CTLW0, UCA0BRW,
			UCA0MCTLW, UCA0IE }, { UCA1CTLW0, UCA1BRW, UCA1MCTLW, UCA1IE } };

	if (index <0 || index >1)
		return -1;

	ui->bitrate_ctrl_w = interface_list[index].bitrate_ctrl_w;
	ui->interrupt_enable = interface_list[index].interrupt_enable;
	ui->modulation_ctrl_w = interface_list[index].modulation_ctrl_w;
	ui->usci_ctrl_w = interface_list[index].usci_ctrl_w;

	return 0;
}

int uart_init(uart_config * uc) {

	/*Clear Register and set resetmode*/
	UCA1CTLW0 = 1;

	/*Init UART Registers*/
	UCA1CTLW0 |= uc->parity | uc->parity_mode;
	UCA1CTLW0 |= uc->endian | uc->nr_data_bits;
	UCA1CTLW0 |= uc->nr_stop_bits | uc->usc_mode_async;
	UCA1CTLW0 |= uc->parity_mode;
	UCA1CTLW0 |= uc->baud_rate->clk_source;

	/*Set Baudrate Settings*/
	UCA1BRW = uc->baud_rate->UCBRx;
	UCA1MCTLW |= uc->baud_rate->UCOS;
	UCA1MCTLW |= uc->baud_rate->UCBRFx << 4;
	UCA1MCTLW |= uc->baud_rate->UCBRSx << 8;

	/*Interrupt enable*/
	UCA1IE &= 0xFFF0;
	UCA1IE |= uc->uart_startbit_interrupt;
	UCA1IE |= uc->uart_transmit_complete_interrupt;
	UCA1IE |= uc->uart_receive_interrupt;
	UCA1IE |= uc->uart_transmit_interrupt;

	UCA1CTLW0 &= ~1;

	return 0;
}

int uart_putc(char data) {

	while (!(UCA1IFG & UCTXIFG))
		;
	UCA1TXBUF = data;
	return 0;
}

char uart_getc() {
	while (!(UCA1IFG & UCRXIFG))
		;
	char data = UCA1RXBUF;
	return data;
}
