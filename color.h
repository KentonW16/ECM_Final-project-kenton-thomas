#ifndef _color_H
#define _color_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

extern unsigned int ambient;

typedef struct HSV_calib { //Structure to store calibrated RGB value
	unsigned int H; //Ranges from 0-36000 (0.00 - 360.00 degrees (2 d.p.) multiplied by 100 to avoid float)
	unsigned int S; //Ranges from 0-10000 (0.00% - 100.00% (2 d.p.) multiplied by 100 to avoid float)
	unsigned int V; //Ranges from 0-10000 (0.00% - 100.00% (2 d.p.) multiplied by 100 to avoid float)
} HSV_calib;

typedef struct RGBC_val { //Structure to store RGB value
	unsigned int R;
	unsigned int G;
	unsigned int B;
    unsigned int C;
} RGBC_val;

typedef struct HSV_val { //Structure to store HSV value
	unsigned int H; //Ranges from 0-36000 (0.00 - 360.00 degrees (2 d.p.) multiplied by 100 to avoid float)
	unsigned int S; //Ranges from 0-10000 (0.00% - 100.00% (2 d.p.) multiplied by 100 to avoid float)
	unsigned int V; //Ranges from 0-10000 (0.00% - 100.00% (2 d.p.) multiplied by 100 to avoid float)
} HSV_val;



/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
void color_click_init(void);

/********************************************//**
 *  Function to clear and re-initialise the clear channel interrupt on color click
 ***********************************************/
void color_clear_init_interrupts(void);

/********************************************//**
 *  Function to write to the colour click module
 *  address is the register within the colour click to write to
 *	value is the value that will be written to that address
 ***********************************************/
void color_writetoaddr(char address, char value);

/********************************************//**
 *  Function to turn on all color click LEDs (white light)
 *	state = 1 for on, state = 0 for off
 ***********************************************/
void white_Light(char state);

/********************************************//**
 *  Function to read all RGBC channels
 *	Writes 16 bit ADC value representing colour intensity to struct for each color
 ***********************************************/
void color_read(RGBC_val *RGBC);

/********************************************//**
 *  Function to normalise all RGB values
 *	Writes normalised values to RGBC_n struct for each color
 ***********************************************/
void color_normalise(RGBC_val RGBC, RGBC_val *RGBC_n);

/********************************************//**
 *  Function to decide on color
 *	Writes code (1-8) for color dependent on RGB values
 *  No color detected = 0
 ***********************************************/
unsigned char color_detect(HSV_val HSV, HSV_calib red, HSV_calib green, HSV_calib blue, HSV_calib yellow, HSV_calib pink, HSV_calib orange, HSV_calib lightblue, HSV_calib white);

/********************************************//**
 *  Function to calibrate color sensor by flashing each color one by one
 *  The RGB value read by the sensor is then converted into HSV values
 *	The HSV values are written into HSV_calib struct, one for each color
 *  Goes through each color sequentially from 1-8
 ***********************************************/
void color_calibration(RGBC_val *RGBC, HSV_val *HSV, HSV_calib *red, HSV_calib *green, HSV_calib *blue, HSV_calib *yellow, HSV_calib *pink, HSV_calib *orange, HSV_calib *lightblue, HSV_calib *white);

/********************************************//**
 *  Function used in rgb_2_hsv function to determine max. value between two values
 ***********************************************/
unsigned int max (unsigned int a, unsigned int b);

/********************************************//**
 *  Function used in rgb_2_hsv function to determine min. value between two values
 ***********************************************/
unsigned int min (unsigned int a,unsigned int b);

/********************************************//**
 *  Function to convert RGB values obtained from color click to HSV values
 *	Writes converted values to HSV_val struct
 ***********************************************/
void rgb_2_hsv(RGBC_val RGBC, HSV_val *HSV);


#endif
