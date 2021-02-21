#include <gd.h>
#include "grid.h"
#include <gdfontmb.h>
#include <gdfontg.h>
#include <gdfonts.h>
#include <gdfontt.h>
#include "utill.h"
#define CLEAR_SCREEN "\033c"
#define MAX_CHARS 60
#define FULL '='
#define EMPTY '-'

static struct asciiGrid{
	Dimention dim;
	Frame currentFrame, maxFrame;
	Pixels pixels;
	Dimention res;
	Generator generator;
	Update update;
	Setup setup;
	gdFontPtr font;
	gdImagePtr img;
	ASCIICharSet charSet;
}* InnerGrid;

static Color generateValue(Position pos);
static ASCIIGridStatus generatePixels();
static ASCIIGridStatus generateImage();
static char* generateBar(int current, int max);
static void writeOutProgress(const char* path, int current, int max);

static void writeOutProgress(const char* path, int current, int max){
	printf("writing out %s\n", path); 
	char* progress = generateBar(current, max);
	printf("%s%s", progress, CLEAR_SCREEN);
	free(progress);	
}

static char* generateBar(int current, int max){
	if(current > max){
		return NULL;
	}
	double precentage = (double)current / (double)max;
	int chars = precentage  * (double)MAX_CHARS;
	char* output = malloc(MAX_CHARS + 10); 
	char* cpy = output;
	for(int i = 0; i < MAX_CHARS;i++){
		char c = (i < chars ? FULL : EMPTY);
		sprintf(output, "%c", c); 
		output++;
	}
	sprintf(output, "| %d%%\n", (int)(precentage * 100));
	return cpy;
}

void printGrid(){
	if(InnerGrid == NULL)
		return;

	generatePixels();
	Dimention scaledDown = DimentionScale(InnerGrid->dim, InnerGrid->res); 
	char *str = malloc((scaledDown.x+1) * (scaledDown.y + 1) + 50);	
	if(str == NULL)
		return;
	char *ret = str;
	colorSetCharset(InnerGrid->charSet);
	for(int j = 0; j < scaledDown.y; j++){
		for(int i = 0; i < scaledDown.x; i++){
			Position pos = {i, j};
			int index = posToIndex(pos, scaledDown.x);
			Color col = InnerGrid->pixels[index];
			char colorChar = colorToChar(col);
			sprintf(str++, "%c", colorChar);
		}
		sprintf(str++, "\n");
	}
	printf("%s", ret);
	free(ret);
}

ASCIIGridStatus gridOpen(unsigned width, unsigned height, Setup setup, Update update, Generator gen){
	if(width == 0 || height == 0)
		return ASCII_GRID_BAD_ARGUMENT;
	if(gen == NULL)
		return ASCII_GRID_BAD_ARGUMENT;
	//allocate the grid
	InnerGrid = malloc(sizeof(*InnerGrid));	
	if(!InnerGrid)
		return ASCII_GRID_OUT_OF_MEMORY;
	//allocate the pixels	
	InnerGrid->dim = (Dimention){width, height};
	InnerGrid->currentFrame = 0;
	InnerGrid->maxFrame = 1;
	InnerGrid->generator = gen;
	InnerGrid->setup = setup;
	InnerGrid->update = update;
	InnerGrid->font = gdFontGetMediumBold();
	InnerGrid->charSet = ASCII_GRID_BIG_ASCII;
	InnerGrid->res = (Dimention){InnerGrid->font->w, InnerGrid->font->h};
	Dimention scaledDown = DimentionScale(InnerGrid->dim, InnerGrid->res);
	InnerGrid->pixels = malloc(sizeof(Color) * (scaledDown.x + 1) * (scaledDown.y + 1));
	InnerGrid->img = gdImageCreateTrueColor(InnerGrid->dim.x, InnerGrid->dim.y);
	if(!InnerGrid->pixels || !InnerGrid->img){
		gridClose();
		return ASCII_GRID_OUT_OF_MEMORY;
	}
	return ASCII_GRID_SUCCESS;
}

