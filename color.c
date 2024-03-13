#include <xc.h>
#include "color.h"
#include "i2c.h"



void color_click_init(void) // See Colour click datasheet P.13
{   
    //setup colour sensor via i2c interface
    I2C_2_Master_Init();      //Initialise i2c Master

    //set device PON
	color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    
    //turn on device ADC
	color_writetoaddr(0x00, 0x03); // RGBC enable

    //set integration time
	color_writetoaddr(0x01, 0xD5);
    
    //initialise color click interrupts
    color_clear_init_interrupts();
    
}

void color_clear_init_interrupts(void) {
    //clear interrupt on color click
    I2C_2_Master_Start();            
    I2C_2_Master_Write(0x52 | 0x00); 
    I2C_2_Master_Write(0b11100110);  
    I2C_2_Master_Stop();
   
    //set interrupt thresholds
    //unsigned int high_threshold = ambient + 10;
    //unsigned int low_threshold = ambient - 10;
    
    unsigned int high_threshold = ambient + (ambient/5);
    unsigned int low_threshold = ambient - (ambient/20);
    
    //initialise interrupt
	color_writetoaddr(0x00, 0x13); //turn on RGBC interrupts
    __delay_ms(3);
    color_writetoaddr(0x07, (high_threshold >> 8)); //high threshold upper
    color_writetoaddr(0x06, (high_threshold & 0xFF)); //high threshold lower
    color_writetoaddr(0x05, (low_threshold >> 8)); //low threshold upper
    color_writetoaddr(0x04, (low_threshold & 0xFF)); //low threshold lower
    color_writetoaddr(0x0C, 0b11); //persistence - requires [5] C readings outside threshold
}
    


void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();                //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit color device address + Write mode
    I2C_2_Master_Write(0x80 | address);  //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();                 //Stop condition
}


void white_Light(char state)
{
    if (state){ //Turn on all RGB LEDs on color click if 1 is passed into function
        LATGbits.LATG0 = 1;
        LATEbits.LATE7 = 1;
        LATAbits.LATA3 = 1;
    }
    else { //Turn off all RGB LEDs on color click if 0 is passed into function
        LATGbits.LATG0 = 0;
        LATEbits.LATE7 = 0;
        LATAbits.LATA3 = 0;
    }
}


void color_read(RGBC_val *RGBC)
{
	I2C_2_Master_Start();                        //Start condition
	I2C_2_Master_Write(0x52 | 0x00);             //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);             //command (P.12) (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			         // start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);             //7 bit address + Read (1) mode
	RGBC->R=I2C_2_Master_Read(1);	             //read the Red Low Byte
	RGBC->R=RGBC->R | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read, hence send it 0)
	I2C_2_Master_Stop();                         //Stop condition
    
    I2C_2_Master_Start();                        //Start condition
	I2C_2_Master_Write(0x52 | 0x00);             //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);             //command (P.12) (auto-increment protocol transaction) + start at Green low register
	I2C_2_Master_RepStart();                     // start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);             //7 bit address + Read (1) mode
	RGBC->G=I2C_2_Master_Read(1);                //read the Green Low Byte
	RGBC->G=RGBC->G | (I2C_2_Master_Read(0)<<8); //read the Green MSB (don't acknowledge as this is the last read, hence send it 0)
	I2C_2_Master_Stop();                         //Stop condition
    
    I2C_2_Master_Start();                        //Start condition
	I2C_2_Master_Write(0x52 | 0x00);             //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);             //command (P.12) (auto-increment protocol transaction) + start at Blue low register
	I2C_2_Master_RepStart();                     // start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);             //7 bit address + Read (1) mode
	RGBC->B=I2C_2_Master_Read(1);                //read the Blue Low Byte
	RGBC->B=RGBC->B | (I2C_2_Master_Read(0)<<8); //read the Blue MSB (don't acknowledge as this is the last read, hence send it 0)
	I2C_2_Master_Stop();                         //Stop condition
    
    I2C_2_Master_Start();                        //Start condition
	I2C_2_Master_Write(0x52 | 0x00);             //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);             //command (P.12) (auto-increment protocol transaction) + start at Clear low register
	I2C_2_Master_RepStart();                     // start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);             //7 bit address + Read (1) mode
	RGBC->C=I2C_2_Master_Read(1);                //read the Clear Low Byte
	RGBC->C=RGBC->C | (I2C_2_Master_Read(0)<<8); //read the Clear MSB (don't acknowledge as this is the last read, hence send it 0)
	I2C_2_Master_Stop();                         //Stop condition
}

