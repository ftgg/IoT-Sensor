#include <msp430.h> 
#include "spi.h"

/*
 * main.c
 */


/*Registers*/
volatile unsigned int * UCAxCTLW0;	//spi settings						slau272  p.515
volatile unsigned int * UCAxMCTLW;	//Baud								slau272  p.497
volatile unsigned int * UCAxBRW;    //Baud								slau272  p.517
volatile unsigned int * UCAxIFG;	//Interrupt Flags pending)			slau272  p.503
volatile unsigned int * UCAxTXBUF;	//Interrupt Flags pending)      	slau272  p.499
volatile unsigned int * UCAxRXBUF;  //Interrupt Flags pending)      	slau272  p.499
volatile unsigned char * PxSEL0_0;    // Select pin functionality 		slau272c p.316
volatile unsigned char * PxSEL1_0;    // Select pin functionality			slau272c p.316
volatile unsigned char * PxSEL0_1;    // Select pin functionality 		slau272c p.316
volatile unsigned char * PxSEL1_1;    // Select pin functionality			slau272c p.316

/*Dev spi pins*/
char spi_pins_0;
char spi_pins_1;
char ste_pin;	//slave select realized as gpIO

/*Current Interface*/
char current_interface = -1;

void init_spi_registerpointers(char devid){
	if(devid == 0){
		// UCA0
		UCAxCTLW0 = &UCA0CTLW0;
		UCAxMCTLW = &UCA0MCTLW;
		UCAxBRW = &UCA0BRW;
		UCAxIFG = &UCA0IFG;
		UCAxTXBUF = &UCA0TXBUF;
		UCAxRXBUF = &UCA0RXBUF;

		PxSEL0_0 = &P2SEL0;
		PxSEL1_0 = &P2SEL1;
		PxSEL0_1 = &P1SEL0;
		PxSEL1_1 = &P1SEL1;

		spi_pins_0 = BIT0 | BIT1; // Port2
		spi_pins_1 = BIT5; // Port1
		ste_pin = BIT4;

		current_interface = 0;
	}else{
		// UCA1
		UCAxCTLW0 = &UCA1CTLW0;
		UCAxMCTLW = &UCA1MCTLW;
		UCAxBRW = &UCA1BRW;
		UCAxIFG = &UCA1IFG;
		UCAxTXBUF = &UCA1TXBUF;
		UCAxRXBUF = &UCA0RXBUF;

		PxSEL0_0 = &P2SEL0;
		PxSEL1_0 = &P2SEL1;
		PxSEL0_1 = &P1SEL0;
		PxSEL1_1 = &P1SEL1;

		spi_pins_0 = BIT2; // Port2
		spi_pins_1 = BIT6 | BIT7; // Port1
		ste_pin = BIT3;

		current_interface = 1;
	}
}


void init_spi_registers(char interface_id){
	/*Change register pointers if necessary*/
	if(current_interface != interface_id)
		init_spi_registerpointers(interface_id);


    /*Set Reset Flag for spi before configuring*/
    *UCAxCTLW0 |= UCSWRST;

    /*Configure spi, Choose Clocksrc smclk  slau272c.pdf p. 515*/
    *UCAxCTLW0 |= UCMST;	//Master mode
    *UCAxCTLW0 |= UCCKPH;	// SET UCCKPH to have data at MOSI before next rising edge of clk
    *UCAxCTLW0 |= UCMSB;	//MSB First
    *UCAxCTLW0 |= UCMODE1;	//select SPI mode Slave when when UCxSTE = 0
    *UCAxCTLW0 |= UCSYNC;	//Synchronus mode
    *UCAxCTLW0 |= UCSSEL1;	// CLK src
    *UCAxCTLW0 |= UCSTEM;	//master this is slave enable signal

    /*Configure Clock BRCLK 1Mhz, Baud= 9600 s.h. slau272c.pdf p. 491*/
    /*unsigned char UCBRx = 6;
    unsigned char UCBRSx = 0x20;
    unsigned char UCBRFx = 8;
    *UCAxBRW = UCBRx;		// SET UCBRx
    *UCAxMCTLW |= (UCBRSx << 8);	// SET UCBRSx
    *UCAxMCTLW |= (UCBRFx << 4); // SET UCBRFx
    *UCAxMCTLW |= UCOS16; // SET UCOS16
*/
    /*Configure Pins for spi - secondary module function*/
    *PxSEL0_0 &= ~spi_pins_0;
    *PxSEL1_0 |=  spi_pins_0;
    *PxSEL1_0 &= ~spi_pins_1;
    *PxSEL1_1 |=  spi_pins_1;
    P1DIR = ste_pin;	 // UCA_ste and UCB_ste use the same Port
    P1OUT |= ste_pin; 					// CS high


    /*Clear Reset Flag*/
    *UCAxCTLW0 &= ~UCSWRST;
}


void spi_putc(char interface_id, char data) {
	/*Change register pointers if necessary*/
	if(current_interface != interface_id)
		init_spi_registerpointers(interface_id);

	while (!(*UCAxIFG & UCTXIFG)); // Wait till Buffer is empty

	P1OUT &= ~ste_pin;	// CS low
	*UCAxTXBUF = data;	//Write Data to Send Buffer
	P1OUT |= ste_pin; // CS high
	return;
}


char spi_getc(char interface_id) {
	/*Change register pointers if necessary*/
	if(current_interface != interface_id)
		init_spi_registerpointers(interface_id);

	while (!(*UCAxIFG & UCRXIFG)); //Wait For new Character
	char data = *UCAxRXBUF;			// read character
	return data;
}


int spi_write(char interface_id, char * buffer, int length){
	if(length < 0)
		return 0;

	/*Change register pointers if necessary*/
	if(current_interface != interface_id)
		init_spi_registerpointers(interface_id);


	P1OUT &= ~ste_pin;					// CS low
	int i;
	for(i = 0; i < length; i++){
		while (!(*UCAxIFG & UCTXIFG));  // Wait till Buffer is empty
		*UCAxTXBUF = buffer[i];			//Write Data to Send Buffer
	}
	while (!(*UCAxIFG & UCTXIFG));

	P1OUT |= ste_pin; 					// CS high

	return i;
}


int spi_read(char interface_id, char * buffer, int length){
	if(length < 0)
		return 0;

	/*Change register pointers if necessary*/
	if(current_interface != interface_id)
		init_spi_registerpointers(interface_id);

	int i;
	for(i = 0; i < length; i++)
		buffer[i] = spi_getc(interface_id);
	return i;
}
