#include <xc.h>
#include "interrupts.h"
#include "serial.h"

/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(void)
{
	// turn on global interrupts, peripheral interrupts and the interrupt source 
	// It's a good idea to turn on global interrupts last, once all other interrupt configuration is done.
    PIE4bits.RC4IE=1;	//enable receive interrupt
    INTCONbits.PEIE=1;  //enable peripheral interrupts
    INTCONbits.GIE=1; 	//turn on interrupts globally

}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR()
{
	//add your ISR code here i.e. check the flag, do something (i.e. toggle an LED), clear the flag...
    
    if(PIR4bits.RC4IF){ 			    //receive interrupt
	//LATHbits.LATH3 = !LATHbits.LATH3;   //toggle LED
    putCharToRxBuf(RC4REG);
	//cannot clear the interrupt flag
	}
    
    if(PIR4bits.TX4IF){ 				//transmit interrupt
	//LATHbits.LATH3 = !LATHbits.LATH3;   //toggle LED
    TX4REG = getCharFromTxBuf();
    if (!isDataInTxBuf()) {PIE4bits.TX4IE=0;}
	//cannot clear the interrupt flag
	}
    
}

