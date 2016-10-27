/*
 * myUART.h
 *
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
////UCAxCTL0 or USCI_Ax Control Register
//enum uart_parity{disabled, enabled=UCPEN};
//enum uart_parity_mode {odd, even=UCPAR};
//enum uart_endian {lsb, msb=UCMSB};
//enum uart_nr_data_bits {seven=UC7BIT, eight=0};									//bits
//enum uart_nr_stop_bits {one, two=UCSPB};
//enum uart_usc_mode_async{uart, idle_line=UCMODE0, adress_bit=UCMODE1, uart_auto_baud=(UCMODE1 | UCMODE0)}; 	//only valid when UCSYNC=0
//enum uart_sync_mode {async, sync};
//
//
////UCAxCTL1, USCI_Ax Control Register 1
//enum uart_clock_source {ext_clk, aclk, smclk1, smclk2};
//enum uart_char_err_rx_int {disabled, enabled};							//Characters received with errors are retained and UCAxRXIFG is set
//enum uart_break_char_mode {enabled, disabled};							//raises UCAxRXIFG
//enum uart_sleep_mode {disabled, enabled};
///*refers to next frame*/
//enum uart_tx_adr_marker {data, address}; 								//only valid for address-bit multiprocessor mode || UCTXADDR
//enum uart_transmit_break {no_break, is_break};
//enum uart_soft_reset {operating, reset};									//must be operating to enable the module
//
//
////UCAxSTAT, status of the module
//enum uart_loopback_listen{disabled, enabled};
//enum uart_frame_error {none, detected}
//enum uart_overrun_error {none, detected}; 								//a character was received and stored in UCAxRXBUF before it was read by software (i.e. character is dropped). Must not be cleared by software
//enum uart_parity_error {none, detected};
//enum uart_break_frame {none, detected};
//enum uart_char_error {none, detected};									//character received with an error
//enum uart_address_received {data, address}; 								//only in address-bit multiprocessor mode
//enum uart_idle_line {none, detected};									//only in idle-line multiprocessor mode
//enum uart_USCI_module_state{idle, working};
//
//
////SFR
//enum uart_transmit_interrupt {disabled, enabled};
//enum uart_receive_interrupt {disabled, enabled};
////SFR transmission state
//enum uart_transmit_state {not_ready, ready};
//enum uart_receive_state {not_ready, ready};

struct uart_baud_config {
	uint16_t clk_source;
	uint16_t UCBRx;
	uint16_t UCBRSx;
	uint16_t UCBRFx;
	uint16_t UCOS;

}typedef uart_baud_config;

struct uart_config {
	uint16_t parity;
	uint16_t parity_mode;
	uint16_t endian;
	uint16_t nr_data_bits;
	uint16_t nr_stop_bits;
	uint16_t usc_mode_async;
	uint16_t sync_mode;

	//UCAxCTL1, USCI_Ax Control Register 1
	uint16_t char_err_rx_int;
	uint16_t break_char_mode;
	uint16_t sleep_mode;
	/*refers to next frame*/
	uint16_t tx_adr_marker;
	uint16_t transmit_break;

	/*BaudRateConfiguration*/
	uart_baud_config * baud_rate;

}typedef uart_config;

int uart_default_init(void);
int uart_init(uart_config * uc);

int uart_putc(char data);
char uart_getc(void);

#endif /* UART_H_ */
