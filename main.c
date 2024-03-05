// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include "dc_motor.h"
#include "battery.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(void) {
    unsigned int PWMcycle = 199;
    initDCmotorsPWM(PWMcycle);
    
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
    
    // initialise LEDs
    LATDbits.LATD7=0;   //set initial output state
    TRISDbits.TRISD7=0; //set TRIS value for pin (output)
    LATHbits.LATH3=0;   //set initial output state
    TRISHbits.TRISH3=0; //set TRIS value for pin (output)
    
    //initialise buttons
    TRISFbits.TRISF2=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2=0; //turn off analogue input on pin  
    TRISFbits.TRISF3=1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF3=0; //turn off analogue input on pin
    
    //display battery voltage in binary on LEDs (before button press)
    batteryLevel();
    
    //wait for button press
    while (PORTFbits.RF2);
    LATDbits.LATD7 = LATHbits.LATH3 = 0; // both LEDs off
    __delay_ms(500);
    
    calibration(&motorL, &motorR, turnSpeed, &turnDuration, turnRamp);
    
    char color = 7; //to test
    move(&motorL, &motorR, color, straightSpeed, reverseDuration, straightRamp, turnSpeed, turnDuration, turnRamp);
    
    while(1) {
        //move(&motorL, &motorR, color, straightSpeed, reverseDuration, straightRamp, turnSpeed, turnDuration, turnRamp);
        
        /*
        turnLeft(&motorL, &motorR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        turnRight(&motorL, &motorR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        fullSpeedAhead(&motorL, &motorR, straightSpeed, straightRamp);
        __delay_ms(50);
        stop(&motorL, &motorR, straightRamp);
        __delay_ms(50);
        reverseOneSquare(&motorL, &motorR, straightSpeed, reverseDuration, straightRamp);
        __delay_ms(50);
        */
    }
    
    
}
