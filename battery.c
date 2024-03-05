#include <xc.h>
#include "battery.h"
#include "ADC.h"

// function to read battery level and display as 2 bit binary value on LEDs
void batteryLevel(void){
    ADC_init();
    unsigned char batteryVoltage;
    unsigned char batteryQuarter;
    
    batteryVoltage = ADC_getval();       //value between 0 and 255 (battery full = 95)
    batteryQuarter = batteryVoltage/24;  //95/4 = 24
    if (batteryQuarter & 0b10) {LATDbits.LATD7=1;} else {LATDbits.LATD7=0;}
    if (batteryQuarter & 0b01) {LATHbits.LATH3=1;} else {LATHbits.LATH3=0;}
}
