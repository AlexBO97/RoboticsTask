/*
 IR Roaming.c
 
 Use IR LEDs and IR receivers to detect obstacles while roaming.
 
 */

//int DO = 22, CLK = 23, DI = 24, CS = 25;            // SD card pins on Activity Board

// void writeToFile(int x, int y)
// {
//     int erc = sd_mount(DO, CLK, DI, CS);              // Mount SD card
//     if(!erc)                                          // Error code = 0, good, continue
//     {
//         FILE* fp = fopen("test.txt", "w");              // Open a file for writing
        
//         if(fp)                                          // Nonzero file pointer?
//         {                                               // Good, continue
//             char strx[] = x;
//             char stry[] = y;
//             fwrite(strx, 1, 21, fp);                    // Add contents to the file
//             fwrite(stry, 1, 21, fp);                    // Add contents to the file
//             fwrite("\n", 1, 21, fp);                    // Add contents to the file
//         }
//         else                                            // Zero file pinter?
//         {                                               // Bad, error message.
//             print("File did not open.\n");
//         }
        
//         fclose(fp);                                     // Close the file
//     }
//     else                                              // Mount error code not zero?
//     {                                                 // Bad, display code
//         print("Error opening card.");
//         print("error code = %d\n", erc);
//     }
// }
// void readFromFile()
// {
//     fp = fopen("test.txt", "r");                    // Reopen file for reading.
    
//     char s[80];                                     // Buffer for characters.
    
//     if(fp)                                          // Nonzero file pinter?
//     {                                               // Good, continue.
//         print("First 21 chars in test.txt:\n");       // Display heading
//         fread(s, 1, 21, fp);                          // Read 21 characters
//         print("%s", s);                               // Display them
//         print("\n");                                  // With a newline at the end.
//     }
//     else                                            // Zero file pointer?
//     {                                               // Bad, print error.
//         print("File did not open.\n");
//         print("\n");
//     }
//     fclose(fp);
// }


#include "simpletools.h"                        // Library includes
#include "abdrive.h"
#include "ping.h"


int irLeft, irRight;                            // IR variables
int target = 8;
int multiplier = 4;
int baseSpeed = 24;
int changeVal = 0;
int atWall = 0;
int distance;

struct node {
    int dataRight;
    int dataLeft;
    struct Node* next;
    struct Node* prev;
};

void getIR() {
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
    drive_goto(51,-51);
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
    begin->dataLeft = 0;
    begin->dataRight = 0;
    
    struct node *previous;
    previous = begin;
    
    int rampStep = 4;
    
    
    
    while(1)
    {
        
        // Sensor navigation
        if (!atWall) {
            
            getIR();
            distance = ping_cm(8);
            
            struct node *current;
            current = (struct node *) malloc(sizeof(struct node));
            
            if(distance < 15) {
              drive_speed(baseSpeed, baseSpeed);
              rotate180();
              atWall = 1;
              current = previous;
            } else {
              if(irRight >= target && irLeft >= target) {                   // No obstacles?
                  drive_speed(baseSpeed, baseSpeed);                          // ...full speed ahead
                 current->dataLeft = baseSpeed;
                 current->dataRight = baseSpeed;
              } else if(irRight < target) {                                 // Just right obstacle?
                 calcChangeVal(irRight);
                 drive_speed(baseSpeed - changeVal, baseSpeed + changeVal);  // ...rotate left
                 current->dataLeft = baseSpeed - changeVal;
                 current->dataRight = baseSpeed + changeVal;
              } else if(irLeft < target) {                                  // Just left obstacle?
                  calcChangeVal(irLeft);
                  drive_speed(baseSpeed + changeVal, baseSpeed - changeVal);  // ...rotate right
                  current->dataLeft = baseSpeed + changeVal;
                  current->dataRight = baseSpeed - changeVal;
              }
              current->prev = previous;
              previous->next = current;
              //pause(50);
            
              //writeToFile(current->DataLeft,current->dataRight);
              previous = current;
            }             
        } else { // Back tracking
          if(previous->prev != NULL) {
            print("%d %d\n", previous->dataLeft, previous->dataRight);
            drive_speed(previous->dataRight, previous->dataLeft);
            previous = previous->prev;
            pause(65);
          } else {
            drive_speed(0, 0);
          }                       
        }          
    }    
}