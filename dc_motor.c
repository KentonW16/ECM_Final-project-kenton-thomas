#include <xc.h>
#include "dc_motor.h"

// function initialise T2 and CCP for DC motor control
void initDCmotorsPWM(unsigned int PWMperiod){
    //initialise your TRIS and LAT registers for PWM  
    
    LATEbits.LATE2=0;   //set initial output state
    TRISEbits.TRISE2=0; //set TRIS value for pin (output)
    LATEbits.LATE4=0;   //set initial output state
    TRISEbits.TRISE4=0; //set TRIS value for pin (output)
    LATCbits.LATC7=0;   //set initial output state
    TRISCbits.TRISC7=0; //set TRIS value for pin (output)
    LATGbits.LATG6=0;   //set initial output state
    TRISGbits.TRISG6=0; //set TRIS value for pin (output)
    
    //configure PPS to map CCP modules to pins
    RE2PPS=0x05; //CCP1 on RE2
    RE4PPS=0x06; //CCP2 on RE4
    RC7PPS=0x07; //CCP3 on RC7
    RG6PPS=0x08; //CCP4 on RG6

    // timer 2 config
    T2CONbits.CKPS=0b011;   // 1:8 prescaler
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    // 0.0001s*16MHz/8 -1 = 199
    T2PR=PWMperiod;  //Period reg 10kHz base period
    T2CONbits.ON=1;
    
    //setup CCP modules to output PMW signals
    //initial duty cycles 
    CCPR1H=0; 
    CCPR2H=0; 
    CCPR3H=0; 
    CCPR4H=0; 
    
    //use tmr2 for all CCP modules used
    CCPTMRS0bits.C1TSEL=0;
    CCPTMRS0bits.C2TSEL=0;
    CCPTMRS0bits.C3TSEL=0;
    CCPTMRS0bits.C4TSEL=0;
    
    //configure each CCP
    CCP1CONbits.FMT=1;           // left aligned duty cycle (we can just use high byte)
    CCP1CONbits.CCP1MODE=0b1100; //PWM mode  
    CCP1CONbits.EN=1;            //turn on
    
    CCP2CONbits.FMT=1;           // left aligned
    CCP2CONbits.CCP2MODE=0b1100; //PWM mode  
    CCP2CONbits.EN=1;            //turn on
    
    CCP3CONbits.FMT=1;           // left aligned
    CCP3CONbits.CCP3MODE=0b1100; //PWM mode  
    CCP3CONbits.EN=1;            //turn on
    
    CCP4CONbits.FMT=1;           // left aligned
    CCP4CONbits.CCP4MODE=0b1100; //PWM mode  
    CCP4CONbits.EN=1;            //turn on
}

// function to set CCP PWM output from the values in the motor structure
void setMotorPWM(DC_motor *m)
{
    unsigned char posDuty, negDuty; //duty cycle values for different sides of the motor
    
    if(m->brakemode) {
        posDuty=m->PWMperiod - ((unsigned int)(m->power)*(m->PWMperiod))/100; //inverted PWM duty
        negDuty=m->PWMperiod; //other side of motor is high all the time
    }
    else {
        posDuty=0; //other side of motor is low all the time
		negDuty=((unsigned int)(m->power)*(m->PWMperiod))/100; // PWM duty
    }
    
    if (m->direction) {
        *(m->posDutyHighByte)=posDuty;  //assign values to the CCP duty cycle registers
        *(m->negDutyHighByte)=negDuty;       
    } else {
        *(m->posDutyHighByte)=negDuty;  //do it the other way around to change direction
        *(m->negDutyHighByte)=posDuty;
    }
}

