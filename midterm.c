/*
 IR Roaming.c
 
 Use IR LEDs and IR receivers to detect obstacles while roaming.
 
 */

#include "simpletools.h"                        // Library includes
#include "abdrive.h"
#include "ping.h"

int DO = 22, CLK = 23, DI = 24, CS = 25;            // SD card pins on Activity Board

void writeToFile(int x, int y)
{
    int erc = sd_mount(DO, CLK, DI, CS);              // Mount SD card
    if(!erc)                                          // Error code = 0, good, continue
    {
        FILE* fp = fopen("test.txt", "w");              // Open a file for writing
        
        if(fp)                                          // Nonzero file pointer?
        {                                               // Good, continue
            char strx[] = x;
            char stry[] = y;
            fwrite(strx, 1, 21, fp);                    // Add contents to the file
            fwrite(stry, 1, 21, fp);                    // Add contents to the file
            fwrite("\n", 1, 21, fp);                    // Add contents to the file
        }
        else                                            // Zero file pinter?
        {                                               // Bad, error message.
            print("File did not open.\n");
        }
        
        fclose(fp);                                     // Close the file
    }
    else                                              // Mount error code not zero?
    {                                                 // Bad, display code
        print("Error opening card.");
        print("error code = %d\n", erc);
    }
}
void readFromFile()
{
    fp = fopen("test.txt", "r");                    // Reopen file for reading.
    
    char s[80];                                     // Buffer for characters.
    
    if(fp)                                          // Nonzero file pinter?
    {                                               // Good, continue.
        print("First 21 chars in test.txt:\n");       // Display heading
        fread(s, 1, 21, fp);                          // Read 21 characters
        print("%s", s);                               // Display them
        print("\n");                                  // With a newline at the end.
    }
    else                                            // Zero file pointer?
    {                                               // Bad, print error.
        print("File did not open.\n");
        print("\n");
    }
    fclose(fp);
}

int irLeft, irRight;                            // IR variables
int target = 20;
int multiplier = 5;
int baseSpeed = 24;
int changeVal = 0;
int atWall = 0;
int distance = 0;

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
    
    struct node *end;
    end = (struct node *) malloc(sizeof(struct node));
    
    int rampStep = 4;
    
    while(1)
    {
        getIR();
        
        //Sensor navigation
        if (!atWall) {
            struct node *current;
            current = (struct node *) malloc(sizeof(struct node));
            
            if(distance < 10) {
              drive_speed(baseSpeed, baseSpeed);
              rotate180();
              end = previous;
            } else if(irRight >= target && irLeft >= target) {                   // No obstacles?
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
            writeToFile(current.DataLeft,current.dataRight);
            previous = current;
        }

    }
    
}