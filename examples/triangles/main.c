#include <raster/raster.h>
#include <raster/ray.h>
#include <gd.h>
#include "raster/triangle.h"
#include <time.h>
#include <raster/mat2x3.h>
#include <math.h>
#define WIDTH 400
#define HEIGHT 400
#define MAX_FRAME 209
Vector vertecies[] = {{100, -100}, {-100, -100}, {100, 100}, {-100, 100}};
int order[] = {0, 1, 2, 1, 3, 2};
Triangle triangles[(sizeof(order) / sizeof(order[0])) / 3];
Vector dirs[2][3] = {{{-1, 1}, {1, 0}, {1,-1}},
					 {{1, -1}, {0, 1}, {-1,1}}};

void setup(){
	for(int i = 0; i < sizeof(order) / sizeof(order[0]); i += 3){
		Vector three[3] = {vertecies[order[i]], vertecies[order[i+1]], vertecies[order[i+2]]};
		triangles[i / 3] = triangleCreate(three);

	}
}
void update(PixelGrid grid, Frame_t frame){
	gridClear(grid);
	for(int i = 0; i < sizeof(triangles) / sizeof(triangles[0]); i++){
		int div = MAX_FRAME / (sizeof(dirs[0]) / sizeof(dirs[0][0]));
		Vector arr[3] = {{cos(0.03), sin(0.03)}, {-sin(0.03), cos(0.03)}, dirs[i][frame / div]};
		Mat2x3 rotate = mat2x3Create(arr);		
		triangleTransform(triangles + i, &rotate);
	}
}
Color generate(Position pos, Dimention dim, Frame_t frame){
	Color output = colorCreate(0, 0, 0);
	for(int i = 0; i < sizeof(triangles) / sizeof(triangles[0]);i++){
		Vector trans[3] = {{1,0}, {0, 1}, {200, 200}};
		Mat2x3 translate = mat2x3Create(trans);
		Triangle translated = triangleTransformed(triangles[i], &translate); 

		srand(pos.x * pos.y);
		double dist = triangleDistToPoint(translated, pos);
		if(triangleIsPointInside(translated, pos)){
			output = colorAdd(output, colorCreate(rand() % 255, rand() % 255 , rand() % 255));
		}
		if(dist < 0)
			continue;
		if(dist <= 4){
			output = colorAdd(output, colorCreate( 255 , 255 , 255));
		}
	}
	return output;
}

int main(){
	PixelGrid grid = gridCreate(WIDTH, HEIGHT, setup, update, generate);
	gridSetMaxFrame(grid, MAX_FRAME);
	gridDrawToImage(grid, "output/out");
	gridFree(grid);
}
