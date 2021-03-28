#include <ascii/grid.h>
#include <stdlib.h>
#include <complex.h>
#include <stdio.h>
#include <math.h>
#include <ascii/input.h>

#define INF 64
#define MAX_ITER 2e3 
#define WIDTH 800 
#define HEIGHT 800 

Position point;
Position len;
double ang = 2.12165456165498;
complex double mandlebrot(complex double val, complex double offset);

void setup(){
	len = vectorCreate(1, 1);
	double r = 0.25;	
	point = vectorCreate(r * cos(ang) - 1, r * sin(ang));
}
complex double mandlebrot(complex double val, complex double offset){
	return val * val + offset;
}
Color gen(Position pos){
	double x = point.x + (double) ((pos.x / WIDTH) - 0.5) * len.x;
	double y = point.y + (double) ((pos.y / HEIGHT) - 0.5) * len.y;
	
	complex double v = mandlebrot(0, x + y * I);
	int cur;
	for(cur = 0; cur < MAX_ITER; cur++){
		if(cabs(v) > INF)
			break;
		v = mandlebrot(v, x + y * I);
	}
	int vl = (cur < MAX_ITER ? 255 : 0);
	return colorHSVToRGB(cur, 255, vl);
}
void update(Frame frame){
	Vector mov = vectorCreate(gridGetAxis(ASCII_AXIS_LX), -gridGetAxis(ASCII_AXIS_LY));
	len.x *= 1 + gridGetAxis(ASCII_AXIS_RY) * 0.01f;
	len.y *= 1 + gridGetAxis(ASCII_AXIS_RY) * 0.01f;

	point.x += mov.x * 0.1f * len.x * 0.1f;
	point.y -= mov.y * 0.1f * len.y * 0.1f;
}
int main(int argc, char *argv[]){
	if(argc == 3)
		point = vectorCreate(strtod(argv[1], NULL), strtod(argv[2], NULL));
	gridOpen(WIDTH, HEIGHT, ASCII_FONT_TERM, setup, update, gen);
	gridSetMaxFrame(-1);
	gridDraw(NULL);
	gridClose();
	return 0;
}
