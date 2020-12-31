#include <gd.h>
#include "grid.h"
#include <gdfontt.h>
#define CLEAR_SCREEN "\033c"
#define MAX_CHARS 60
#define FULL '='
#define EMPTY '-'

struct pixelGrid{
	Dimention dim;
	Frame_t currentFrame, maxFrame;
	Pixels pixels;
	Generator generator;
	Update update;
	Setup setup;
	gdFontPtr font;
	gdImagePtr img;
};

static Color generateValue(const PixelGrid grid, Position pos, Frame_t frame);
static void generatePixels(PixelGrid grid);
static void generateImage(const PixelGrid grid);
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
	char* output = malloc(MAX_CHARS + 5); 
	char* cpy = output;
	for(int i = 0; i < MAX_CHARS;i++){
		char c = (i < chars ? FULL : EMPTY);
		sprintf(output, "%c", c); 
		output++;
	}
	sprintf(output, "| %d%%\n", (int)(precentage * 100));
	return cpy;
}

PixelGrid gridCreate(unsigned width, unsigned height, Setup setup, Update update, Generator gen){
	if(width == 0 || height == 0)
		return NULL;
	if(gen == NULL)
		return NULL;
	//allocate the grid
	PixelGrid grid = malloc(sizeof(*grid));	
	if(!grid)
		return NULL;
	//allocate the pixels	
	grid->dim = (Dimention){width, height};
	grid->currentFrame = 0;
	grid->maxFrame = 1;
	grid->generator = gen;
	grid->setup = setup;
	grid->update = update;
	grid->font = gdFontGetTiny();
	if(grid->font == NULL)
		return NULL;
	Dimention fontDim = (Dimention){grid->font->w, grid->font->h};
	Dimention scaledDown = DimentionScale(grid->dim, fontDim);
	grid->pixels = malloc(sizeof(Color) * (scaledDown.x + 1) * (scaledDown.y + 1));
	grid->img = gdImageCreateTrueColor(grid->dim.x, grid->dim.y);
	if(!grid->pixels || !grid->img){
		gridFree(grid);
		return NULL;
	}
	return grid;
}

static void generatePixels(PixelGrid grid){
	if(grid->pixels == NULL)
		return;
	Dimention fontDim = (Dimention){grid->font->w, grid->font->h};
	Dimention scaledDown = DimentionScale(grid->dim, fontDim); 
	for(int j = 0; j < scaledDown.y; j ++){
		for(int i = 0; i < scaledDown.x; i ++){
			Position pos = {i, j};
			int index = posToIndex(pos, scaledDown.x);
			pos.x *= grid->font->w;
			pos.y *= grid->font->h;
			Color color = generateValue(grid, pos, grid->currentFrame); 
			grid->pixels[index] = color;
		}
	}
}
static Color generateValue(const PixelGrid grid, Position pos, Frame_t frame){
	if(grid == NULL)
		return colorCreate(0, 0, 0);	
	if(grid->generator == NULL)
		return colorCreate(0, 0, 0);
	Color val = grid->generator(pos, grid->dim, frame);
	Color maxVal = colorCreate(255, 255, 255);
	double r = val.r / maxVal.r;
	double g = val.g / maxVal.g;
	double b = val.b / maxVal.b;

	if(r > 1) r = 1;
	if(g > 1) g = 1;
	if(b > 1) b = 1;
	if(r < 0) r = 0;
	if(g < 0) g = 0;
	if(b < 0) b = 0;
	return colorCreate(r, g, b);
}
unsigned posToIndex(Position pos, unsigned width){
	return pos.x + pos.y * width;
}
static void generateImage(const PixelGrid grid){
	if(grid == NULL)
		return;
	if(!grid->font || !grid->pixels)
		return;
	Dimention fontDim = (Dimention){grid->font->w, grid->font->h};
	Dimention scaledDown = DimentionScale(grid->dim, fontDim);
	for(int j = 0; j < scaledDown.y; j ++){
		for(int i = 0; i < scaledDown.x; i ++){
			Position pos = {i, j};
			int index = posToIndex(pos, scaledDown.x);
			Color col = grid->pixels[index];
			char colorChar = colorToChar(col);
			gdImageChar(grid->img, grid->font, i * grid->font->w, j * grid->font->h, colorChar, gdTrueColor((int)(col.r*255), (int)(col.g*255), (int)(col.b*255)));
		}
	}
}
void gridDrawToImage(PixelGrid grid, const char* filepath){
	if(grid == NULL)
		return;
	if(grid->setup == NULL || grid->update == NULL){
		return;
	}
	if(filepath == NULL){
		return;
	}
	char path[64];
	grid->setup();
	do{
		grid->update(grid, grid->currentFrame);
		if(grid->maxFrame > 1){
			sprintf(path, "%s%d.png", filepath, grid->currentFrame);
		}else{
			sprintf(path, "%s.png", filepath);
		}
		generatePixels(grid);
		generateImage(grid);
		if(grid->img == NULL)
			return;
		writeOutProgress(path, grid->currentFrame, grid->maxFrame);
		gdImageFile(grid->img, path); 
		grid->currentFrame++;
	}while(grid->currentFrame < grid->maxFrame);
}

void gridClear(PixelGrid grid){
	if(grid == NULL || grid->img == NULL){
		return;
	}
	gdImageFilledRectangle(grid->img, 0, 0, grid->dim.x, grid->dim.y, gdTrueColor(0,0,0));
}
void gridFree(PixelGrid grid){
	gdImageDestroy(grid->img);
	free(grid->pixels);
	free(grid);
}
void gridSetMaxFrame(PixelGrid grid, Frame_t max){
	if(grid == NULL){
		return;
	}
	grid->maxFrame = max;
}

