# Course project - Mine navigation search and rescue - Kenton Wong & Thomas Lawson
## Table of contents
- [Functionality Overview](#functionality-overview)
- [User Instructions](#user-instructions)
- [Key Features](#key-features)
- [Reflections and Future Improvements](#reflections-and-future-improvements)
- [Code Details](#code-details) <br/><br/>
- [Challenge brief](#challenge-brief)
- ["Mine" environment specification](#mine-environment-specification)
- [Resources and project submission](#resources-and-project-submission)
- [Supplementary technical information](#supplementary-technical-information)

## Functionality Overview
1. Traverse maze by controlling the 4 motors on the buggy

1. Detect when approaching a color card/wall
	-  Achieved through interpreting interrupt on the RGBC clear channel with a moving threshold
	-  Additional movement into the wall, followed by a short reverse,  made to align buggy perpendicular to wall and ensure color sensing is carried out at a consistent distance

1. Read the color card on the wall and determine color of card
	-  Read the Red, Green and Blue values from the color click
	-  RGB (red, green, blue) values converted into HSV (hue, saturation, value) values to reduce sensitivity to ambient light conditions
	-  The HSV value is compared against thresholds determined during the color calibration routine, and a decision is made on the color

1. Carry out the instruction indicated by the color
	- Buggy reverses further to ensure ample room for maneuvering 
	-  Motor calibration routine allows buggy to make accurate turns on different surfaces

1. Return home if white color card read or if color of card could not be determined
	-  Time moving and color read recorded for each step, to allow the buggy to retrace its path

### Flow Chart
Below is a flowchart demonstrating the logic flow of the buggy's operation

![code_flowchart](gifs/code_flowchart.jpg)

## User Instructions
1. When buggy is turned on, the battery level is indicated as a 2 bit value through the two LEDs on the clicker board.

	Both LEDs | LED 2 only | LED 1 only | No LEDs
	---------|---------|---------|---------
	Above 75% charge | 50% - 75% charge | 25% - 50% charge | Below 25% charge

1. Press button 1 to activate buggy, headlights and color click LED will turn on, and the buggy will enter the color calibration sequence.

1. Color cards are read by holding each in front of the sensor at about 3cm distance in front of the buggy, then pressing button 1 to read and proceed to the next color. The order is as follows: red, green, blue, yellow, pink, orange, light blue, white. This gives a reference value for each color in the operational lighting conditions.

1. After all colors have been read, the buggy enters the motor calibration sequence to adjust the turning angle. is adjusted by running a test sequence (90 degrees left followed by 90 degrees right). The angle of turning is increased by a press of button 2 or decreased by button 1.

1. Once the angle is satisfactory, a long press of button 2 begins the main navigation function.

Link to video showing calibration routine:
https://imperiallondon-my.sharepoint.com/:f:/g/personal/khw121_ic_ac_uk/ElhzdluG545Nvhx_GZEV6rABJHsDoW-Rxq6_Fvrupplgjg?e=K78ZXv

## Key Features
### Wall Detection
Wall detection at a consistent distance is challenging, as the light level varies depending on the direction that the buggy is pointing relative to light sources and the color of the card pointed at. 

We noticed that the clear channel light level always decreases as the buggy approaches a wall and then sharply increases when the buggy is within a few centimetres from the wall (as the light from the LED reflects off the card). We used this minimum as the point at which the buggy detects that it is near the wall. The clear channel reading is taken when the color click interrupt triggers, which is at a given percentage above or below the previous reading. A lower reading is taken at 5% below the previous, so that the decrease in light level is tracked closely, whereas a higher reading (which indicates a wall) is only taken when at 33% above the previous, to avoid accidental triggers due to small changes in the ambient light and so it detects the sharp increase near the wall. 

When the interrupt flag is raised, code in main is run to determine whether an increase or decrease has been detected. A decrease causes the ambient value to be updated, and an increase causes the wall detect flag to be set.

### Color Detection
For more reliable color detection, RGB values are converted to HSV (hue, saturation, value). Though this requires slightly more computation compared to just using the RGB values, we believed that the benefits outweighed the extra computational complexity:
1. Most of the colors can be distinguished from each other just by comparing the hue value (and also the saturation value in a few limited cases) which are all quite distinct from each other. This makes threshold setting for color recognition much simpler than using RGB, where 3 variables need to be altered. 

1. During testing we found that RGB readings for the same color card fluctuated greatly depending on ambient light levels, whereas the use of HSV values eliminated this problem as the values were consistent regardless of

The ```rgb_2_hsv``` function achieves this, by converting the inputted RGB values into HSV values and storing it in a structure. Extreme care was taken during the conversion arithmetic to ensure that were no negative numbers or floats at all to avoid casting errors.

### Lost Function
When a color is not recognised, for example when the buggy reaches a black wall, the buggy will return to its starting position. The time taken for each straight movement, as well as the sequence of moves performed, are stored in arrays, which can be read back to retrace the path taken. On its return, the buggy will ensure that it is aligned correctly by reversing into the wall after each turn.

### Code Optimisation
The use of floats and negative numbers are avoided entirely to ensure memory is utilised efficiently and avoid errors in casting negative numbers into an unsigned int for example. Variables are also declared as chars where possible to conserve memory, with ints only needed here when dealing with timer and color values, as these must exceed the limits of int capacity.

Global variables are kept to a minimum and are only used when values are set by interrupts. Therefore pointers are used when variables need to be changed in other functions, which minimises opportunities for values to be changed by mistake.

## Reflections and Future Improvements
1. A major issue we encountered, and didn't have time to fix, was the use of a low priority timer interrupt as an alternative method to initiate the lost function. This would have meant that the buggy would return home after no wall had been detected in >16 seconds, which would ensure that it could return home if the wall detection failed and avoid any timer overflow issues. This worked in isolation but could not be made to function with the final full system. With more time this could be fixed and the functions to operate it are left (commented out) in the code.

1. In the final test, the buggy performed mostly as expected. When correctly positioned, all colors were identified correctly, and the movement, return and lost functions worked as expected. A hair stuck in one wheel affected turning performance in one maze, but the ambient light in the test room provided a greater challenge. The wall detection functionality was compromised by this, meaning that it was less reliable than in previous tests. This could be fixed with further calibration of the interrupt thresholds in the test room conditions.

1. Further work could be carried out to improve performace overall. Optimisation of return routes could be implemented to avoid doubling back on itself once the route is known.
   
1. Physical improvements to the buggy, for example to the wheel surface, could allow more consistent turning, especially on the rough floors that it was tested on.

## Code Details
### File Structure
Source Files | Key Functionality
---------|---------
```main.c``` | <ul><li>Declare global variables</li><li>Call initialisation functions</li><li>Overall system operation (see flow chart above)</li></ul>
```color.c/h``` | <ul><li>```color_click_init```: Initialise the color click</li><li>```color_read```: Obtain RGBC readings</li><li>```rgb_2_hsv```: Process the RGBC readings into HSV values</li><li>```color_detect```: Determine color based on these values</li></ul>
```i2c.c/h``` | <ul><li>Enable I2C communication between the color click and the clicker board</li></ul>
```dc_motor.c/h``` | <ul><li>```initDCmotorsPWM```: Initialise motors</li><li>```turnLeft```,```fullSpeedAhead```,etc.: Define individual movements</li><li>```move```: Call sets of movements depending on color detected</li><li>```returnHome```: Return to starting position</li><li>```calibration```: Calibrate the turning angle</li></ul>
```interrupts.c/h``` | <ul><li>```Interrupts_init```: Initialise interrupts </li><li>```__interrupt(high_priority)```,```__interrupt(low_priority)```: Define high and low priority ISRs</li></ul>
```timers.c/h``` | <ul><li>```Timer0_init```: Initialise timer</li><li>```get16bitTMR0val```: Read timer values</li></ul>
```buggysetup.c/h``` | <ul><li>```Buggy_init```: Initialise all LEDs and buttons on clicker board and all headlights and taillights on buggy</li></ul>
```battery.c/h``` | <ul><li>```batteryLevel```: Obtain battery charge level and display on LEDs </li><li>Calls functions from ```ADC.c```: </li></ul>
```serial.c/h``` | <ul><li>Enables serial communication with computer to allow for easy debugging (unused in operation)</li></ul>

### Structures
Structure | Purpose
---------|---------
```DC_motor``` | <ul><li>Contains motor control parameters required for ```setMotorPWM``` function</li><li>Separate structure declared for each motor</li></ul>
```RGBC_val``` | <ul><li>Contains red, green, blue and clear channel values obtained from color click</li></ul>
```HSV_val``` | <ul><li>Contains hue, saturation and value calculated</li></ul>
```HSV_calib``` | <ul><li>Contains hue, saturation and value measured for color cards during calibration</li><li>Separate structure declared for each color of card</li></ul>

### Interrupts
Interrupt | Priority | Purpose
---------|---------|---------
Color Click | High | <ul><li>Triggered when clear channel value falls outside of threshold, indicating that the light level has changed</li><li>Sets ```brightnessChange = 1```, causing the ambient reading to be updated and checked against the previous ambient reading</li><li>See Key Features section above for details on wall detection</li></ul>
Timer | Low | <ul><li>Triggered on timer overflow (16 seconds since last timer reset)</li><li>Sets ```lost = 1```, instructing the buggy to return home</li><li>Currently turned off due to bug (see Reflections and Future Improvements section for details)</li></ul>









<br/><br/><br/><br/>
# Project Brief
## Challenge brief

Your task is to develop an autonomous robot that can navigate a "mine" using a series of instructions coded in coloured cards and return to its starting position.  Your robot must be able to perform the following: 

1. Navigate towards a coloured card and stop before impacting the card
1. Read the card colour
1. Interpret the card colour using a predefined code and perform the navigation instruction
1. When the final card is reached, navigate back to the starting position
1. Handle exceptions and return back to the starting position if final card cannot be found

## "Mine" environment specification

A "mine" is contstructed from black plywood walls 100mm high with some walls having coloured cards located on the sides of the maze to assist with navigation. The following colour code is to be used for navigation:

Colour | Instruction
---------|---------
Red | Turn Right 90
Green | Turn Left 90
Blue | Turn 180
Yellow | Reverse 1 square and turn right 90
Pink | Reverse 1 square and turn left 90
Orange | Turn Right 135
Light blue | Turn Left 135 
White | Finish (return home)
Black | Maze wall colour

Mine courses will vary in difficulty, with the simplest requiring 4 basic moves to navigate. More advanced courses may require 10 or moves to navigate. The mines may have features such as dead ends but colour cards will always direct you to the end of the maze. Once the end of the maze has been reached, you must return to the starting position. An example course to navigate is shown below. You do not know in advance which colours will be in the course or how many.

![Navi Diagram](gifs/maze.gif)

## Resources and project submission

To develop your solution you have your Clicker 2 board, buggy and colour click add on board. You must not use any hardware that is not provided as part of this course and you must use the XC8 compiler to compile your C code. 

Please use this GitHub repo to manage your software development and submit your project code. 

Final testing will take place in the CAGB foyer and testing areas will be provided around the mechatronics lab. You will not know the exact layout of the maze(s) in advance. You will also be asked to give a short presentation on the testing day.

## Supplementary technical information

### Additional buggy features

In addition to the motor drives we explored in lab 6, the buggy contains some additional features that may be of use during the project. The first feature is additional LEDs, controlled through the pins labelled **H.LAMPS**, **M.BEAM**, **BRAKE**, **TURN-L** and **TURN-R**. H.LAMPS turns on the front white LEDs and rear red LEDs, at a reduced brightness. M.BEAM and BRAKE enable you to turn these LEDs on at full brightness. The turn signals have not hardware based brightness control. These LEDs give you a method to provide feedback for debugging, in addition of the LEDs on the clicker board.

![Buggy pinout](gifs/buggy_pins.png)

A further feature of the buggy is **BAT-VSENSE** pin which allows you to monitor the batter voltage via an analogue input pin. The battery is connected across a voltage divider, as shown in the diagram below:

![Bat sense](gifs/bat_vsense.png)

The voltage at BAT-VSENSE will always be one third of that at the battery. Measuring this value can be useful for determining if your battery needs charging. You could also use it to calibrate your robot to perform well at different charge levels. 

### Colour click

The Colour click board contains 2 devices to help with navigation, a tri-colour LED for illumination and a 4 channel RGBC photodiode sensor. This combination of two devices (an illumination source and a sensor) allow you a make measurements of the reflected colour of objects near the sensor. The circuit diagram for the Colour click is shown below:

![Color Cick](gifs/color_click.png)

The tri-colour LED is the simpler of the two devices to control. Three separate pins control the red, green and blue LEDs individually, despite them being in a single package. Control via these pins is simple digital on/off control and if any brightness control was required, the user would need program the microcontroller to generate a PWM signal to achieve this.  

The second device on the Colour click is the TCS3471 colour light-to-digital converter. The sensor contains a grid of 4x4 photodiodes, 4 are sensitive to red light, 4 green light, 4 blue light and 4 "clear" light (that is, a range of wavelengths, see datasheet for exact spectral response). When light falls on the photodiode the photons are absorbed and current is generated. This signal is then integrated over time using integrators and sampled by 16 bit on board ADCs. Communication with the device is achieved using an I2C interface. This enables configuration of the device to customise sampling of the data (i.e. integration time, gain, etc.) and to read the 16 bit digital values for each of the RGBC channels. The relative magnitude of these values gives you information about the colour of light that is falling on the sensor. The device can also be configured to send an interrupt signal to the PIC when signal reaches a preset value.

### I2C

The I2C interface widely used in industry for communication between microcontrollers and peripheral integrated circuits (other chips) over short distances. I2C is serial communication bus that enables communication between many devices over a simple 2 wire interface. One wire is the data line (SDA) and is used for both transmission and receiving. The second wire (SCL) is used for a clock signal to ensure all devices are synchronous. To ensure communication of data occurs without problem a message protocol must be followed and understood by all devices on the bus. Devices are termed master and slave devices, with master devices initiation communication to a slave device via unique address for that device. The general sequence of communication between a master/slave over the I2C interface is a follows:

1. Send a Start bit
1. Send the slave address, usually 7 bits
1. Send a Read (1) or Write (0) bit to define what type of transaction it is
1. Wait for an Acknowledge bit
1. Send a data or command byte (8 bits)
1. Wait for Acknowledge bit
1. Send the Stop bit

This is shown pictorial in the figure below:

![I2C](gifs/i2c.png)

Although it is possible to program an entirely software based I2C interface, luckily for us our PIC chip has a module dedicated to generating and receiving I2C signals: the Master Synchronous Serial Port Module, or MSSP (see chapter 28 of the PIC datasheet). This module provides methods for sending start/stop/acknowledge bits and allows us to focus on sending/receiving data.

The included i2c.c/h files contain functions to help you get started with I2C communication. The first function below sets up the MSSP module as an I2C master device and configures the necessary pins.

	void I2C_2_Master_Init(void)
	{
		//i2c config  
		SSP2CON1bits.SSPM= 0b1000;    // i2c master mode
		SSP2CON1bits.SSPEN = 1;       //enable i2c
		SSP2ADD = (_XTAL_FREQ/(4*_I2C_CLOCK))-1; //Baud rate divider bits (in master mode)
  
		//pin configuration for i2c  
		TRISDbits.TRISD5 = 1;                   //Disable output driver
		TRISDbits.TRISD6 = 1;                   //Disable output driver
		ANSELDbits.ANSELD5=0;					// disable analogue on pins
		ANSELDbits.ANSELD6=0;					// disable analogue on pins
		SSP2DATPPS=0x1D;      //pin RD5
		SSP2CLKPPS=0x1E;      //pin RD6
		RD5PPS=0x1C;      // data output
		RD6PPS=0x1B;      //clock output
	}
	
Bits with the SSP2CON2 register are set to send the individual start/stop/acknowledge bits used in the protocol. These must only be set when the bus is idle (nothing being sent/received). The I2C_2_Master_Start(), I2C_2_Master_Stop() and I2C_2_Master_RepStart() functions allow you add the necessary bits as defined in the protocol above. Data is sent on the bus using the SSP2BUF register:

	void I2C_2_Master_Write(unsigned char data_byte)
	{
		I2C_2_Master_Idle();
		SSP2BUF = data_byte;         //Write data to SSPBUF
	}

Data is also read using the same SSP2BUF registers. However, to receive data we first need to switch the module into receiver mode. We also need to start the acknowledge sequence to let the slave device know what we have received the data OK. The following function achieves this:

	unsigned char I2C_2_Master_Read(unsigned char ack)
	{
		unsigned char tmp;
		I2C_2_Master_Idle();
		SSP2CON2bits.RCEN = 1;        // put the module into receive mode
		I2C_2_Master_Idle();
		tmp = SSP2BUF;                //Read data from SS2PBUF
		I2C_2_Master_Idle();
		SSP2CON2bits.ACKDT = !ack;     // 0 turns on acknowledge data bit
		SSP2CON2bits.ACKEN = 1;        //start acknowledge sequence
		return tmp;
	}

The functions described so form the basics required for I2C communication with the PIC. To communicate with the TCS3471 onboard the Colour click we first need to know its address. This is listed in the data sheet as 0x29. To send our first byte over the I2C we need to send this address combined with the transaction type (read or write) as defined in the protocol diagram above. This lets the TCS3471 know the message is intended for it and not some other device on the interface. Next we send a byte which is a combination of command type and the register address in the TCS3471 that we want to write to. Finally we the value that we want to write to that register. This sequence is shown in the function below:

	void color_writetoaddr(char address, char value){
		I2C_2_Master_Start();         		//Start condition
		I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write (0) mode (note 0x52=0x29<<1)
		I2C_2_Master_Write(0x80 | address);    //command + register address
		I2C_2_Master_Write(value);    			//value to store in the register
		I2C_2_Master_Stop();          //Stop condition
	}

We then call the function to, for example, turn the device on:

	color_writetoaddr(0x00, 0x01); // write 1 to the PON bit in the device enable register
	
There are additional commands that must be set to initialise the device and many registers that be configured to obtain optimal performance for the sensor in your conditions. It is up to you to carefully read the TCS3471 datasheet and experiment with this.

To read values from the TCS3471 you need to a similar sequence to above but you first need to tell the device which register you want to deal with, before telling the device you want read from it. The example below uses the combined read format to read multiple bytes in sequence. The auto-increment is set so that instead of reading the same register over and over again, it automatically advances to the next one. The example starts at the Red channel low byte address and then automatically advances and reads the associated high byte.

	unsigned int color_read_Red(void)
	{
		unsigned int tmp;
		I2C_2_Master_Start();         //Start condition
		I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
		I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
		I2C_2_Master_RepStart();
		I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
		tmp=I2C_2_Master_Read(1);			// read the Red LSB
		tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
		I2C_2_Master_Stop();          //Stop condition
		return tmp;
	}

Instead of separate functions for each channel you may want to create a structure to store all the values together, and pass a pointer to the function so that all values in the structure can be updated in one operation. An example structure might look like this:

	//definition of RGB structure
	struct RGB_val { 
		unsigned int R;
		unsigned int G;
		unsigned int B;
	};

This concludes the basics of I2C and communication with the colour sensor. Best of luck! 
