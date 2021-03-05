#include <ascii/grid.h>
#include <ascii/color.h>
#include <gd.h>
#include <stdio.h>
#include <stdlib.h>
#define BRIGHTNESS 1.5
gdImagePtr image;
Color generate(Position pos){
	int pixel = gdImageGetTrueColorPixel(image, pos.x, pos.y);
	int r = gdTrueColorGetRed(pixel);
	int g = gdTrueColorGetGreen(pixel);
	int b = gdTrueColorGetBlue(pixel);

	return colorCreate(r * BRIGHTNESS, g * BRIGHTNESS, b * BRIGHTNESS);
}



int main(int argc, const char* argv[]){

	const char* inPath = argv[1];
	const char* imageType = argv[2];
	//const char* outPath = argv[3];
	int type = atoi(imageType);
	FILE* img = fopen(inPath, "rb");
	switch(type){
		case 1:
			image = gdImageCreateFromPng(img);
			break;
		case 2:
			image = gdImageCreateFromJpeg(img);
			break;
	}
	fclose(img);
    if(gridOpen(image->sx, image->sy, ASCII_FONT_TINY, NULL,  NULL , generate) != ASCII_GRID_SUCCESS){
		fprintf(stderr, "COULD'T OPEN GRID");    
		return 1;
	}

	gridDraw(NULL);

	gridClose();
	gdImageDestroy(image);
	return 0;
}
