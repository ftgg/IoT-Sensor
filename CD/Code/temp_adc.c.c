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

void displayValueAtLeds(unsigned char val){
	P3OUT = val & 0xF0;
	PJOUT = val & 0x0F;
}

void setUpADC () {
	//POWER
	P2DIR |= BIT7;
	P2OUT |= BIT7;

	//Config ADC
	P1SEL1 |= BIT4;
	P1SEL0 |= BIT4;

	__delay_cycles(50000);

	//Config ADC

	ADC10CTL0 &= ~ADC10ENC;
	ADC10CTL0 = ADC10SHT_7 + ADC10ON;
	ADC10CTL1 = ADC10SHS_0 + ADC10SHP + ADC10SSEL_0;
	ADC10CTL2 = ADC10RES;
	ADC10MCTL0 = ADC10INCH_4;
}

int getADCVal(){
	while (ADC10CTL1 & BUSY);
	ADC10CTL0 |= ADC10ENC | ADC10SC ; //Start Conversion
	while (ADC10CTL1 & BUSY);
	return ADC10MEM0;
}


int main(void) {
	WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
	setUpADC();
	RTCSEC = 0x0;
	RTCCTL01 &= ~RTCHOLD;				// Start RealTimeClock

	P3DIR |= 0xF0;						// Set P3 to output direction
	PJDIR |= 0x0F;						// Set PJ to output direction


	char value = 0;
	for(;;) {
		volatile unsigned int i;	// volatile to prevent optimization

		displayValueAtLeds(getADCVal());
		i = 60000;					// SW Delay
 		do i--;
		while(i != 0);
	}
	
	return 0;
}
