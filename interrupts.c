#include <xc.h>
#include "interrupts.h"
#include "serial.h"
#include "i2c.h"

/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/
void Interrupts_init(void)
{
	TRISBbits.TRISB0=1; //set TRIS value for pin (input)
    ANSELBbits.ANSELB0=0; //turn off analogue input on pin 
    PIE0bits.INT0IE=1; //enable interrupts from color click (pin RB0) pg. 8
    IPR0bits.INT0IP=1;
    
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
    
    if(PIR0bits.INT0IF){ 			    //receive interrupt
	LATHbits.LATH3 = 1;   //toggle LED
    
    I2C_2_Master_Start();                        //Start condition
	I2C_2_Master_Write(0x52 | 0x00);             //7 bit address + Write mode
	I2C_2_Master_Write(0b11100110);              //write to special function register to clear (pg. 12)
	I2C_2_Master_Stop();     
    
    PIR0bits.INT0IF = 0; //clear interrupt flag
	}
    
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

