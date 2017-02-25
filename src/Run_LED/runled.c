#include <msp430.h>

/*
 * runled.c
 * connect P1.0 with 1.1 to stop =)
 */

volatile unsigned int direction = 0;
volatile unsigned int speed = 8;

/*
 * the LEDs on this board are splitted to two Ports P3.x and PJ.x
 */
void lightning(int lights) {
	P3OUT = lights & 0xF0;
	PJOUT = lights & 0x0F;
}

/*
 * wait function
 * volatile to prevent optimization!
 */
void dda(void) {
	volatile unsigned int t = (3*speed-2) * 10000;
	while (t > 0) {
		t--;
	}
}

/*
 *select the next LED, select the first one if there is no next one.
 */
unsigned int next(unsigned int light) {
	if (light == 0b10000000) {
		return 0b00000001;
	}
	return light * 2;
}
unsigned int prev(unsigned int light) {
	if (light == 0b00000001) {
		return 0b10000000;
	}
	return light / 2;
}

/*
 * choose next or previous LED in dependence of the direction boolean set by Interrupt
 */
unsigned int wich(unsigned int light) {
	if (direction == 0)
		return next(light);
	return prev(light);
}

int main(void) {
	unsigned int light =1;

	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

	P3DIR = 0xF0;				//Port 3 as output
	PJDIR = 0x0F;				//Port J as output
	P1DIR = (0xFF & BIT1);		//Port 1.1 as output rest input!

	P4DIR = 0x00; 				//Port 4 as input

	P4IE |= (BIT0 + BIT1);  	//enable interrupts for S1 button
								//Button S1 and S2 are in active-low configuration
	P4REN = (BIT0 + BIT1);		//enable pullups on resoective ports
								//Since S1,S2 are in an active low config,
								//the other end of the resistor has to be pulled high
	P4OUT = (BIT0 + BIT1);
	P4IFG &= ~(BIT0 + BIT1);	//clear interrupt flag
	_enable_interrupt();		//enable interupts

	P3OUT = 0;					//clear output LEDs
	PJOUT = 0;


	for (;;) {
		light = wich(light);
		lightning(light);
		dda();
		if((P1IN & BIT0) == 0b00000000)
			break;
	}

	return 0;
}

// Port 4 interrupt service routine
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void) {
	if (P4IFG & BIT0) { 				//check if S1 was pressed
		direction = ~direction;			//change direction
	}

	if (P4IFG & BIT1) { 				//check if S2 was pressed
		speed = ((speed + 1) % 3) + 1;	//decrease speed lvl 1 -> 3 -> 2 -> 1 ...
	}

	P4IFG = 0;							//clear interrupt flags

}
