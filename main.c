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
#include "dc_motor.h"
#include "battery.h"



#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void){
    Buggy_init();
    color_click_init();
    initUSART4();
    Interrupts_init();
    unsigned int PWMcycle = 199;
    initDCmotorsPWM(PWMcycle);
    struct RGBC_val RGBC, RGBC_n;
    unsigned char color;
    char buf[40] = {0};
    
    struct DC_motor motorL, motorR; 		//declare two DC_motor structures 

    motorL.power=0; 						//zero power to start
    motorL.direction=1; 					//set default motor direction
    motorL.brakemode=1;						// brake mode (slow decay)
    motorL.posDutyHighByte=(unsigned char *)(&CCPR1H);  //store address of CCP1 duty high byte
    motorL.negDutyHighByte=(unsigned char *)(&CCPR2H);  //store address of CCP2 duty high byte
    motorL.PWMperiod=PWMcycle;              //store PWMperiod for motor (value of T2PR in this case)
    motorL.compensation=3;                   //left motor run at higher power

    motorR.power=0; 						//zero power to start
    motorR.direction=1; 					//set default motor direction
    motorR.brakemode=1;						// brake mode (slow decay)
    motorR.posDutyHighByte=(unsigned char *)(&CCPR3H);  //store address of CCP1 duty high byte
    motorR.negDutyHighByte=(unsigned char *)(&CCPR4H);  //store address of CCP2 duty high byte
    motorR.PWMperiod=PWMcycle;              //store PWMperiod for motor (value of T2PR in this case)
    motorR.compensation=0;                  //right motor run at lower power
    
    //calibration parameters for motor control
    char straightSpeed=60;             //maximum power
    unsigned char straightRamp=2;      //time between each power step
    
    unsigned char reverseDuration=10;  //adjust to length of one square
    
    char turnSpeed=21;              //maximum power
    unsigned char turnDuration=10;   //time between ramp up and ramp down
    unsigned char turnRamp=4;       //time between each power step 
    
    //display battery voltage in binary on LEDs (before button press)
    batteryLevel();
    
    //wait for button press
    while (PORTFbits.RF2);
    LATDbits.LATD7 = LATHbits.LATH3 = 0; // both LEDs off
    // Turn on bright headlights to show buggy is on
    LATHbits.LATH1=LATDbits.LATD3=1;
    __delay_ms(500);
    
    calibration(&motorL, &motorR, turnSpeed, &turnDuration, turnRamp);
    
    // Turn on white LED on color click 
    white_Light(1);
    
    while(1) {
        while (PORTFbits.RF2);          //read RGBC and send to PC on button press

        color_read(&RGBC);              //read RGBC values
        color_normalise(RGBC, &RGBC_n); //normalise RGB values
        color = color_detect(RGBC_n);
        move(&motorL, &motorR, color, straightSpeed, reverseDuration, straightRamp, turnSpeed, turnDuration, turnRamp);
        
        sprintf(buf,"r=%d g=%d b=%d c=%d   n: r=%d g=%d b=%d  color: %d \r\n",RGBC.R,RGBC.G,RGBC.B,RGBC.C, RGBC_n.R,RGBC_n.G,RGBC_n.B,color);
        sendTxBuf();
        TxBufferedString(buf); //send string to PC
        sendTxBuf();
        TxBufferedString(""); 
        __delay_ms(300);
        
    }
    
    
    
}
