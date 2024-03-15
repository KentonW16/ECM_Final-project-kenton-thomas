#include <xc.h>
#include "battery.h"
#include "ADC.h"

/******************************************
 * function to read battery level and display as 2 bit binary value on LEDs
******************************************/
void batteryLevel(void){
    ADC_init(); //requires ADC to read battery voltage from pin
    unsigned char batteryVoltage;
    unsigned char batteryQuarter;
    
    // Read battery voltage
    batteryVoltage = ADC_getval();       //value between 0 and 255 (battery full = 95)
    batteryQuarter = batteryVoltage/24;  //95/4 = 24
    
    // Display to LEDs
    if (batteryQuarter & 0b10) {LATDbits.LATD7=1;} else {LATDbits.LATD7=0;}
    if (batteryQuarter & 0b01) {LATHbits.LATH3=1;} else {LATHbits.LATH3=0;}
}
