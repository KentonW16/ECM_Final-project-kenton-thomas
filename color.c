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
   
}

void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit color device address + Write mode
    I2C_2_Master_Write(0x80 | address);  //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);    //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (P.12) (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);    //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red Low Byte
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read, hence send it 0)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Green(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);    //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (P.12) (auto-increment protocol transaction) + start at Green low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);    //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red Low Byte
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read, hence send it 0)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Blue(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);    //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (P.12) (auto-increment protocol transaction) + start at Blue low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);    //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red Low Byte
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read, hence send it 0)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
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

