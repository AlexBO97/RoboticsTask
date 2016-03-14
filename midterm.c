/*
  IR Roaming.c

  Use IR LEDs and IR receivers to detect obstacles while roaming.

*/

#include "simpletools.h"                        // Library includes
#include "abdrive.h"

int irLeft, irRight;                            // IR variables
int target = 20;
int multiplier = 5;
int baseSpeed = 24;
int changeVal = 0;

int getIR() {
    irLeft = 0;
    irRight = 0;
    for (int dacVal = 0; dacVal < 160; dacVal += 8) {
        dac_ctr(26, 0, dacVal);                       
        freqout(11, 1, 38000);                        
        irLeft += input(10);
        
        dac_ctr(27, 1, dacVal);
        freqout(1, 1, 38000);
        irRight += input(2);
    }
}

void calcChangeVal(int irVal){
  int difference = target - irVal;
  changeVal = multiplier * difference;  
}  

int main()                                      // main function
{
  low(26);                                      // D/A0 & D/A1 to 0 V
  low(27);
  
  int rampStep = 4;
 
  
  while(1)
  {
    getIR();
    
    
    //Sensor navigation
    
    if(irRight >= target && irLeft >= target) {                   // No obstacles?
      drive_speed(baseSpeed, baseSpeed);                          // ...full speed ahead
    } else if(irRight < target) {                                 // Just right obstacle?
      calcChangeVal(irRight);
      drive_speed(baseSpeed - changeVal, baseSpeed + changeVal);  // ...rotate left
    } else if(irLeft < target) {                                  // Just left obstacle?
      calcChangeVal(irLeft);
      drive_speed(baseSpeed + changeVal, baseSpeed - changeVal);  // ...rotate right
    }      
  }
}