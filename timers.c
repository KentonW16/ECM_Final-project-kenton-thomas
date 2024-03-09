#include <xc.h>
#include "timers.h"

/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b1011; // 1:2048 (increments every 0.13 ms, overflow after 8.4 seconds)
    T0CON0bits.T016BIT=1;	//16bit mode	
	
    // initialise the timer registers to 0
    TMR0H=0;            //write High reg first, update happens when low reg is written to
    TMR0L=0;
    T0CON0bits.T0EN=1;	//start the timer
}

/************************************
 * Function to reset timer to 0
************************************/
void resetTimer(void)
{
    TMR0H=0;            //write High reg first, update happens when low reg is written to
    TMR0L=0;
}

/************************************
 * Function to return the full 16bit timer value
 * Note TMR0L and TMR0H must be read in the correct order, or TMR0H will not contain the correct value
************************************/
unsigned int get16bitTMR0val(void)
{
	//returns 16bit timer value
    unsigned int timer_low = TMR0L; //reading low first
    unsigned int timer_high = TMR0H; //reading high after
    unsigned int timer_val = (timer_high<<8) | timer_low; //combining to one 16 bit number
    return timer_val;
}