void move(DC_motor *mL, DC_motor *mR, char color ,char straightSpeed, unsigned char reverseDuration, unsigned char straightRamp, char turnSpeed, unsigned char turnDuration, unsigned char turnRamp)
{
    if (color == 1) { //red - right 90
        turnRight(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        turnRight(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        fullSpeedAhead(mL, mR, straightSpeed, straightRamp);
    }
    
    else if (color == 2) { //green - left 90
        turnLeft(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        turnLeft(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        fullSpeedAhead(mL, mR, straightSpeed, straightRamp);
    }
    
    else if (color == 3) { //blue - 180
        turnLeft(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        turnLeft(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        turnLeft(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        turnLeft(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        fullSpeedAhead(mL, mR, straightSpeed, straightRamp);
    }
    
    else if (color == 4) { //yellow - reverse right 90
        reverseOneSquare(mL, mR, straightSpeed, reverseDuration, straightRamp);
        __delay_ms(50);
        turnRight(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        turnRight(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        fullSpeedAhead(mL, mR, straightSpeed, straightRamp);
    }
    
    else if (color == 5) { //pink - reverse left 90
        reverseOneSquare(mL, mR, straightSpeed, reverseDuration, straightRamp);
        __delay_ms(50);
        turnLeft(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        turnLeft(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        fullSpeedAhead(mL, mR, straightSpeed, straightRamp);
    }
    
    else if (color == 6) { //orange - right 135
        turnRight(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        turnRight(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        turnRight(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        fullSpeedAhead(mL, mR, straightSpeed, straightRamp);
    }
    
    else if (color == 7) { //light blue - left 135
        turnLeft(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        turnLeft(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        turnLeft(mL, mR, turnSpeed, turnDuration, turnRamp);
        __delay_ms(50);
        fullSpeedAhead(mL, mR, straightSpeed, straightRamp);
    }
    
    else if (color == 8) { //white - return home
        /*return home code*/
    }
}
//function to stop the robot gradually 
void stop(DC_motor *mL, DC_motor *mR, unsigned char straightRamp)
{
    mL->direction = 1;
    mR->direction = 1;
    int i;
    int cur_power;
    for (cur_power=mL->power-mL->compensation;cur_power>=0;cur_power--) {
        mL->power = cur_power+mL->compensation;
        mR->power = cur_power+mR->compensation;
        setMotorPWM(mL);
        setMotorPWM(mR);
        for (i=0;i<straightRamp;i++) {
            __delay_ms(5);
        }
    }
    mL->power = 0;
    mR->power = 0;
}

//function to make the robot turn left 
void turnLeft(DC_motor *mL, DC_motor *mR, char turnSpeed, unsigned char turnDuration, unsigned char turnRamp)
{
    mL->direction = 0;
    mR->direction = 1;
    int i;
    int cur_power;
    for (cur_power=1;cur_power<=turnSpeed;cur_power++) {
        mL->power = cur_power+mL->compensation;
        mR->power = cur_power+mR->compensation;
        setMotorPWM(mL);
        setMotorPWM(mR);
        for (i=0;i<turnRamp;i++) {
            __delay_ms(5);
        }
    }
    
    for (i=0;i<turnDuration;i++) {
            __delay_ms(5);
        }
    
    for (cur_power=turnSpeed;cur_power>=0;cur_power--) {
        mL->power = cur_power+mL->compensation;
        mR->power = cur_power+mR->compensation;
        setMotorPWM(mL);
        setMotorPWM(mR);
        for (i=0;i<turnRamp;i++) {
            __delay_ms(5);
        }
    }
}

//function to make the robot turn right 
void turnRight(DC_motor *mL, DC_motor *mR, char turnSpeed, unsigned char turnDuration, unsigned char turnRamp)
{
    mL->direction = 1;
    mR->direction = 0;
    int i;
    int cur_power;
    for (cur_power=1;cur_power<=turnSpeed;cur_power++) {
        mL->power = cur_power+mL->compensation;
        mR->power = cur_power+mR->compensation;
        setMotorPWM(mL);
        setMotorPWM(mR);
        for (i=0;i<turnRamp;i++) {
            __delay_ms(5);
        }
    }
    
    for (i=0;i<turnDuration;i++) {
            __delay_ms(5);
        }
    
    for (cur_power=turnSpeed;cur_power>=0;cur_power--) {
        mL->power = cur_power+mL->compensation;
        mR->power = cur_power+mR->compensation;
        setMotorPWM(mL);
        setMotorPWM(mR);
        for (i=0;i<turnRamp;i++) {
            __delay_ms(5);
        }
    }
}

//function to make the robot go straight
void fullSpeedAhead(DC_motor *mL, DC_motor *mR, char straightSpeed, unsigned char straightRamp)
{
    mL->direction = 1;
    mR->direction = 1;
    int i;
    int cur_power;
    for (cur_power=1;cur_power<=straightSpeed;cur_power++) {
        mL->power = cur_power+mL->compensation;
        mR->power = cur_power+mR->compensation;
        setMotorPWM(mL);
        setMotorPWM(mR);
        for (i=0;i<straightRamp;i++) {
            __delay_ms(5);
        }
    }
}

void reverseOneSquare(DC_motor *mL, DC_motor *mR, char straightSpeed, unsigned char reverseDuration, unsigned char straightRamp)
{
    mL->direction = 0;
    mR->direction = 0;
    int i;
    int cur_power;
    for (cur_power=1;cur_power<=straightSpeed;cur_power++) {
        mL->power = cur_power+mL->compensation;
        mR->power = cur_power+mR->compensation;
        setMotorPWM(mL);
        setMotorPWM(mR);
        for (i=0;i<straightRamp;i++) {
            __delay_ms(5);
        }
    }
    
    for (i=0;i<reverseDuration;i++) {
            __delay_ms(5);
        }
    
    for (cur_power=straightSpeed;cur_power>=0;cur_power--) {
        mL->power = cur_power+mL->compensation;
        mR->power = cur_power+mR->compensation;
        setMotorPWM(mL);
        setMotorPWM(mR);
        for (i=0;i<straightRamp;i++) {
            __delay_ms(5);
        }
    }
    mL->power = 0;
    mR->power = 0;
}

void calibration(DC_motor *mL, DC_motor *mR, char turnSpeed, unsigned char *turnDuration, unsigned char turnRamp)
{
    // run unadjusted
    turnLeft(mL, mR, turnSpeed, *turnDuration, turnRamp);
    __delay_ms(50);
    turnLeft(mL, mR, turnSpeed, *turnDuration, turnRamp);
    __delay_ms(50);
    turnRight(mL, mR, turnSpeed, *turnDuration, turnRamp);
    __delay_ms(50);
    turnRight(mL, mR, turnSpeed, *turnDuration, turnRamp);
    
    while (1) {   
        while (PORTFbits.RF2 && PORTFbits.RF3); //wait for button press
        
        if (!PORTFbits.RF2 && *turnDuration>0) { // button 1 press decreases turning angle
            *turnDuration--;
            LATDbits.LATD7 = 1; // LED 1 on
        } 
        
        else if (!PORTFbits.RF3) {              // button 2 press increases turning angle
            *turnDuration++;
            LATHbits.LATH3 = 1; // LED 2 on
        } 
        
        __delay_ms(500); // delay to check for long press
        
        if (!PORTFbits.RF3) { // long press on button 2 to continue
            *turnDuration--;   // no effect on calibration from a long press
            break;            // leave while loop
        }
        
        LATDbits.LATD7 = LATHbits.LATH3 = 0; // both LEDs off
        
        // run to assess improvement
        turnLeft(mL, mR, turnSpeed, *turnDuration, turnRamp);
        __delay_ms(50);
        turnLeft(mL, mR, turnSpeed, *turnDuration, turnRamp);
        __delay_ms(50);
        turnRight(mL, mR, turnSpeed, *turnDuration, turnRamp);
        __delay_ms(50);
        turnRight(mL, mR, turnSpeed, *turnDuration, turnRamp);
        
    }
    
    // flash both to indicate continue
    LATDbits.LATD7 = LATHbits.LATH3 = 1; // both LEDs on
    __delay_ms(100);
    LATDbits.LATD7 = LATHbits.LATH3 = 0; // both LEDs off
}

