#include <ascii/grid.h>
#include <stdlib.h>
#include <math.h>
#define W 1920
#define H 1080



Color gen(Position pos){
	pos.x -= W/2;
	pos.y -= H/2;
	double ang = atan2(pos.y, pos.x);
	Vector max = vectorCreate(W/4.0, H/4.0);
	double mag = vectorMag(pos);
	double s = mag / vectorMag(max);
	return colorHSVToRGB(ang * (180.0 / M_PI), s * 255.0, s * 255.0);
}


int main(){
	gridOpen_(W, H, NULL, NULL, gen);
	
	gridDraw("out");
	gridClose();
	return 0;
}
