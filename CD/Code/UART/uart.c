#include <msp430.h> 
#include "uart.h"

/*
 * main.c
 */


/*Registers*/
volatile unsigned int * UCAxCTLW0;	//uart settings						slau272  p.515
volatile unsigned int * UCAxMCTLW;	//Baud								slau272  p.497
volatile unsigned int * UCAxBRW;    //Baud								slau272  p.517
volatile unsigned int * UCAxIFG;	//Interrupt Flags pending)			slau272  p.503
volatile unsigned int * UCAxTXBUF;	//Interrupt Flags pending)      	slau272  p.499
volatile unsigned int * UCAxRXBUF;  //Interrupt Flags pending)      	slau272  p.499
volatile unsigned char * PxSEL0;    // Select pin functionality 		slau272c p.316
volatile unsigned char * PxSEL1;    // Select pin functionality			slau272c p.316


/*Dev Uart pins*/
char uart_pins;

/*Current Interface*/
char current_interface = -1;

/*declare Receive Buffer*/
char rcv_buffer[RCV_BUFFERSIZE];

void init_uart_registerpointers(char devid){
	if(devid == 0){
		// UCA0
		UCAxCTLW0 = &UCA0CTLW0;
		UCAxMCTLW = &UCA0MCTLW;
		UCAxBRW = &UCA0BRW;
		UCAxIFG = &UCA0IFG;
		UCAxTXBUF = &UCA0TXBUF;
		UCAxRXBUF = &UCA0RXBUF;
		PxSEL0 = &P2SEL0;
		PxSEL1 = &P2SEL1;
		uart_pins = BIT0 | BIT1; // Port2

		current_interface = 0;
	}else{
		// UCA1
		UCAxCTLW0 = &UCA1CTLW0;
		UCAxMCTLW = &UCA1MCTLW;
		UCAxBRW = &UCA1BRW;
		UCAxIFG = &UCA1IFG;
		UCAxTXBUF = &UCA1TXBUF;
		UCAxRXBUF = &UCA0RXBUF;
		PxSEL0 = &P2SEL0;
		PxSEL1 = &P2SEL1;
		uart_pins = BIT5 | BIT6; // Port2

		current_interface = 1;
	}
}


void init_uart_registers(char interface_id){
	/*Change register pointers if necessary*/
	if(current_interface != interface_id)
		init_uart_registerpointers(interface_id);


    /*Set Reset Flag for UART before configuring*/
    *UCAxCTLW0 |= BIT0;

    /*Configure Uart, Choose Clocksrc smclk parity=no databits=8 stopbits=1*/
    *UCAxCTLW0 &= ~UCPEN;	// Parity disabled;
    *UCAxCTLW0 |= UCSSEL1;	// CLK src

    /*Configure Baudrate BRCLK 1Mhz, Baud= 9600 s.h. slau272c.pdf p. 491*/
    unsigned char UCBRx = 6;
    unsigned char UCBRSx = 0x20;
    unsigned char UCBRFx = 8;
    *UCAxBRW = UCBRx;		// SET UCBRx
    *UCAxMCTLW |= (UCBRSx << 8);	// SET UCBRSx
    *UCAxMCTLW |= (UCBRFx << 4); // SET UCBRFx
    *UCAxMCTLW |= UCOS16; // SET UCOS16

    /*Configure Pins for UART - secondary module function*/
    *PxSEL0 &= ~uart_pins;
    *PxSEL1 |=  uart_pins;

    /*Clear Reset Flag*/
    *UCAxCTLW0 &= ~BIT0;
}



void uart_putc(char interface_id, char data) {
	/*Change register pointers if necessary*/
	if(current_interface != interface_id)
		init_uart_registerpointers(interface_id);

	while (!(*UCAxIFG & UCTXIFG)); // Wait till Buffer is empty

	*UCAxTXBUF = data;	//Write Data to Send Buffer
	return;
}

char uart_getc(char interface_id) {
	/*Change register pointers if necessary*/
	if(current_interface != interface_id)
		init_uart_registerpointers(interface_id);

	while (!(*UCAxIFG & UCRXIFG)); //Wait For new Character
	char data = *UCAxRXBUF;			// read character
	return data;
}


int uart_write(char interface_id, char * buffer, int length){
	if(length < 0)
		return 0;

	/*Change register pointers if necessary*/
	if(current_interface != interface_id)
		init_uart_registerpointers(interface_id);

	int i;
	for(i = 0; i < length; i++)
		uart_putc(interface_id,buffer[i]);
	return i;
}

void uart_writeLine(char interface_id, char * buffer){
	/*Change register pointers if necessary*/
	if(current_interface != interface_id)
		init_uart_registerpointers(interface_id);

	int i = 0;
	char c;
	while ((c = buffer[i]) != '\0'){
		uart_putc(interface_id,c);
		i++;
	}

	uart_putc(interface_id,'\r');
	uart_putc(interface_id,'\n');
	return;
}


int uart_read(char interface_id, char * buffer, int length){
	if(length < 0)
		return 0;

	/*Change register pointers if necessary*/
	if(current_interface != interface_id)
		init_uart_registerpointers(interface_id);

	int i;
	for(i = 0; i < length; i++)
		buffer[i] = uart_getc(interface_id);
	return i;
}

int uart_readLine(char interface_id, char ** buffer){
	/*Change register pointers if necessary*/
	if(current_interface != interface_id)
		init_uart_registerpointers(interface_id);

	char *  bff= rcv_buffer;
	int i = 0;
	int lineExceeds = 1;
	for(i = 0; i < RCV_BUFFERSIZE-1; i++){
		bff[i] = uart_getc(interface_id);
		if(bff[i] == '\n' || bff[i] == '\r'){
			lineExceeds = 0;
			break;
		}
	}
	bff[i] = '\0';
	*buffer = bff;
	return lineExceeds;
}


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    char devID = 0;
    init_uart_registerpointers(devID);
    init_uart_registers();

    char * buff;

    while(1){
    	uart_readLine(devID, &buff);
    	uart_writeLine(devID,buff);
    }

	return 0;
}
