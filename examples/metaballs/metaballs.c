#include <ascii/grid.h>
#include <ascii/utill.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#define WIDTH 400
#define HEIGHT 400
#define NUM_BALLS 2
Position balls[NUM_BALLS];
Direction dirs[NUM_BALLS];
double radii[NUM_BALLS];

void setup(){
	for(int i = 0; i < NUM_BALLS; i++){
		balls[i] = vectorCreate(rand() % WIDTH, rand() % HEIGHT); 
		dirs[i] = vectorCreate((rand() % 200 - 100.0) / 100.0, (rand() % 200 - 100.0) / 100.0); 
		radii[i] = rand() % 60 + 40;
	}
}
void update(){
	for(int i = 0; i < NUM_BALLS; i++){
		balls[i] = vectorAdd(balls[i],  vectorScale(dirs[i], 0.5));
		if(balls[i].x > WIDTH || balls[i].x < 0)
			dirs[i].x *= -1;

		if(balls[i].y > HEIGHT || balls[i].y < 0)
			dirs[i].y *= -1;
	}
	gridClear(colorCreate(0, 0, 0));
}

Color gen(Position pos){
	double h = 0;
	
	for(int i = 0; i < NUM_BALLS; i++){
		double m = pow(pos.x - balls[i].x, 2) + pow(pos.y - balls[i].y, 2);
		h += 1e4 * (radii[i]/m);
	}
	return colorHSVToRGB(h, 255, 255);
}

int main(){
	gridOpen(WIDTH, HEIGHT, ASCII_FONT_TINY, setup, update, gen); 
	gridSetMaxFrame(300);
	gridDraw("output/out");
	gridClose();
	return 0;

}
