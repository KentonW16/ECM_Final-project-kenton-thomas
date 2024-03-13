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
    unsigned int high_threshold = ambient + 12;
    unsigned int low_threshold = ambient - 12;
    
    //initialise interrupt
	color_writetoaddr(0x00, 0x13); //turn on RGBC interrupts
    __delay_ms(3);
    color_writetoaddr(0x07, (high_threshold >> 8)); //high threshold upper
    color_writetoaddr(0x06, (high_threshold & 0xFF)); //high threshold lower
    color_writetoaddr(0x05, (low_threshold >> 8)); //low threshold upper
    color_writetoaddr(0x04, (low_threshold & 0xFF)); //low threshold lower
    color_writetoaddr(0x0C, 0b0100); //persistence - requires [5] C readings outside threshold
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
    if (state){
        LATGbits.LATG0 = 1;
        LATEbits.LATE7 = 1;
        LATAbits.LATA3 = 1;
    }
    else {
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


void color_normalise(RGBC_val RGBC, RGBC_val *RGBC_n) {
    /*method 1 - normalising against C*/
    /*
    RGBC_n->C = RGBC.C/100;
    RGBC_n->R = RGBC.R/RGBC_n->C;
    RGBC_n->G = RGBC.G/RGBC_n->C;
    RGBC_n->B = RGBC.B/RGBC_n->C;
    */
    
    /*method 2 - normalising against C, increased precision by converting to longs*/
    /*
    RGBC_n->C = RGBC.C;
    RGBC_n->R = 1000L*RGBC.R/RGBC.C;
    RGBC_n->G = 1000L*RGBC.G/RGBC.C;
    RGBC_n->B = 1000L*RGBC.B/RGBC.C;
    */
    
    /*method 3 - normalising against sum of RGB*/
    RGBC_n->C = RGBC.C;
    RGBC_n->R = 1000L*RGBC.R/(RGBC.R+RGBC.G+RGBC.B);
    RGBC_n->G = 1000L*RGBC.G/(RGBC.R+RGBC.G+RGBC.B);
    RGBC_n->B = 1000L*RGBC.B/(RGBC.R+RGBC.G+RGBC.B);
}

unsigned char color_detect(HSV_val HSV, HSV_calib red, HSV_calib green, HSV_calib blue, HSV_calib yellow, HSV_calib pink, HSV_calib orange, HSV_calib lightblue, HSV_calib white)
{
    unsigned char color=0;
    
    // Red (10 degrees tolerance on hue)
    if ((red.H)-1000 < HSV.H && HSV.H < (red.H)+1000) {color = 1;}

    // Green
    else if ((green.H)-1000 < HSV.H && HSV.H < (green.H)+1000) {color = 2;}

    // Blue
    else if ((blue.H)-1000 < HSV.H && HSV.H < (blue.H)+1000) {color = 3;}
    
    // Yellow
    else if ((yellow.H)-1000 < HSV.H && HSV.H < (yellow.H)+1000) {color = 4;}
    
    // Pink
    else if ((pink.H)-1000 < HSV.H && HSV.H < (pink.H)+1000) {color = 5;}
    
    // Orange
    else if ((orange.H)-1000 < HSV.H && HSV.H < (orange.H)+1000) {color = 6;}
    
    // Light blue
    else if ((lightblue.H)-1000 < HSV.H && HSV.H < (lightblue.H)+1000) {color = 7;}
    
    // White
    else if ((white.H)-1000 < HSV.H && HSV.H < (white.H)+1000) {color = 8;}
    
    else {color = 9;}
   
    return color;
}

void color_calibration(RGBC_val *RGBC, HSV_val *HSV, HSV_calib *red, HSV_calib *green, HSV_calib *blue, HSV_calib *yellow, HSV_calib *pink, HSV_calib *orange, HSV_calib *lightblue, HSV_calib *white)
{
    white_Light(1);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;

    while (PORTFbits.RF2); //read red when button pressed
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    rgb_2_hsv(*RGBC, HSV);
    red->H = HSV->H;
    red->S = HSV->S;
    red->V = HSV->V;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
    
    while (PORTFbits.RF2); //read green when button pressed
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    rgb_2_hsv(*RGBC, HSV);
    green->H = HSV->H;
    green->S = HSV->S;
    green->V = HSV->V;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
    
    while (PORTFbits.RF2); //read blue when button pressed
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    rgb_2_hsv(*RGBC, HSV);
    blue->H = HSV->H;
    blue->S = HSV->S;
    blue->V = HSV->V;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
    
    while (PORTFbits.RF2); //read yellow when button pressed
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    rgb_2_hsv(*RGBC, HSV);
    yellow->H = HSV->H;
    yellow->S = HSV->S;
    yellow->V = HSV->V;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
    
    while (PORTFbits.RF2); //read pink when button pressed
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    rgb_2_hsv(*RGBC, HSV);
    pink->H = HSV->H;
    pink->S = HSV->S;
    pink->V = HSV->V;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
    
    while (PORTFbits.RF2); //read orange when button pressed
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    rgb_2_hsv(*RGBC, HSV);
    orange->H = HSV->H;
    orange->S = HSV->S;
    orange->V = HSV->V;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1; 
    
    while (PORTFbits.RF2); //read light blue when button pressed
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    rgb_2_hsv(*RGBC, HSV);
    lightblue->H = HSV->H;
    lightblue->S = HSV->S;
    lightblue->V = HSV->V;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1; 
    
    while (PORTFbits.RF2); //read white when button pressed
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
    if (a > b){return a;}
    else {return b;}
}

unsigned int min (unsigned int a,unsigned int b){
    if (a < b){return a;}
    else {return b;}
}

void rgb_2_hsv(RGBC_val RGBC, HSV_val *HSV) {
    //Divide RGB values by maximum value that can be obtained from each sensor
    // Change r to be value from 0-1, but multiply by power of 10 so int used
    unsigned int r = (unsigned int)(RGBC.R*10000L/255); //Theoretical max=43008
    unsigned int g = (unsigned int)(RGBC.G*10000L/255); //Range from 1-10000 (0-1 but 2 d.p.)
    unsigned int b = (unsigned int)(RGBC.B*10000L/255);
    
    // h, s, v = hue, saturation, value 
    unsigned int cmax = max(r, max(g, b)); // maximum of r, g, b 
    unsigned int cmin = min(r, min(g, b)); // minimum of r, g, b 
    unsigned long diff = cmax - cmin;
    
    //Find H (hue) value
    if (cmax == r){
        if (g > b){HSV->H = (unsigned int)((g-b)*6000L/diff);}
        else if (b > g){HSV->H = (unsigned int)((g + 6*diff -b)*6000L/diff);}
    }
    else if (cmax == g){HSV->H = (unsigned int)((b + 2*diff -r)*6000L/diff);}
        
    else if (cmax == b){HSV->H = (unsigned int)((r + 4*diff -g)*6000L/diff);}
    
    //Find S (saturation) value
    HSV->S = (unsigned int)((diff * 10000)/cmax);
    
    if (HSV->S == 0){HSV->H = 0;}
    
    //Find V (value) value
    HSV->V = cmax;
  
}

