#ifndef ASCII_UTILL_H
#define ASCII_UTILL_H
#include "vector.h"
#define ASCII_CLEAR_SCREEN "\033[2J"
#define ASCII_CLEAR_SCREEN_SIZE 4
#define ASCII_RESET_TERM "\033c"
#define ASCII_RESET_TERM_SIZE 2
#define ASCII_MOVE_CURSOR(x, y) "\033["#x";"#y"H"
#define ASCII_RESET_CURSOR_SIZE 6
#define ASCII_TURN_CURSOR_OFF "\033[?25l"
#define ASCII_TURN_CURSOR_OFF_SIZE 6
#define ASCII_TURN_CURSOR_ON "\033[?25h"
#define ASCII_TURN_CURSOR_ON_SIZE 6




//returns the max between two numbers
inline double maxf(double a1, double a2){
	if(a1 > a2)
		return a1;
	return a2;
}

//returns the min between two numbers
inline double minf(double a1, double a2){
	if(a1 < a2)
		return a1;
	return a2;
}

//returns the sign of a number
inline double signf(double n){
	if(n > 0){
		return 1;
	}
	if(n < 0){
		return -1;
	}
	return 0;
}

//clamps the value
inline double clamp(double value, double lower, double upper){
	if(value < lower)
		return lower;
	if(value > upper)
		return upper;
	return value;
}

//turns an x,y pair into an index
inline unsigned posToIndex(Position pos, unsigned width){
	return pos.x + pos.y * width;
}


#endif //ASCII_UTILL_H
