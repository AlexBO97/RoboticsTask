#include "simpletools.h"
#include "abdrive.h"
#include "ping.h"

int irLeft, irRight;    // IR variables
int target = 18;
int multiplier = 5;
int baseSpeed = 48;
int changeVal = 0;
int atWall = 0;
int distance;

struct node {
    int dataRight;
    int dataLeft;
    struct Node* next;
    struct Node* prev;
};

int main()
{
    low(26);
    low(27);
    
    struct node *begin;
    begin = (struct node *) malloc(sizeof(struct node));
    begin->prev = NULL;
    begin->dataLeft = 0;
    begin->dataRight = 0;
    
    struct node *previous;
    previous = begin;
    
    int rampStep = 4;
    
    while(1)
    {
        if (!atWall) {  // Sensor navigation
            getIR();
            distance = ping_cm(8);
            
            struct node *current;
            current = (struct node *) malloc(sizeof(struct node));
            
            if (distance < 15) {
                drive_speed(baseSpeed, baseSpeed);
                rotate180();
                atWall = 1;
                current = previous;
            } else {
                if(irRight >= target && irLeft >= target || ((irRight - irLeft > -3) && (irRight - irLeft < 3))){   // No obstacles?
                    drive_speed(baseSpeed, baseSpeed);  // ...full speed ahead
                    current->dataLeft = baseSpeed;
                    current->dataRight = baseSpeed;
                } else if (irRight < target) {  // Just right obstacle?
                    if (irLeft < irRight) {
                        calcChangeVal(irLeft);
                        drive_speed(baseSpeed + changeVal, baseSpeed - changeVal);  // ...rotate right
                        current->dataLeft = baseSpeed + changeVal;
                        current->dataRight = baseSpeed - changeVal;
                    } else {
                        calcChangeVal(irRight);
                        drive_speed(baseSpeed - changeVal, baseSpeed + changeVal);  // ...rotate left
                        current->dataLeft = baseSpeed - changeVal;
                        current->dataRight = baseSpeed + changeVal;
                    }
                } else if (irLeft < target) {   // Just left obstacle?
                    calcChangeVal(irLeft);
                    drive_speed(baseSpeed + changeVal, baseSpeed - changeVal);  // ...rotate right
                    current->dataLeft = baseSpeed + changeVal;
                    current->dataRight = baseSpeed - changeVal;
                }
                current->prev = previous;
                previous->next = current;
                pause(50);
                previous = current;
            }             
        } else {    // Back tracking
            if (previous->prev != NULL) {
                print("%d %d\n", previous->dataLeft, previous->dataRight);
                drive_speed(previous->dataRight, previous->dataLeft);
                previous = previous->prev;
                pause(120);
            } else {
                drive_speed(0, 0);
            }
        }          
    }
}