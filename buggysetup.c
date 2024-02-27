#include <xc.h>

/************************************
 Set up two buttons and two lights on clicker board
 Set up lights on buggy
************************************/
void Buggy_init(void)
{
    // setup pin for output (connected to LED 1)
    LATDbits.LATD7=0;   //set initial output state
    TRISDbits.TRISD7=0; //set TRIS value for pin (output)
    
    // setup pin for output (connected to LED 2)
    LATHbits.LATH3=0;   //set initial output state
    TRISHbits.TRISH3=0; //set TRIS value for pin (output)
    
    // setup pin for input (connected to button 1)
    TRISFbits.TRISF2=1; //set TRIS value for pin (input) BUTTON 1
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin 
    
    // setup pin for input (connected to button 2)
    TRISFbits.TRISF3=1; //set TRIS value for pin (input) BUTTON 2
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin
    
    // SEE BUGGY MANUAL P.20 AND CLICKER MANUAL P.21
    // TURN L and TURN R regulate L/R signal lights
    // H.LAMPS turn on headlights and rear lights at low intensity
    // Activate both M.BEAM and H.LAMPS at same time for brighter head lights
    // Activate both BRAKE and H.LAMPS at same time for brighter stop lights -DOESNT SEEM TO WORK
    
    
    // setup pin for H.LAMPS
    LATHbits.LATH1=0;   //set initial output state
    TRISHbits.TRISH1=0; //set TRIS value for pin (output)
    
    // setup pin for BRAKE
    LATDbits.LATD4=0;   //set initial output state
    TRISDbits.TRISD4=0; //set TRIS value for pin (output)
    
    // setup pin for M.BEAM 
    LATDbits.LATD3=0;   //set initial output state
    TRISDbits.TRISD3=0; //set TRIS value for pin (output)
    ANSELDbits.ANSELD3=0; //Ensure analogue circuitry is active (it is by default - watch out for this later in the course!)
    
    // setup pin for TURN R
    LATHbits.LATH0=0;   //set initial output state
    TRISHbits.TRISH0=0; //set TRIS value for pin (output)
    
    // setup pin for TURN L
    LATFbits.LATF0=0;   //set initial output state
    TRISFbits.TRISF0=0; //set TRIS value for pin (output)
    
    //Set up LEDs on color click
    LATGbits.LATG0 = 0; // red LED
    TRISGbits.TRISG0 = 0; 
    
    LATEbits.LATE7 = 0; // green LED
    TRISEbits.TRISE7 = 0; 
    
    LATAbits.LATA3 = 0; // blue LED
    TRISAbits.TRISA3 = 0; 
    
}
