#include <msp430.h> 
#include "../UART/uart.h"

/*
 * main.c
 */

float(*operation)(float,float) decode(char * line, float * opA, float * opB){
	char * tmp;
	char * opA, opB;
	char op;
	char i = 0;
	do {
		tmp = line + i;
		if(*tmp == '+' || *tmp == '-' || *tmp == '*' || *tmp == '/'){
			op = *tmp;
			*op = '\n';
			opA = line;
			opB = op + 1;
			break;
		}
	}while(*op != '\0');

	//TODO float cast operator identifizieren und zurückliefern

}

float add(float a, float b){
	return a + b;
}


int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
    char devID = 0;
    float(*operation)(float,float);
    init_uart_registers(devID);

    while(1){
    	char * line = uart_readLine(devID,&line);
    	float a,b,result;
    	operation = decode(line,&a,&b);
    	result = operation(a,b);

    }
	return 0;
}
