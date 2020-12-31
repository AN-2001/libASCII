#include <raster/raster.h>
#include <gd.h>
#include <stdio.h>
#include <stdlib.h>

gdImagePtr image;
void error(const char* msg);
Color raster(Position pos, Dimention dim, Frame_t frame){
	int pixel = gdImageGetTrueColorPixel(image, pos.x, pos.y);
	int r = gdTrueColorGetRed(pixel);
	int g = gdTrueColorGetGreen(pixel);
	int b = gdTrueColorGetBlue(pixel);
	int a = gdTrueColorGetAlpha(pixel);

	r *= 1.5;
	g *= 1.5;
	b *= 1.5;

	if(a == 255){
		r = g = b = 0;
	}

	return colorCreate(r, g, b);
}


void update(PixelGrid grid, Frame_t frame){

}
void setup(){

}
void error(const char* msg){
	fprintf(stderr, "-=-=-=ERROR=-=-=-\n%s\n", msg);
}
int main(int argc, const char* argv[]){
	if(argc < 4){
		error("please use this syntax:\nimageToASCII [inputFile] [outputFile] [fileType]");
		return 1;
	}
	const char* inPath = argv[1];
	const char* outPath = argv[2];
	const char* imageType = argv[3];
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
    PixelGrid grid = gridCreate(image->sx, image->sy,setup, update ,raster);
	if(grid == NULL){
		fprintf(stderr, "GRID IS NULL");    
		return 1;
	}
	gridSetMaxFrame(grid, 1);
	gridDrawToImage(grid, outPath);
	gridFree(grid);
	gdImageDestroy(image);
	return 0;
}
