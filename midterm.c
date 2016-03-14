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
int atWall = 0;

struct node {
    int dataRight;
    int dataLeft;
    struct Node* next;
    struct Node* prev;
}

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

void rotate180(){
    drive_goto(51,51);
}

void calcChangeVal(int irVal){
    int difference = target - irVal;
    changeVal = multiplier * difference;
}

int main()                                      // main function
{
    low(26);                                      // D/A0 & D/A1 to 0 V
    low(27);
    struct node *begin;
    begin = (struct node *) malloc(sizeof(struct node));
    begin->prev = NULL;
    begin.dataLeft = 0;
    begin.dataRight = 0;
    
    struct node *previous;
    previous = (struct node *) malloc(sizeof(struct node));
    previous = begin;
    int rampStep = 4;
    
    while(1)
    {
        getIR();
        
        //Sensor navigation
        while (!atWall) {
            struct node *current;
            current = (struct node *) malloc(sizeof(struct node));
            
            if(irRight >= target && irLeft >= target) {                   // No obstacles?
                drive_speed(baseSpeed, baseSpeed);                          // ...full speed ahead
                current.dataLeft = baseSpeed;
                current.dataRight = baseSpeed;
            } else if(irRight < target) {                                 // Just right obstacle?
                calcChangeVal(irRight);
                drive_speed(baseSpeed - changeVal, baseSpeed + changeVal);  // ...rotate left
                current.dataLeft = baseSpeed - changeVal;
                current.dataRight = baseSpeed + changeVal;
            } else if(irLeft < target) {                                  // Just left obstacle?
                calcChangeVal(irLeft);
                drive_speed(baseSpeed + changeVal, baseSpeed - changeVal);  // ...rotate right
                current.dataLeft = baseSpeed - changeVal;
                current.dataRight = baseSpeed + changeVal;
            }
            previous->next = current;
            current->prev = previous;
            previous = current;
            
        }

    }
    
}