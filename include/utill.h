#ifndef ASCII_UTILL_H
#define ASCII_UTILL_H
#include "vector.h"
//returns the max between two numbers
double maxf(double a1, double a2);
//returns the min between two numbers
double minf(double a1, double a2);
//returns the sign of a number
double signf(double n);
//clamps the value
double clamp(double value, double lower, double upper);
//turns an x,y pair into an index
unsigned posToIndex(Position pos, unsigned width);
#endif //ASCII_UTILL_H
