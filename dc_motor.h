#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>

#define _XTAL_FREQ 64000000

extern char lost;

typedef struct DC_motor { //definition of DC_motor structure
    char power;         //motor power, out of 100
    char direction;     //motor direction, forward(1), reverse(0)
    char brakemode;		// short or fast decay (brake or coast)
    unsigned int PWMperiod; //base period of PWM cycle
    unsigned char *posDutyHighByte; //PWM duty address for motor +ve side
    unsigned char *negDutyHighByte; //PWM duty address for motor -ve side
    char compensation;  //compensation to ensure straight running
} DC_motor;

//function prototypes
void initDCmotorsPWM(unsigned int PWMperiod); // function to setup PWM
void setMotorPWM(DC_motor *m);
void move(DC_motor *mL, DC_motor *mR, char color, unsigned char *moveSequence, unsigned int *straightTime, char curMove, char straightSpeed, unsigned int reverseDuration, unsigned char straightRamp, char turnSpeed, unsigned char turnDuration, unsigned char turnRamp);
void returnHome(DC_motor *mL, DC_motor *mR, unsigned char *moveSequence, unsigned int *straightTime, char curMove, char straightSpeed, unsigned int reverseDuration, unsigned char straightRamp, char turnSpeed, unsigned char turnDuration, unsigned char turnRamp);
void lostReturnHome(DC_motor *mL, DC_motor *mR, unsigned char *moveSequence, unsigned int *straightTime, char curMove, char straightSpeed, unsigned int reverseDuration, unsigned char straightRamp, char turnSpeed, unsigned char turnDuration, unsigned char turnRamp);
void stop(DC_motor *mL, DC_motor *mR, unsigned char straightRamp);
void turnLeft(DC_motor *mL, DC_motor *mR, char turnSpeed, unsigned char turnDuration, unsigned char turnRamp);
void turnRight(DC_motor *mL, DC_motor *mR, char turnSpeed, unsigned char turnDuration, unsigned char turnRamp);
void fullSpeedAhead(DC_motor *mL, DC_motor *mR, char straightSpeed, unsigned char straightRamp);
void reverseOneSquare(DC_motor *mL, DC_motor *mR, char straightSpeed, unsigned int reverseDuration, unsigned char straightRamp);
void wallAdjust(DC_motor *mL, DC_motor *mR, char straightSpeed, unsigned char straightRamp);
void reverseWallAdjust(DC_motor *mL, DC_motor *mR, char straightSpeed, unsigned char straightRamp);
void reverseShort(DC_motor *mL, DC_motor *mR, char straightSpeed, unsigned char straightRamp);
void calibration(DC_motor *mL, DC_motor *mR, char turnSpeed, unsigned char *turnDuration, unsigned char turnRamp);

#endif
