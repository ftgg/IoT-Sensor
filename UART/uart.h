/*
 * myUART.h
 *
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>

////UCAxCTL1, USCI_Ax Control Register 1
//enum uart_soft_reset {operating, reset};									//must be operating to enable the module

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

////SFR
//enum uart_transmit_interrupt {disabled, enabled};
//enum uart_receive_interrupt {disabled, enabled};
////SFR transmission state
//enum uart_transmit_state {not_ready, ready};
//enum uart_receive_state {not_ready, ready};

struct uart_baud_config {
	uint16_t clk_source;	//ext_clk, aclk, smclk1, smclk2
	/*can be found in Baudratetable (PDF)*/
	uint16_t UCBRx;
	uint16_t UCBRSx;
	uint16_t UCBRFx;
	uint16_t UCOS;

}typedef uart_baud_config;

struct uart_config {
	////UCAxCTL0 or USCI_Ax Control Register
	uint16_t parity;			//enabled=UCPEN
	uint16_t parity_mode;		//odd=0, even=UCPAR
	uint16_t endian;			//msb first=UCMSB
	uint16_t nr_data_bits;		//seven=UC7BIT, eight=0
	uint16_t nr_stop_bits;		//one=0, two=UCSPB
	uint16_t usc_mode_async;	//uart, idle_line=UCMODE0, adress_bit=UCMODE1, uart_auto_baud=(UCMODE1 | UCMODE0) only valid when UCSYNC=0
	uint16_t sync_mode;			//async, sync

	//UCAxCTL1, USCI_Ax Control Register 1
	uint16_t char_err_rx_int;	//disabled, enabled
	uint16_t break_char_mode;	//enabled, disabled		//raises UCAxRXIFG
	uint16_t sleep_mode;		//disabled, enabled
	/*refers to next frame*/
	uint16_t tx_adr_marker;		//data, address			//only valid for address-bit multiprocessor mode || UCTXADDR
	uint16_t transmit_break;	//no_break, is_break

	/*BaudRateConfiguration*/
	uart_baud_config * baud_rate;

}typedef uart_config;

/*
 * Default initialization for UART with 8 Bit data, 1 stop Bit and Baudrate = 9600
 */
int uart_default_init(void);

/*
 * initialization for other settings
 */
int uart_init(uart_config * uc);

/*
 * Writes a Byte of data to output Buffer
 */
int uart_putc(char data);

/*
 * returns a Byte wich is currently in input Buffer
 */
char uart_getc(void);

#endif /* UART_H_ */
