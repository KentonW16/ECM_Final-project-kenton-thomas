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
#include "timers.h"
#include "color.h"
#include "i2c.h"
#include "buggysetup.h"
#include "dc_motor.h"
#include "battery.h"


#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

// Declare global variables
unsigned int ambient = 500;
char wall = 0;
char lost = 0;

void main(void){
    // Declare local variables
    char buf[40] = {0};
    unsigned int PWMcycle = 199;
    unsigned char color = 0;
    unsigned char moveSequence[40] = {0}; //length is max number of moves
    unsigned int straightTime[41] = {0};
    char curMove = 0;
    
    unsigned char testSequence[4] = {4,3,9,8}; //***for testing without colors
    
    // Declare structures
    struct RGBC_val RGBC, RGBC_n;
    struct DC_motor motorL, motorR; //declare two DC_motor structures 
    
    // Initialisation functions
    Buggy_init();
    color_click_init();
    Timer0_init();
    Interrupts_init();
    initUSART4();
    initDCmotorsPWM(PWMcycle);

    motorL.power=0; 						//zero power to start
    motorL.direction=1; 					//set default motor direction
    motorL.brakemode=1;						// brake mode (slow decay)
    motorL.posDutyHighByte=(unsigned char *)(&CCPR1H);  //store address of CCP1 duty high byte
    motorL.negDutyHighByte=(unsigned char *)(&CCPR2H);  //store address of CCP2 duty high byte
    motorL.PWMperiod=PWMcycle;              //store PWMperiod for motor (value of T2PR in this case)
    motorL.compensation=3;                  //left motor run at higher power

    motorR.power=0; 						//zero power to start
    motorR.direction=1; 					//set default motor direction
    motorR.brakemode=1;						// brake mode (slow decay)
    motorR.posDutyHighByte=(unsigned char *)(&CCPR3H);  //store address of CCP1 duty high byte
    motorR.negDutyHighByte=(unsigned char *)(&CCPR4H);  //store address of CCP2 duty high byte
    motorR.PWMperiod=PWMcycle;              //store PWMperiod for motor (value of T2PR in this case)
    motorR.compensation=0;                  //right motor run at lower power
    
    // Calibration parameters for motor control (durations in 10ms increments)
    char straightSpeed=25;             //maximum power
    unsigned char straightRamp=1;      //time between each power step
    
    unsigned char reverseDuration=10;  //adjust to length of one square
    
    char turnSpeed=20;                 //maximum power
    unsigned char turnDuration=5;      //time between ramp up and ramp down
    unsigned char turnRamp=2;          //time between each power step 
    
    // Display battery voltage in binary on LEDs (before button press)
    batteryLevel();
    
    // Wait for button press
    while (PORTFbits.RF2);
    LATDbits.LATD7 = LATHbits.LATH3 = 0; // both LEDs off 
    
    // Calibration mode, getting values through serial connection
    /*
    while (1){
        while (!PORTFbits.RF2){
            LATHbits.LATH1=LATDbits.LATD3=1;
            white_Light(1);
            color_read(&RGBC);              //read RGBC values
            color_normalise(RGBC, &RGBC_n); //normalise RGB values
            //color = color_detect(RGBC_n);
            //sprintf(buf,"r=%d g=%d b=%d c=%d   n: r=%d g=%d b=%d  color: %d \r\n",RGBC.R,RGBC.G,RGBC.B,RGBC.C, RGBC_n.R,RGBC_n.G,RGBC_n.B,color);
            sprintf(buf,"r=%d g=%d b=%d c=%d   n: r=%d g=%d b=%d  \r\n",RGBC.R,RGBC.G,RGBC.B,RGBC.C, RGBC_n.R,RGBC_n.G,RGBC_n.B);

            sendTxBuf();
            TxBufferedString(buf); //send string to PC
            sendTxBuf();
            TxBufferedString(""); 
            __delay_ms(300);
            }
    }
    */
    
    
    // Turn on bright headlights to show buggy is on
    LATHbits.LATH1=LATDbits.LATD3=1;
    __delay_ms(500);
    
    // Flash color cards in front of buggy
    struct RGB_calib red, green, blue, yellow, pink, orange, lightblue, white; 		//declare 8 color calibration structures to store RBG values of each color
    color_calibration(&RGBC, &RGBC_n, &red, &green, &blue, &yellow, &pink, &orange, &lightblue, &white);
    
    // Calibration for turning angle
    calibration(&motorL, &motorR, turnSpeed, &turnDuration, turnRamp);
    
    // Turn on white LED on color click 
    white_Light(1);
    __delay_ms(1000);
    
    // Calibrate to ambient light
    color_read(&RGBC);
    ambient=RGBC.C;
    __delay_ms(500);
    
    fullSpeedAhead(&motorL, &motorR, straightSpeed, straightRamp);
    resetTimer();
    
    wall=0;
    lost=0;
    
    while(1) {
        if (wall == 1) { //if wall interrupt triggered
            PIE0bits.INT0IE=TMR0IE=0;      //turn off interrupts so not triggered during movement
            straightTime[curMove] = get16bitTMR0val();
            
            // Stop and read color
            stop(&motorL, &motorR, straightRamp);  //stop
            color_read(&RGBC);                     //read RGBC values
            color_normalise(RGBC, &RGBC_n);        //normalise RGB values
            color = color_detect(RGBC_n, red, green, blue, yellow, pink, orange, lightblue, white);          //determine color from RGBC values
            //color = testSequence[curMove];         //***for testing without colors
            moveSequence[curMove] = color;         //record movement
            
            // Carry out movement based on color detected
            move(&motorL, &motorR, color, moveSequence, straightTime, curMove, straightSpeed, reverseDuration, straightRamp, turnSpeed, turnDuration, turnRamp);

            // Calibrate to ambient light
            color_read(&RGBC);
            ambient=RGBC.C;
            
            curMove++;                             //increment current move number
            resetTimer();                          //reset timer
            PIE0bits.INT0IE=TMR0IE=1;              //turn interrupts on
            wall = 0;                              //reset flag
            
        }
        
        if (lost == 1) {  //if timer interrupt triggered (moving straight for > 8s)
            PIE0bits.INT0IE=0;                     // turn off color click interrupts (not timer)
            stop(&motorL, &motorR, straightRamp);  //stop
            lostReturnHome(&motorL, &motorR, moveSequence, straightTime, curMove, straightSpeed, reverseDuration, straightRamp, turnSpeed, turnDuration, turnRamp);
            PIE0bits.INT0IE=1;                     // turn back on color click interrupts
            lost = 0;
            break;
        }
        
        if (color == 8 || color == 9) {break;} //color white or not recognised
        
    }
    
}
