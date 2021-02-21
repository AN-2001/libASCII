#include <ascii/grid.h>
#include <ascii/ray.h>
#include <gd.h>
#include "ascii/triangle.h"
#include <time.h>
#include <ascii/mat2x3.h>
#include <math.h>
#define WIDTH 400
#define HEIGHT 400

//don't change this, it wont be an infinite gif anymore!
#define MAX_FRAME 210	

#define ARR_SIZE(arr) ( sizeof(arr) /sizeof(arr[0]) )
#define VERTS 50

Triangle triangles[VERTS];


void setup(){
	//get the triangles from the vertecies
	double step = (2*M_PI) / (double)VERTS;
	for(int i = 0; i < VERTS ; i ++){
		Vector first = {100.0 * cos(i*step), 100.0 * sin(i*step)};
		Vector zero  = {0, 0};
		Vector second = {100.0 * cos(i*step + step), 100.0 * sin(i*step + step)};
		Vector three[3] = {first, zero, second};
		triangles[i] = triangleCreate(three);
	}
}

void update( Frame_t frame){
	gridClear();
	srand(time(NULL));
	//rotate the triangles and move them using dirs
	for(int i = 0; i < ARR_SIZE(triangles); i++){
		Vector arr[3] = {{cos(0.03), sin(0.03)}, {-sin(0.03), cos(0.03)}};
		Mat2x3 rotate = mat2x3Create(arr);		
		triangleTransform(triangles + i, &rotate);
		if(i % 2 == 0){	
			Vector t[4] = {{1, 0}, {0, 1}, {1 ,-1}};
			Mat2x3 trans = mat2x3Create(t);		
			triangleTransform(triangles + i , &trans);
		}else{
			Vector t[4] = {{1, 0}, {0, 1}, {-1 ,1}};
			Mat2x3 trans = mat2x3Create(t);		
			triangleTransform(triangles + i , &trans);
		}
	}
}

Color generate(Position pos, Dimention dim, Frame_t frame){
	Color output = colorCreate(0, 0, 0);
	srand(pos.x + pos.y);
	for(int i = 0; i < sizeof(triangles) / sizeof(triangles[0]);i++){
		Vector trans[3] = {{1,0}, {0, 1}, {200, 200}};
		Mat2x3 translate = mat2x3Create(trans);
		Triangle translated = triangleTransformed(triangles[i], &translate); 
		if(triangleIsPointInside(translated, pos)){
			output =  colorCreate( 255,  255 ,  255);
		}


	}

	return output;
}

int main(){
	gridOpen(WIDTH, HEIGHT, setup, update, generate);
	gridSetMaxFrame(MAX_FRAME);
	gridDrawToImage("output/out");
	gridClose();	
	return 0;
}