unsigned char color_detect(HSV_val HSV, HSV_calib red, HSV_calib green, HSV_calib blue, HSV_calib yellow, HSV_calib pink, HSV_calib orange, HSV_calib lightblue, HSV_calib white)
{
    unsigned char color=0;
    
    // Red (10 degrees tolerance on hue)
    if (red.H > 30000){ //Somewhere between 350 and 360 degrees
        if(((red.H)-1000 < HSV.H && HSV.H < 36000) || 0 < HSV.H && HSV.H < 1000 + red.H -36000)) {color = 1;}
    }
    
    else if (red.H < 10000){
        if((36000 - 1000 + (red.H) < HSV.H && HSV.H < 36000) || 0 < HSV.H && HSV.H < red.H + 1000)) {color = 1;}
    }
        
    // Green or light blue
    else if (min(green.H,lightblue.H)-2000 < HSV.H && HSV.H < max(green.H,lightblue.H)+2000) {
        if (HSV.S > lightblue.S + 500){color = 2;} // Green has higher S value than light blue
        else {color = 7;}
    }
    
    // Blue (20 degree tolerance as no other colors near)
    else if ((blue.H)-2000 < HSV.H && HSV.H < (blue.H)+2000) {color = 3;}
    
    // Yellow or pink
    else if (min(yellow.H, pink.H)-1000 < HSV.H && HSV.H < max(yellow.H, pink.H)+1000) {
        if (HSV.S > pink.S + 500){color = 4;} // Yellow has higher S value than pink
        else {color = 5;} // Pink
    }
    
    // Orange (5 degree tolerance)
    else if ((orange.H)-500 < HSV.H && HSV.H < (orange.H)+500) {color = 6;}
    
    // White (5 degree tolerance)
    else if ((white.H)-500 < HSV.H && HSV.H < (white.H)+500) {color = 8;}
    
    else {color = 9;}
   
    return color;
}

void color_calibration(RGBC_val *RGBC, HSV_val *HSV, HSV_calib *red, HSV_calib *green, HSV_calib *blue, HSV_calib *yellow, HSV_calib *pink, HSV_calib *orange, HSV_calib *lightblue, HSV_calib *white)
{
    white_Light(1); //Turn on color click LED in preparation to read values
    LATDbits.LATD7 = LATHbits.LATH3 = 1; // Turn on board LEDs to show color calibration routine started

    // Read RED when button pressed
    while (PORTFbits.RF2); 
    LATDbits.LATD7 = LATHbits.LATH3 = 0; //Turn off board LEDs to show that color is being read
    color_read(RGBC); // Obtain RGB values from color click
    rgb_2_hsv(*RGBC, HSV); //Convert RGB values into HSV values
    red->H = HSV->H; // Obtain converted values from HSV_val HSV struct and store into HSV_calib struct for each individual color
    red->S = HSV->S;
    red->V = HSV->V;
    
    __delay_ms(500); // Prevent multiple button press and allow visual confirmation that color has been read (board LEDs turning off)
    LATDbits.LATD7 = LATHbits.LATH3 = 1; //Turn board LEDs back on to signal that it's ready and waiting for next color read
    
    // Read GREEN when button pressed
    while (PORTFbits.RF2);
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    rgb_2_hsv(*RGBC, HSV);
    green->H = HSV->H;
    green->S = HSV->S;
    green->V = HSV->V;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
    
    // Read BLUE when button pressed
    while (PORTFbits.RF2);
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    rgb_2_hsv(*RGBC, HSV);
    blue->H = HSV->H;
    blue->S = HSV->S;
    blue->V = HSV->V;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
    
    // Read YELLOW when button pressed
    while (PORTFbits.RF2);
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    rgb_2_hsv(*RGBC, HSV);
    yellow->H = HSV->H;
    yellow->S = HSV->S;
    yellow->V = HSV->V;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
    
    // Read PINK when button pressed
    while (PORTFbits.RF2);
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    rgb_2_hsv(*RGBC, HSV);
    pink->H = HSV->H;
    pink->S = HSV->S;
    pink->V = HSV->V;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
    
    // Read ORANGE when button pressed
    while (PORTFbits.RF2);
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    rgb_2_hsv(*RGBC, HSV);
    orange->H = HSV->H;
    orange->S = HSV->S;
    orange->V = HSV->V;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1; 
    
    // Read LIGHT BLUE when button pressed
    while (PORTFbits.RF2);
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    rgb_2_hsv(*RGBC, HSV);
    lightblue->H = HSV->H;
    lightblue->S = HSV->S;
    lightblue->V = HSV->V;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1; 
    
    // Read WHITE when button pressed
    while (PORTFbits.RF2);
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    rgb_2_hsv(*RGBC, HSV);
    white->H = HSV->H;
    white->S = HSV->S;
    white->V = HSV->V;  
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
}

