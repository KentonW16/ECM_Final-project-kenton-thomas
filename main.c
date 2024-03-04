// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include <stdio.h>
#include "serial.h"
#include "interrupts.h"
#include "color.h"
#include "i2c.h"
#include "buggysetup.h"




#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void){
    Buggy_init();
    color_click_init();
    initUSART4();
    Interrupts_init();
    char buf[40] = {0};

    struct RGBC_val RGBC, RGBC_n;
    
    // Turn on bright headlights to show buggy is on
    LATHbits.LATH1=LATDbits.LATD3=1;
    
    // Turn on white LED on color click 
    white_Light(1);
    
    while(1) {
        while (PORTFbits.RF2);          //read RGBC and send to PC on button press

        color_read(&RGBC);              //read RGBC values
        color_normalise(RGBC, &RGBC_n); //normalise RGB values
        
        sprintf(buf,"r=%d g=%d b=%d c=%d   n: r=%d g=%d b=%d\r\n",RGBC.R,RGBC.G,RGBC.B,RGBC.C, RGBC_n.R,RGBC_n.G,RGBC_n.B);
        sendTxBuf();
        TxBufferedString(buf); //send string to PC
        sendTxBuf();
        TxBufferedString(""); 
        __delay_ms(300);
        
    }
    
    
    
}
