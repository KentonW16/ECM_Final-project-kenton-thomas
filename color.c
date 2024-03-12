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

unsigned char color_detect(RGBC_val RGBC_n,RGB_calib red, RGB_calib green, RGB_calib blue, RGB_calib yellow, RGB_calib pink, RGB_calib orange, RGB_calib lightBlue, RGB_calib white)
{
    unsigned char color=0;
    
    if ((red.R)-40 < RGBC_n.R) { //red
        color = 1;
    }
    
    else if ((green.G)-25 < RGBC_n.G) { //green
        color = 2;
    }
    
    else if ((blue.B)-40 < RGBC_n.B) { //blue
        color = 3;
    }
    
    else {color = 9;}
    
    /*
    if (RGBC_n.R > 500) { // red, yellow or orange
        if (RGBC_n.G < 200){color = 1;} //Red
        else if (255 < RGBC_n.G < 295) {color = 4;} //Yellow
        else if (200 < RGBC_n.G < 250) {color = 6;} //Orange
        else {color = 0;}
    }
    
    else if (RGBC_n.R < 420) { // Blue or light blue
        if (345 < RGBC_n.B < 385){color = 3;} //Blue
        else if (290 < RGBC_n.B < 330) {color = 7;} //Light blue
        else {color = 0;}
    }
    
    else if (300 < RGBC_n.G) {color = 2;} // green
        
    else if (460 < RGBC_n.R < 500 && 240 < RGBC_n.G < 260) {color=5;} //Pink
    
    else if (430 < RGBC_n.R < 470 && 260 < RGBC_n.G < 300) {color=8;} //White

    else if (RGBC_n.G > 290) { // green
        color = 2;
    }
    else if (RGBC_n.B > 230) { // blue
        color = 3;
    }
    
    else if (RGBC_n.B > 300 && RGBC_n.B > 300) { // yellow
        color = 4;
    }
    else if (RGBC_n.B > 300 && RGBC_n.B > 300) { // pink
        color = 5;
    }
    else if (RGBC_n.B > 300 && RGBC_n.B > 300) { // orange
        color = 6;
    }
    else if (RGBC_n.B > 300 && RGBC_n.B > 300) { // light blue
        color = 7;
    }
    else if (RGBC_n.R > 300 && RGBC_n.G > 300 && RGBC_n.B > 300) { // white
        color = 8;
    }
     
    
    else {  //color not recognised
        color = 9;
    }
    */
    return color;
}

void color_calibration(RGBC_val *RGBC, RGBC_val *RGBC_n, RGB_calib *red, RGB_calib *green, RGB_calib *blue, RGB_calib *yellow, RGB_calib *pink, RGB_calib *orange, RGB_calib *lightblue, RGB_calib *white)
{
    white_Light(1);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;

    while (PORTFbits.RF2); //read red when button pressed
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    color_normalise(*RGBC, RGBC_n);
    red->R = RGBC_n->R;
    red->G = RGBC_n->G;
    red->B = RGBC_n->B;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
    
    while (PORTFbits.RF2); //read green when button pressed
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    color_normalise(*RGBC, RGBC_n);
    green->R = RGBC_n->R;
    green->G = RGBC_n->G;
    green->B = RGBC_n->B;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
    
    while (PORTFbits.RF2); //read blue when button pressed
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    color_normalise(*RGBC, RGBC_n);
    blue->R = RGBC_n->R;
    blue->G = RGBC_n->G;
    blue->B = RGBC_n->B;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
    /*
    while (PORTFbits.RF2); //read yellow when button pressed
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    color_normalise(*RGBC, RGBC_n);
    yellow->R = RGBC_n->R;
    yellow->G = RGBC_n->G;
    yellow->B = RGBC_n->B;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
    
    while (PORTFbits.RF2); //read pink when button pressed
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    color_normalise(*RGBC, RGBC_n);
    pink->R = RGBC_n->R;
    pink->G = RGBC_n->G;
    pink->B = RGBC_n->B;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
    
    while (PORTFbits.RF2); //read orange when button pressed
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    color_normalise(*RGBC, RGBC_n);
    orange->R = RGBC_n->R;
    orange->G = RGBC_n->G;
    orange->B = RGBC_n->B;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1; 
    
    while (PORTFbits.RF2); //read light blue when button pressed
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    color_normalise(*RGBC, RGBC_n);
    lightblue->R = RGBC_n->R;
    lightblue->G = RGBC_n->G;
    lightblue->B = RGBC_n->B;
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1; 
    
    while (PORTFbits.RF2); //read white when button pressed
    LATDbits.LATD7 = LATHbits.LATH3 = 0;
    color_read(RGBC);
    color_normalise(*RGBC, RGBC_n);
    white->R = RGBC_n->R;
    white->G = RGBC_n->G;
    white->B = RGBC_n->B;  
    
    __delay_ms(500);
    LATDbits.LATD7 = LATHbits.LATH3 = 1;
    */
}