unsigned int max (unsigned int a, unsigned int b){
    if (a > b){return a;} // Check if a is greater than b, if so, then a is max val; return a
    else {return b;} // Otherwise, b is greater (or equal to) a, then return b
}

unsigned int min (unsigned int a,unsigned int b){
    if (a < b){return a;} // Check if a is smaller than b, if so, then a is min val; return a
    else {return b;} // Otherwise, b is smaller (or equal to) a, then return b
}

void rgb_2_hsv(RGBC_val RGBC, HSV_val *HSV) {
    // H:Hue S:Saturation V:Value
    
    // Divide sensor RGB values by maximum value that each sensor can read 
    // (Theoretical max. is 43008 from datasheet, but varies in testing)
    // Each of the r g and b values will now vary from 0-1 (Ranges from 1-10000 in code for increased accuracy without using float)
    unsigned int r = (unsigned int)(RGBC.R*10000L/1900); //Max value for R sensor found to be 1900
    unsigned int g = (unsigned int)(RGBC.G*10000L/1400); //Max value for G sensor found to be 1400
    unsigned int b = (unsigned int)(RGBC.B*10000L/1600); //Max value for B sensor found to be 1600
    
    
    unsigned int cmax = max(r, max(g, b)); // Maximum between r, g, b 
    unsigned int cmin = min(r, min(g, b)); // Minimum between r, g, b 
    unsigned long diff = cmax - cmin; // Difference between max and min value
    
    // Find H (hue) value (Ranges from 0-360 degrees, 0-36000 in code for 2 d.p. accuracy)
    if (cmax == r){ // If max value is red, need to also check if g > b to avoid negative degree readings as unsigned int used (wraparound)
        if (g > b){HSV->H = (unsigned int)((g-b)*6000L/diff);}
        else if (b > g){HSV->H = (unsigned int)((g + 6*diff -b)*6000L/diff);}
    }
    
    else if (cmax == g){HSV->H = (unsigned int)((b + 2*diff -r)*6000L/diff);} // Negative number avoided by adding 2*diff before subtracting
        
    else if (cmax == b){HSV->H = (unsigned int)((r + 4*diff -g)*6000L/diff);}
    
    // Find S (saturation) value (Ranges from 0-100% , 0-10000 in code for 2 d.p. accuracy)
    HSV->S = (unsigned int)((diff * 10000)/cmax);
    
    if (HSV->S == 0){HSV->H = 0;}
    
    //Find V (value) value (Ranges from 0-100% , 0-10000 in code for 2 d.p. accuracy)
    HSV->V = cmax;
  
}