static ASCIIGridStatus generatePixels(){
	if(InnerGrid->pixels == NULL)
		return ASCII_GRID_ERROR;
	Dimention scaledDown = DimentionScale(InnerGrid->dim, InnerGrid->res); 
	for(int j = 0; j < scaledDown.y; j ++){
		for(int i = 0; i < scaledDown.x; i ++){
			Position pos = {i, j};
			int index = posToIndex(pos, scaledDown.x);
			pos.x *= InnerGrid->res.x;
			pos.y *= InnerGrid->res.y;
			Color color = generateValue(pos); 
			if(index > InnerGrid->dim.x * InnerGrid->dim.y)
				return ASCII_GRID_ERROR;
			InnerGrid->pixels[index] = color;
		}
	}
	return ASCII_GRID_SUCCESS;
}
static Color generateValue(Position pos){
	if(InnerGrid == NULL)
		return colorCreate(0, 0, 0);	
	if(InnerGrid->generator == NULL)
		return colorCreate(0, 0, 0);
	Color val = InnerGrid->generator(pos, InnerGrid->dim, InnerGrid->currentFrame);
	Color maxVal = colorCreate(255, 255, 255);
	double r = val.r / maxVal.r;
	double g = val.g / maxVal.g;
	double b = val.b / maxVal.b;
	r = clamp(r, 0.0f, 1.0f); 
	g = clamp(g, 0.0f, 1.0f); 
	b = clamp(b, 0.0f, 1.0f); 

	Color out = colorCreate(r, g, b);
	return out;
}
static ASCIIGridStatus generateImage(){
	if(InnerGrid == NULL)
		return ASCII_GRID_ERROR;
	if(!InnerGrid->font || !InnerGrid->pixels)
		return ASCII_GRID_ERROR;
	colorSetCharset(InnerGrid->charSet);
	Dimention scaledDown = DimentionScale(InnerGrid->dim, InnerGrid->res);
	for(int j = 0; j < scaledDown.y; j ++){
		for(int i = 0; i < scaledDown.x; i ++){
			Position pos = {i, j};
			int index = posToIndex(pos, scaledDown.x);
			Color col = InnerGrid->pixels[index];
			char colorChar = colorToChar(col);
			gdImageChar(InnerGrid->img, InnerGrid->font, i * InnerGrid->font->w, j * InnerGrid->font->h, colorChar, gdTrueColor((int)(col.r*255), (int)(col.g*255), (int)(col.b*255)));
		}
	}
	return ASCII_GRID_SUCCESS;
}
ASCIIGridStatus gridDrawToImage(const char* filepath){
	if(InnerGrid == NULL)
		return ASCII_GRID_ERROR;
	if(InnerGrid->setup == NULL || InnerGrid->update == NULL){
		return ASCII_GRID_ERROR;
	}
	if(filepath == NULL){
		return ASCII_GRID_BAD_ARGUMENT;
	}
	char path[64];
	gridClear();
	InnerGrid->setup();
	do{
		InnerGrid->update(InnerGrid->currentFrame);
		if(InnerGrid->maxFrame > 1){
			sprintf(path, "%s%d.png", filepath, InnerGrid->currentFrame);
		}else{
			sprintf(path, "%s.png", filepath);
		}
		generatePixels();
		generateImage();
		if(InnerGrid->img == NULL)
			return ASCII_GRID_ERROR;
		writeOutProgress(path, InnerGrid->currentFrame, InnerGrid->maxFrame);
		gdImageFile(InnerGrid->img, path); 
		InnerGrid->currentFrame++;
	}while(InnerGrid->currentFrame < InnerGrid->maxFrame);
	return ASCII_GRID_SUCCESS;
}

ASCIIGridStatus gridClear(){
	if(InnerGrid == NULL || InnerGrid->img == NULL)
		return ASCII_GRID_ERROR;

	gdImageDestroy(InnerGrid->img);
	InnerGrid->img = gdImageCreateTrueColor(InnerGrid->dim.x, InnerGrid->dim.y);
	//gdImageRectangle(InnerGrid->img, 0, 0, InnerGrid->dim.x, InnerGrid->dim.y, gdTrueColor(0, 0, 0));
	return ASCII_GRID_SUCCESS;
}
ASCIIGridStatus gridClose(){
	if(InnerGrid == NULL)
		return ASCII_GRID_ERROR;
	gdImageDestroy(InnerGrid->img);
	free(InnerGrid->pixels);
	free(InnerGrid);
	return ASCII_GRID_SUCCESS;
}
ASCIIGridStatus gridSetMaxFrame( Frame max){
	if(InnerGrid == NULL){
		return ASCII_GRID_ERROR;
	}
	InnerGrid->maxFrame = max;
	return ASCII_GRID_SUCCESS;
}
ASCIIGridStatus gridSetFont(ASCIIFont font){
	switch(font){
		case ASCII_GRID_TINY:
				InnerGrid->font = gdFontGetTiny();
			break;
		case ASCII_GRID_MEDIUM_BOLD:
				InnerGrid->font = gdFontGetMediumBold();
			break;
		case ASCII_GRID_GIANT:
				InnerGrid->font = gdFontGetGiant();
			break;
		case ASCII_GRID_SMALL:
				InnerGrid->font = gdFontGetSmall();
			break;
		default:
			return ASCII_GRID_BAD_ARGUMENT;
	}

	//TODO: fix this
	InnerGrid->res = (Dimention){InnerGrid->font->w, InnerGrid->font->h};
	Dimention scaledDown = DimentionScale(InnerGrid->dim, InnerGrid->res);
	free(InnerGrid->pixels);
	InnerGrid->pixels = malloc(sizeof(Color) * (scaledDown.x + 1) * (scaledDown.y + 1));
	if(InnerGrid->pixels == NULL)
		return ASCII_GRID_OUT_OF_MEMORY;
	gdImageDestroy(InnerGrid->img);
	InnerGrid->img = gdImageCreateTrueColor(InnerGrid->dim.x, InnerGrid->dim.y);
	return ASCII_GRID_SUCCESS;
}
ASCIIGridStatus gridSetCharset(ASCIICharSet set){
	if(set > 1 || set < 0)
		return ASCII_GRID_BAD_ARGUMENT;
	InnerGrid->charSet = set;
	return ASCII_GRID_SUCCESS;
}
