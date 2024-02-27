// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "color.h"
#include "i2c.h"
#include "buggysetup.h"




#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void){
    Buggy_init();
    color_click_init();

    
    // Turn on bright headlights to show buggy is on
    LATHbits.LATH1=1;
    LATDbits.LATD3=1;
    
    // Turn on white LED on color click 
    white_Light(1);
    
    //Read red value
    unsigned int red;
    unsigned int green;
    unsigned int blue;
    
    red=color_read_Red();
    //green=color_read_Green();
    //blue=color_read_Blue();
    
    LATDbits.LATD7=1;
    
    

}
