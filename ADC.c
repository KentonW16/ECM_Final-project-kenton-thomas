#include <xc.h>
#include "ADC.h"

/************************************
/ ADC_init
/ Function used to initialise ADC module and set it up
/ to sample on pin RF6
************************************/
void ADC_init(void)
{
    TRISFbits.TRISF6=1;   // Select pin RF6 as input
    ANSELFbits.ANSELF6=1; //Ensure analogue circuitry is active (it is by default - watch out for this later in the course!)

    // Set up the ADC module - check section 32 of the datasheet for more details
    ADREFbits.ADNREF = 0;    // Use Vss (0V) as negative reference
    ADREFbits.ADPREF = 0b00; // Use Vdd (3.3V) as positive reference
    ADPCH=0b101110;          // Select channel RF6/ANF6 (BAT-VSENSE) for ADC (should vary from 0-1.23 (3.7/3))
    ADCON0bits.ADFM = 0;     // Left-justified result (i.e. no leading 0s)
    ADCON0bits.ADCS = 1;     // Use internal Fast RC (FRC) oscillator as clock source for conversion
    ADCON0bits.ADON = 1;     // Enable ADC
}

unsigned char ADC_getval(void)
{
    unsigned char tmpval;
       
    ADCON0bits.GO = 1; // Start ADC conversion

    while (ADCON0bits.GO); // Wait until conversion done (bit is cleared automatically when done)
        
    tmpval = ADRESH; // Get 8 most significant bits of the ADC result - if we wanted the 
	// full 10bit result we would also need to read the 2 LSBs from ADRESL and combine the result.
	// An 8bit result is sufficient for our needs here

    return tmpval; //return this value when the function is called
}
