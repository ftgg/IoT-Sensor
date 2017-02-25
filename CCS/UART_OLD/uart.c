/*
 * myUART.c
 *
 */

#include <stddef.h>
#include "uart.h"
#include <msp430.h>

#define NUMBER_OF_INTERFACES 2

struct uart_interface {
	volatile unsigned int * usci_ctrl_w;
	volatile unsigned int * bitrate_ctrl_w;
	volatile unsigned int * modulation_ctrl_w;
	volatile unsigned int * interrupt_enable;
	volatile unsigned int * UCAxIFG;
	volatile unsigned int * UCAxTXBUF;
	volatile unsigned int * UCAxRXBUF;
	unsigned int TX_Pin;
	unsigned int RX_Pin;

}typedef uart_interface;

static uart_interface ui_list[NUMBER_OF_INTERFACES];
char buffer[RCV_BUFFERSIZE]; 	/*Interface Receive Buffer*/


int uart_default_init(char interface) {
	uart_baud_config baud = { UCSSEL1, 52, 0x49, 1, 1 };

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

	if(uart_init(interface, &uconf))
		return 1;
	return 0;
}

static void init_uart_interface_descriptor() {
	static char initialized;

	if(initialized)
		return;

	ui_list[0] = (uart_interface){&UCA0CTLW0, &UCA0BRW, &UCA0MCTLW, &UCA0IE, &UCA0IFG, &UCA0TXBUF, &UCA0RXBUF, BIT0, BIT1};
	ui_list[1] = (uart_interface){&UCA1CTLW0, &UCA1BRW, &UCA1MCTLW, &UCA1IE, &UCA1IFG, &UCA1TXBUF, &UCA1RXBUF, BIT5, BIT6};

	initialized = 1;
}

int uart_init(char ui_id, uart_config * uc) {

	/* Check index bounds*/
	if(ui_id >= NUMBER_OF_INTERFACES)
		return 1;

	init_uart_interface_descriptor();

	/*Clear Register and set resetmode*/
	*(ui_list[ui_id].usci_ctrl_w) = 1;

	/*Init UART Registers*/
	*(ui_list[ui_id].usci_ctrl_w) |= uc->parity | uc->parity_mode;
	*(ui_list[ui_id].usci_ctrl_w) |= uc->endian | uc->nr_data_bits;
	*(ui_list[ui_id].usci_ctrl_w) |= uc->nr_stop_bits | uc->usc_mode_async;
	*(ui_list[ui_id].usci_ctrl_w) |= uc->parity_mode;
	*(ui_list[ui_id].usci_ctrl_w) |= uc->baud_rate->clk_source;
	//UCA0CTLW0 |= UCMSB;
	/*Set Baudrate Settings*/

	*(ui_list[ui_id].bitrate_ctrl_w) = uc->baud_rate->UCBRx;
	*(ui_list[ui_id].modulation_ctrl_w) |= uc->baud_rate->UCOS;
	*(ui_list[ui_id].modulation_ctrl_w) |= uc->baud_rate->UCBRFx << 4;
	*(ui_list[ui_id].modulation_ctrl_w) |= uc->baud_rate->UCBRSx << 8;

	/*Interrupt enable*/
	*(ui_list[ui_id].interrupt_enable) &= 0xFFF0;
	*(ui_list[ui_id].interrupt_enable) |= 0;//uc->uart_startbit_interrupt;
	*(ui_list[ui_id].interrupt_enable) |= uc->uart_transmit_complete_interrupt;
	*(ui_list[ui_id].interrupt_enable) |= uc->uart_receive_interrupt;
	*(ui_list[ui_id].interrupt_enable) |= uc->uart_transmit_interrupt;

	/* Configure Pins */
	P2SEL0 &= ~ui_list[ui_id].TX_Pin;
	P2SEL1 |= ui_list[ui_id].TX_Pin;
	P2SEL0 &= ~ui_list[ui_id].RX_Pin;
	P2SEL1 |= ui_list[ui_id].RX_Pin;

	/*Unset resetmode*/
	*(ui_list[ui_id].usci_ctrl_w) &= ~1;

	return 0;
}

int uart_putc(char interface_id, char data) {

	if(interface_id >= NUMBER_OF_INTERFACES)
		return 1;

	while (!(*(ui_list[interface_id].UCAxIFG) & UCTXIFG))
		;
	*(ui_list[interface_id].UCAxTXBUF) = data;
	return 0;
}

char uart_getc(char interface_id) {

	if(interface_id >= NUMBER_OF_INTERFACES)
		return -1;

	while (!(*(ui_list[interface_id].UCAxIFG) & UCRXIFG))
		;
	char data = *(ui_list[interface_id].UCAxRXBUF);
	return data;
}


int uart_write(char interface_id, char * buffer, int length){

	if(interface_id >= NUMBER_OF_INTERFACES || length < 0)
		return 1;
	int i;
	for(i = 0; i < length; i++)
		uart_putc(interface_id,buffer[i]);
	return 0;
}

int uart_read(char interface_id, char * buffer, int length){
	if(interface_id >= NUMBER_OF_INTERFACES || length < 0)
		return 1;
	int i;
	for(i = 0; i < length; i++)
		buffer[i] = uart_getc(interface_id);
	return 0;
}

int uart_readLine(char interface_id, char ** buffer){
	if(interface_id >= NUMBER_OF_INTERFACES)
		return 1;

	char *  bff= buffer; //ui_list[interface_id].buffer;
	int i = 0;
	int lineExceeds = 1;
	for(i = 0; i < RCV_BUFFERSIZE-1; i++){
		bff[i] = uart_getc(interface_id);
		if(bff[i] == '\n' && bff[i - 1] == '\r'){
			i = i -1;
			lineExceeds = 0;
			break;				// Windows
		} else if (bff[i] == '\n'){
			lineExceeds = 0;
			break;				// Unix
		}
	}
	bff[i] = '\0';
	*buffer = bff;
	return lineExceeds;

}
