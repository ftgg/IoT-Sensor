/*
 * uart.h
 *
 *  Created on: 25.01.2017
 *      Author: edc42
 */

#ifndef UART_H_
#define UART_H_

# define RCV_BUFFERSIZE 256

/*
 * Initialize uart interface
 */
void init_uart_registers(char interface_id);

/*
 * Writes a chraracter
 */
void uart_putc(char interface_id, char data);

/*
 * Returns a character
 */
char uart_getc(char interface_id);

/*
 * Write length bytes from buffer
 */
int uart_write(char interface_id, char * buffer, int length);
/*
 * Writes the char array buffer to the given interface.
 * the array requires an terminating null sign
 */
void uart_writeLine(char interface_id, char * buffer);

/*
 * Read lenght characters and writes them into buffer
 */
int uart_read(char interface_id, char * buffer, int length);


/*
 * reads a line from the given interface ID
 *  - Returns 0 when received \n or \r else.
 *  if there are more than 255 characters in one line 1 is returned
 *
 * 	A pointer to the received Data will be stored in buffer
 * 	uart_read_line will always return the same pointer.
 * 	The user has to copy the received data into another array, before invoking uart_readLine one more Time.
 */
int uart_readLine(char interface_id, char ** buffer);


#endif /* UART_H_ */
