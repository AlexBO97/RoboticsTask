#include "logging.h"
#include "math.h"

double ticksToDistance(int ticks) // Converts a number of ticks into a distance value in mm
{
  return ticks * MM_PER_TICK;
}

double calcAngle(double lDist, double rDist) {
  return (lDist - rDist) / WIDTH;
}

double calcMidRad(double lDist, double rDist, double angle) {
    return (1 / angle) * (lDist + rDist) / 2;
}

double calcXChangeStraight(double dist, double runningAngle) {
  return dist * cos(runningAngle);
}  

double calcYChangeStraight(double dist, double runningAngle) {
  return dist * sin(runningAngle);
}  

double calcXChangeAngle(double midRad, double angle) {
  return midRad - midRad * sin(angle);
}     

double calcYChangeAngle(double midRad, double angle, double runningAngle) {
  double p = midRad * sin(angle + runningAngle);
  double q = midRad * sin(runningAngle);
  return p - q;
}

double calcHyp(double dX, double dY) {
  double hyp = sqrt(dX * dX + dY * dY);
  return hyp;
}

double calcFinalAngle(double dX, double dY) {
  double angle;
  if ( dX == 0 && dY > 0 ) {
    angle = (3.14159265358979323846 / 2);
  } else if ( dX == 0 && dY < 0 ) {
    angle = (3 * 3.14159265358979323846 / 2);
  } else if ( dX < 0 && dY == 0 ) {
    angle = 3.14159265358979323846;
  } else if ( dX >= 0 && dY < 0) {
    angle = (2*3.14159265358979323846 + atan( dY / dX ));
  } else if ( dX < 0 ) {
    angle = (3.14159265358979323846 + atan( dY / dX ));
  } else {
    angle = atan( dY / dX );
  }     
  return angle; 
}

double toDegrees(double radians) {
  return radians * ( 180 / 3.14159265358979323846 );
}

void rotate180() {
    drive_speed(1,1);
    drive_goto(51,-51);
}

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

