#include "math.h"

#define MM_PER_TICK 3.25;
#define WIDTH 105.8;
#define PI_VAL 3.14159265358979323846;

double ticksToDistance(int ticks);

double calcAngle(double lDist, double rDist);

double calcMidRad(double lDist, double rDist, double angle);

double calcXChangeStraight(double dist, double runningAngle);

double calcYChangeStraight(double dist, double runningAngle);

double calcXChangeAngle(double midRad, double angle);

double calcYChangeAngle(double midRad, double angle, double runningAngle);

double calcHyp(double dX, double dY);

double calcFinalAngle(double dX, double dY);

double toDegrees(double radians);