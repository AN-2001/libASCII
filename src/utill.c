#include "utill.h"

double maxf(double a1, double a2){
	if(a1 > a2)
		return a1;
	return a2;
}

double minf(double a1, double a2){
	if(a1 < a2)
		return a1;
	return a2;
}
double signf(double n){
	if(n > 0){
		return 1;
	}
	if(n < 0){
		return -1;
	}
	return 0;
}

double clamp(double value, double lower, double upper){
	if(value < lower)
		return lower;
	if(value > upper)
		return upper;
	return value;
}
unsigned posToIndex(Position pos, unsigned width){
	return pos.x + pos.y * width;
}
