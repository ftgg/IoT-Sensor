//***************************************************************************************
//  MSP430 Blink the LED Demo - Software Toggle P1.0
//
//  Description; Toggle P1.0 by xor'ing P1.0 inside of a software loop.
//  ACLK = n/a, MCLK = SMCLK = default DCO
//
//                MSP430x5xx
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|-->LED
//
//  J. Stevenson
//  Texas Instruments, Inc
//  July 2011
//  Built with Code Composer Studio v5
//***************************************************************************************

#include <msp430.h>				

int check_bit(char reg, int id);


int main(void) {
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
	P3DIR |= 0b11100000;			// Set P3.6 & 3.7 to output direction
	P1DIR = 0x00;					// als input
	P3OUT = 0x00;
	P1REN = 0xFF;
	int cnt = 0;
	for(;;) {
		volatile unsigned int i;	// volatile to prevent optimization

		//P3OUT = 0;
		P3OUT = 0b00000000;
		if((P1IN & BIT0) != 0){
			P3OUT |= 0b10000000;
			cnt++;
		}

		if((P1IN & BIT1) != 0){
			P3OUT |= 0b01000000;
		}
		// P3.3 und 3.2 sind input

 		i = 10000;					// SW Delay
		do i--;
		while(i != 0);
	}
	
	return 0;
}

int check_bit(char reg, int id){
	char mask = 1 << id;
	if(reg & mask != 0)
		return 1;
	return 0;
}
