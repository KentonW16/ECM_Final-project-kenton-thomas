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
   
    //RGBC interrupts
	color_writetoaddr(0x00, 0x13); //turn on RGBC interrupts
    color_writetoaddr(0x07, 0x07); //high threshold upper
    color_writetoaddr(0x06, 0xD0); //high threshold lower
    color_writetoaddr(0x05, 0x00); //low threshold upper
    color_writetoaddr(0x04, 0x00); //low threshold lower
    //color_writetoaddr(0x0C, 0x02); //persistence - requires [2] C readings outside threshold
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

unsigned char color_detect(RGBC_val RGBC_n)
{
    unsigned char color=0;
    if (RGBC_n.R > 560) { // red
        color = 1;
    }
    else if (RGBC_n.G > 290) { // green
        color = 2;
    }
    else if (RGBC_n.B > 230) { // blue
        color = 3;
    }
    
    /*
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
     */
    return color;
}

