#include <ascii/grid.h>
#include <ascii/color.h>
#include <gd.h>
#include <stdio.h>
#include <stdlib.h>

gdImagePtr image;
void error(const char* msg);
Color generate(Position pos, Dimention dim, Frame_t frame){
	int pixel = gdImageGetTrueColorPixel(image, pos.x, pos.y);
	int r = gdTrueColorGetRed(pixel);
	int g = gdTrueColorGetGreen(pixel);
	int b = gdTrueColorGetBlue(pixel);
	int a = gdTrueColorGetAlpha(pixel);
	r *= 1.5;
	g *= 1.5;
	b *= 1.5;
	Color out = colorCreate(r, g, b);
	out.a = a;
	return out;
}


void update(Frame_t frame){

}
void setup(){

}
void error(const char* msg){
	fprintf(stderr, "-=-=-=ERROR=-=-=-\n%s\n", msg);
}

int main(int argc, const char* argv[]){
	if(argc < 4){
		error("please use this syntax:\nimageToASCII [inputFile] [outputFile] [fileType {1 for png} {2 for jpeg}]");
		return 1;
	}
	const char* inPath = argv[1];
	const char* imageType = argv[2];
	const char* outPath = argv[3];
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
    if(gridOpen(image->sx, image->sy,setup, update ,raster) != ASCII_GRID_SUCCESS){
		fprintf(stderr, "COULD'T OPEN GRID");    
		return 1;
	}
	gridSetMaxFrame(1);
	gridSetCharset(ASCII_GRID_SMALL_ASCII);
	gridSetFont(ASCII_GRID_SMALL);
	gridDrawToImage(outPath);
	gridClose();
	gdImageDestroy(image);
	return 0;
}
