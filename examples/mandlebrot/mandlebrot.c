#include <ascii/grid.h>
#include <stdlib.h>
#include <complex.h>
#include <stdio.h>
#include <math.h>
#define INF 32
//#define MAX_ITER 8e3
#define WIDTH 400 
#define HEIGHT 400 
double ang = 0.13201;
double MAX_ITER = 0;
Position point;
Position len;
complex double mandlebrot(complex double val, complex double offset);
void setup(){
	len = vectorCreate(10, 10);
	double r = 0.25;	
	point = vectorCreate(r * cos(ang) - 1, r * sin(ang));
}
complex double mandlebrot(complex double val, complex double offset){
	return val * val + offset;
}
Color gen(Position pos, Frame frame){
	double x = point.x + (double) ((pos.x / WIDTH) - 0.5) * len.x;
	double y = point.y + (double) ((pos.y / (HEIGHT)) - 0.5) * len.y;
	
	complex double v = mandlebrot(0, x + y * I);
	int cur;
	for(cur = 0; cur < MAX_ITER; cur++){
		if(cabs(v) > INF)
			break;
		v = mandlebrot(v, x + y * I);
	}
	int vl = (cur < MAX_ITER ? 255 : 0);
	return colorHSVToRGB(cur % 360, 255, vl);
}
void update(Frame frame){
	len.x *= 0.99;
	len.y *= 0.99;
	MAX_ITER += 1;
	gridClear();	
}
int main(int argc, char *argv[]){
	if(argc == 3)
		point = vectorCreate(strtod(argv[1], NULL), strtod(argv[2], NULL));
	gridOpen(WIDTH, HEIGHT, ASCII_FONT_TINY, setup, update, gen);
	gridSetMaxFrame(-1);
	gridDraw(NULL);
	gridClose();
	return 0;
}
