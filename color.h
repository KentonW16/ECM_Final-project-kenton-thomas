#ifndef _color_H
#define _color_H

#include <xc.h>

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

extern unsigned int ambient;


typedef struct RGBC_val { //Structure to store RGB value
	unsigned int R;
	unsigned int G;
	unsigned int B;
    unsigned int C;
} RGBC_val;

typedef struct HSV_val { //Structure to store HSV value
	unsigned int H;
	unsigned int S;
	unsigned int V;
} HSV_val;


/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
void color_click_init(void);

/********************************************//**
 *  Function to clear and re-initialise the clear channel interrupt on the color click
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
unsigned char color_detect(RGBC_val RGBC_n);

/********************************************//**
 *  Function to write to convert RGB value to HSV
 ***********************************************/



#endif
