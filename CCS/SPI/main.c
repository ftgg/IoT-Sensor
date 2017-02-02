#include <msp430.h> 
#include "spi.h"


#define OP_NOOP   0

#define OP_DIGIT0 1
#define OP_DIGIT1 2
#define OP_DIGIT2 3
#define OP_DIGIT3 4

#define OP_FEATURE 0x0E

#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

void write(int device, char reg, char data);



/*
 * interessanter Treiber : http://www.ccsinfo.com/forum/viewtopic.php?p=148855
 * main.c
 */
int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    char init_sequ[2] = {0x0E, 0b00001001};
    char test [2]= {0x0F, 0x00};
    char scanLimit [2] = {0x0B, 0x03};
    char leavesd[2] = {0x0C, 0x81};
    char number0[2] = {0x01,0x00};
    char number1[2] = {0x02,0b00000101};

    init_spi_registers(0);
  	//spi_write(0,init_sequ,2);		//init feature Register
  	//spi_write(0,test,2);
  	//spi_write(0,leavesd,2);
  	//spi_write(0,scanLimit,2);


  	//spi_write(0,number0,2);
  	//spi_write(0,number1,2);

    write(0,OP_FEATURE,0b00000000);

    write(0,OP_DISPLAYTEST,0);
    write(0,OP_SCANLIMIT,3);
    //write(0,OP_DECODEMODE,0);
    write(0,OP_DIGIT0,1);
    write(0,OP_DIGIT1,2);
    write(0,OP_DIGIT2,3);
    write(0,OP_DIGIT3,4);
    //write(0,OP_SHUTDOWN,0x80);

    char i = 0;
  	while(1)
  		write(0,OP_FEATURE,i++);

	return 0;
}

void write(int device, char reg, char data){
	char array[2] = {reg,data};
	spi_write(device,array,2);
}
