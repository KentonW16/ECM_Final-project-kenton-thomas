#include <xc.h>
#include "interrupts.h"
#include "serial.h"
#include "i2c.h"
#include "color.h"

/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(void)
{
	//set up interrupts from color click (through pin RB0)
    TRISBbits.TRISB0=1;     //set TRIS value for pin (input)
    ANSELBbits.ANSELB0=0;   //turn off analogue input on pin 
    PIE0bits.INT0IE=1;      //enable interrupts from color click (pin RB0) pg. 8
    IPR0bits.INT0IP = 1;    //high priority
    INTCONbits.INT0EDG = 0; //trigger on falling edge
    
    // turn on global interrupts, peripheral interrupts and the interrupt source 
	// It's a good idea to turn on global interrupts last, once all other interrupt configuration is done.
    TMR0IE=1;           //enable interrupts from timer
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
    
    // Wall detection interrupt
    if(PIR0bits.INT0IF){ 			    //receive interrupt
    
    wall = 1;                      //set flag to be read in main   
    color_clear_init_interrupts(); //clear and re-initialise interrupt on color click
    PIR0bits.INT0IF = 0;           //clear interrupt flag
    
    LATHbits.LATH3 = !LATHbits.LATH3;   //toggle LED
    //__delay_ms(200);
    
	}
    
    // Timer interrupt (trigger => lost)
    if(TMR0IF){ 					  //check the interrupt source
        
    //LATHbits.LATH3 = !LATHbits.LATH3; //toggle LED
	TMR0IF=0; 						  //clear the interrupt flag
    
	}
    
    
    // Serial transfer interrupts
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

