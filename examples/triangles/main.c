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
#define MAX_FRAME 209

#define ARR_SIZE(arr) ( sizeof(arr) /sizeof(arr[0]) )

//set the vertecies of the shape
Vector vertecies[] = {{WIDTH/4, -HEIGHT/4}, {-WIDTH/4, -HEIGHT/4}, {WIDTH/4, HEIGHT/4}, {-WIDTH/4, HEIGHT/4}};

//give it the order that you want to draw the triangles in
int order[] = {0, 1, 2, 1, 3, 2};

Triangle triangles[ARR_SIZE(order) / 3];

//this part is for animation, it tells each triangle how to move
Vector dirs[2][4] = {{{-1, 1}, {1, -1}, {-1,1}, {1,-1}},
					 {{1, -1}, {-1, 1}, {1,-1}, {-1,1}}};

void setup(){
	//get the triangles from the vertecies
	for(int i = 0; i < ARR_SIZE(order); i += 3){
		Vector three[3] = {vertecies[order[i]], vertecies[order[i+1]], vertecies[order[i+2]]};
		triangles[i / 3] = triangleCreate(three);

	}
}
void update(Frame frame){
	gridClear();
	//rotate the triangles and move them using dirs
	for(int i = 0; i < ARR_SIZE(triangles); i++){
		int div = MAX_FRAME / ARR_SIZE(dirs[0]);
		Vector arr[3] = {{cos(0.03), sin(0.03)}, {-sin(0.03), cos(0.03)}, dirs[i][frame / div]};
		Mat2x3 rotate = mat2x3Create(arr);		
		triangleTransform(triangles + i, &rotate);
	}
}
Color generate(Position pos, Dimention dim, Frame frame){
	Color output = colorCreate(0, 0, 0);
	srand(pos.x + pos.y);
	for(int i = 0; i < ARR_SIZE(triangles);i++){
		//first translate everything to the center of the screen
		Vector trans[3] = {{1,0}, {0, 1}, {WIDTH/2, HEIGHT/2}};
		Mat2x3 translate = mat2x3Create(trans);
		Triangle translated = triangleTransformed(triangles[i], &translate); 
		//seed the random generator
		double dist = triangleDistToPoint(translated, pos);
		//check if the ASCII charcater is inside the triangle
		if(triangleIsPointInside(translated, pos)){
			output = colorAdd(output, colorCreate(rand() % 255, rand() % 255 , rand() % 255));
		}
		
		if(dist < 0)
			continue;
		//check if the triangle intersects this ASCII chracter
		if(dist <= 5){
			output = colorAdd(output, colorCreate( 255 , 255 , 255));
		}
	}
	return output;
}

int main(){
	for(;;){
		gridOpen_(WIDTH, HEIGHT, setup, update, generate);
		gridSetMaxFrame(MAX_FRAME);
		gridSetFrameDelay(1e4);
		gridDraw(NULL);
		gridClose();	
	}
	return 0;
}
