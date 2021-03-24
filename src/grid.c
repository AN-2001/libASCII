#include <gd.h>
#include <gdfontmb.h>
#include <gdfontg.h>
#include <gdfonts.h>
#include <gdfontt.h>
#include "grid.h"
#include <stdlib.h>
#include <stdio.h>
#include "utill.h"
#include "outputProgress.h"
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "drawing.h"
#include <math.h>

#define INDEX(x, y, w) (int)((int)(x) + (int)(y) * (w))

//TODO: add perlin noise
//TODO: refactor how iteration is done

extern inline char colorToChar(Color col);
static ASCIIGridStatus setupFont(ASCIIFont font);
static void handleInterrupt(int sig);
static void setupSignals();
static ASCIIGridStatus clear();
static ASCIIGridStatus clearImage();
static ASCIIGridStatus clearPixels();
inline static ASCIIGridStatus fixDim();
static const char *statusToStr(ASCIIGridStatus status);

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
	Delay frameDelay;
	int shouldClear;
	Color clearColor;
}* InnerGrid = NULL;


ASCIIGridStatus gridOpen(unsigned width, unsigned height, ASCIIFont font, Setup setup, Update update, Generator gen){
	if(width == 0 || height == 0) error(ASCII_GRID_BAD_ARGUMENT); 
	InnerGrid = malloc(sizeof(*InnerGrid));	
	if(!InnerGrid)
		error(ASCII_GRID_OUT_OF_MEMORY);
	if(!update && !gen)
		error(ASCII_GRID_CANT_GENERATE);

	InnerGrid->dim = vectorCreate(width, height);
	InnerGrid->currentFrame = 0;
	InnerGrid->maxFrame = 1;
	InnerGrid->generator = gen;
	InnerGrid->setup = setup;
	InnerGrid->update = update;
	InnerGrid->charSet = ASCII_SET_BIG;
	InnerGrid->frameDelay = 0;



	ASCIIGridStatus status;
	if((status = setupFont(font)) != ASCII_GRID_SUCCESS){
		gridClose();
		error(status);
	}

	Dimention scaledDown = DimentionScale(InnerGrid->dim, InnerGrid->res);
	InnerGrid->pixels = malloc(sizeof(Color) * (scaledDown.x) * (scaledDown.y));
	if(!InnerGrid->pixels){
		gridClose();
		error(ASCII_GRID_OUT_OF_MEMORY);
	}

	write(fileno(stdout), ASCII_RESET_TERM ASCII_TURN_CURSOR_OFF, ASCII_RESET_TERM_SIZE + ASCII_TURN_CURSOR_OFF_SIZE);

	setupSignals();
	return ASCII_GRID_SUCCESS;
}
//TODO:maybe use a lookup table?
static const char *statusToStr(ASCIIGridStatus status){
	switch(status){
		case ASCII_GRID_SUCCESS:
			return "well, this shouldn't happen";
		case ASCII_GRID_ERROR:
			return "unexpected error";
		case ASCII_GRID_NOT_OPEN:
			return "grid is not open";
		case ASCII_GRID_OUT_OF_MEMORY:
			return "out of memory";
		case ASCII_GRID_BAD_ARGUMENT:
			return "bad argument";
		case ASCII_GRID_OUT_OF_BOUNDS:
			return "index out of bounds";
		case ASCII_GRID_BAD_MAX_FRAME:
			return "trying to use negative frame count in PNG mode";
		case ASCII_GRID_TOO_SMALL:
			return "grid is too small";
		case ASCII_GRID_COULDNT_SAVE_IMAGE:
			return "couldn't save image";
		case ASCII_GRID_CANT_GENERATE:
			return "grid has no way to generate images";

		default:
			return "unknown error";
	}
}
static void setupSignals(){
	struct sigaction new_action, old_action;
	new_action.sa_handler = handleInterrupt;
	sigemptyset(&new_action.sa_mask);
	new_action.sa_flags = 0;
	sigaction(SIGINT, NULL, &old_action);
	if(old_action.sa_handler != SIG_IGN)
		sigaction(SIGINT, &new_action, NULL);
}
static void handleInterrupt(int sig){
	gridClose();	
	exit(0);
}
ASCIIGridStatus _error(ASCIIGridStatus status, const char *func, const char *file, int line){
#ifdef ASCII_DEBUG
	if(status == ASCII_GRID_SUCCESS)
		return status;
	fprintf(stderr, ASCII_TEXT_COLOR(255, 0, 0) "error: " ASCII_TEXT_COLOR(255,255,0)"%s" ASCII_TEXT_COLOR(255,0,0)" at line %d in %s %s\n", statusToStr(status), line, func, file);
	return status;
#else	
	return status;
#endif
}

static Color generateValue(Position pos){
	if(InnerGrid == NULL)
		return colorCreate(0, 0, 0);	
	if(InnerGrid->generator == NULL)
		return colorCreate(0, 0, 0);
	Color c = InnerGrid->generator(pos);
	c.r = clamp(c.r, 0, 255);
	c.g = clamp(c.g, 0, 255);
	c.b = clamp(c.b, 0, 255);

	return c;
}

static ASCIIGridStatus generatePixels(){
	if(!InnerGrid)
		error(ASCII_GRID_NOT_OPEN);

	if( !InnerGrid->pixels)
		error(ASCII_GRID_ERROR);


	Dimention scaledDown = DimentionScale(InnerGrid->dim, InnerGrid->res); 

	for(int j = 0; j < scaledDown.y; j ++){
		for(int i = 0; i < scaledDown.x ; i ++){
			int index = INDEX(i, j, scaledDown.x);
			if(index >= (scaledDown.x) * (scaledDown.y))
				error(ASCII_GRID_OUT_OF_BOUNDS);
			
			Position pos = {i, j};
			pos.x *= InnerGrid->res.x;
			pos.y *= InnerGrid->res.y;
			Color color = generateValue(pos); 
			InnerGrid->pixels[index] = color;
		}
	}
	return ASCII_GRID_SUCCESS;
}

static ASCIIGridStatus generateImage(){
	if(InnerGrid == NULL)
		error(ASCII_GRID_NOT_OPEN);

	if(!InnerGrid->font || !InnerGrid->pixels)
		error(ASCII_GRID_ERROR);

	Dimention scaledDown = DimentionScale(InnerGrid->dim, InnerGrid->res);

	for(int j = 0; j < scaledDown.y; j ++){
		for(int i = 0; i < scaledDown.x ; i ++){
			int index = INDEX(i, j, scaledDown.x);
			if(index >= (scaledDown.x) * (scaledDown.y))
				error(ASCII_GRID_OUT_OF_BOUNDS);

			Color col = InnerGrid->pixels[index];
			char colorChar = colorToChar(col);
			Color bg = InnerGrid->clearColor;
			gdImageFilledRectangle(InnerGrid->img, i * InnerGrid->res.x, j * InnerGrid->res.y, (i+1) * InnerGrid->res.x, (j+1) * InnerGrid->res.y, gdTrueColor((int)bg.r, (int)bg.g, (int)bg.b));
			gdImageChar(InnerGrid->img, InnerGrid->font, i * InnerGrid->res.x , j * InnerGrid->res.y, colorChar, gdTrueColor((int)col.r, (int)col.g, (int)col.b));
		}
	}
	return ASCII_GRID_SUCCESS;
}
static ASCIIGridStatus drawToImage(const char* filepath){
	if(!filepath)
		error(ASCII_GRID_BAD_ARGUMENT);
	
	char path[64];
	if(InnerGrid->maxFrame > 1){
		sprintf(path, "%s%d.png", filepath, InnerGrid->currentFrame);
	}else{
		sprintf(path, "%s.png", filepath);
	}

	generateImage();

	if(InnerGrid->img == NULL)
		error(ASCII_GRID_ERROR);

	if(gdImageFile(InnerGrid->img, path) == GD_FALSE)
		error(ASCII_GRID_COULDNT_SAVE_IMAGE);

	return ASCII_GRID_SUCCESS;
}

static ASCIIGridStatus printToTerm(){
	if(InnerGrid == NULL)
		error(ASCII_GRID_BAD_ARGUMENT);
	Dimention scaledDown = DimentionScale(InnerGrid->dim, InnerGrid->res); 
	size_t size = (scaledDown.x + 1) * (scaledDown.y + 1) + 1;
	char *output = malloc(20*size);
	if(!output)
		error(ASCII_GRID_OUT_OF_MEMORY);

	char *start = output;
	Color lastColor = colorCreate(0, 0, 0);



	for(int j = 0; j < scaledDown.y; j ++){
		for(int i = 0; i < scaledDown.x ; i ++){
			int index = INDEX(i, j, scaledDown.x);
			if(index >= (scaledDown.x) * (scaledDown.y))
				error(ASCII_GRID_OUT_OF_BOUNDS);
			Color col = InnerGrid->pixels[index];
			char c = colorToChar(col);

			if(colorIsEqual(col, lastColor)){
				output += sprintf(output, "%c", c);
			}else{
				output += colorPrintChar(output, c, col);  
				lastColor = col;
			}
		}
		sprintf(output++, "\n");
	}

	write(fileno(stdout), ASCII_MOVE_CURSOR(1, 1), ASCII_RESET_CURSOR_SIZE);
	write(fileno(stdout), start, strlen(start));
	free(start);
	return ASCII_GRID_SUCCESS;
}

ASCIIGridStatus gridDraw(const char* filepath){
	if(!InnerGrid)
		error(ASCII_GRID_NOT_OPEN);

	if(InnerGrid->maxFrame < 0 && filepath)
		error(ASCII_GRID_BAD_MAX_FRAME);
		
	if(InnerGrid->setup)
		InnerGrid->setup();
	do{
		if(InnerGrid->shouldClear)
			clear();
		if(InnerGrid->update)
			InnerGrid->update(InnerGrid->currentFrame);

		if(InnerGrid->generator)
			generatePixels();

		if(!filepath){
			printToTerm();
			usleep(InnerGrid->frameDelay);
		}else{
			drawToImage(filepath);
			writeOutProgress(InnerGrid->currentFrame, InnerGrid->maxFrame);
		}
		InnerGrid->currentFrame++;


	}while(InnerGrid->currentFrame != InnerGrid->maxFrame);

	return ASCII_GRID_SUCCESS;
}

ASCIIGridStatus clearImage(){

	if(!InnerGrid)
		error(ASCII_GRID_NOT_OPEN);
	if(!InnerGrid->img)
		return ASCII_GRID_SUCCESS;
	Color bg = InnerGrid->clearColor;
	for(int x = 0; x < InnerGrid->img->sx; x++){
		for(int y = 0; y < InnerGrid->img->sy; y++){
			gdImageSetPixel(InnerGrid->img, x, y, gdTrueColorAlpha((int)bg.r, (int)bg.g, (int)bg.b, 0));
		}
	}
	return ASCII_GRID_SUCCESS;
}

ASCIIGridStatus clearPixels(){

	if(!InnerGrid)
		error(ASCII_GRID_NOT_OPEN);


	Dimention scaledDown = DimentionScale(InnerGrid->dim, InnerGrid->res); 
	for(int j = 0; j < scaledDown.y; j ++){
		for(int i = 0; i < scaledDown.x ; i ++){
			int index = INDEX(i, j, scaledDown.x);
			if(index >= (scaledDown.x) * (scaledDown.y))
				error(ASCII_GRID_OUT_OF_BOUNDS);
			InnerGrid->pixels[index] = InnerGrid->clearColor;
		}
	}
	return ASCII_GRID_SUCCESS;
}
ASCIIGridStatus clear(){
	if(!InnerGrid)
		error(ASCII_GRID_NOT_OPEN);
	ASCIIGridStatus s1 = ASCII_GRID_SUCCESS;
	ASCIIGridStatus s2 = clearPixels();

	if(InnerGrid->img){
		s1 = clearImage();
	}
	if(s1 != ASCII_GRID_SUCCESS || s2 != ASCII_GRID_SUCCESS)
		error(ASCII_GRID_ERROR);
	InnerGrid->shouldClear = 0;
	return ASCII_GRID_SUCCESS;
		
}
ASCIIGridStatus gridClear(Color bg){
	if(!InnerGrid)
		error(ASCII_GRID_NOT_OPEN);
	InnerGrid->shouldClear = 1;
	InnerGrid->clearColor = bg;
	return ASCII_GRID_SUCCESS;
}
ASCIIGridStatus gridClose(){
	if(InnerGrid == NULL)
		error(ASCII_GRID_NOT_OPEN);

	if(InnerGrid->img){
		gdImageDestroy(InnerGrid->img);
		InnerGrid->img = NULL;
	}

	free(InnerGrid->pixels);
	InnerGrid->pixels = NULL;
	free(InnerGrid);

	write(fileno(stdout), ASCII_TURN_CURSOR_ON, ASCII_TURN_CURSOR_ON_SIZE);

	return ASCII_GRID_SUCCESS;
}
ASCIIGridStatus gridSetMaxFrame( Frame max){
	if(InnerGrid == NULL)
		error(ASCII_GRID_NOT_OPEN);

	InnerGrid->maxFrame = max;
	return ASCII_GRID_SUCCESS;
}
//TODO: use a lookup table
static ASCIIGridStatus setupFont(ASCIIFont font){
	switch(font){
		case ASCII_FONT_TINY:
				InnerGrid->font = gdFontGetTiny();
			break;
		case ASCII_FONT_MEDIUM_BOLD:
				InnerGrid->font = gdFontGetMediumBold();
			break;
		case ASCII_FONT_GIANT:
				InnerGrid->font = gdFontGetGiant();
			break;
		case ASCII_FONT_SMALL:
				InnerGrid->font = gdFontGetSmall();
			break;
		case ASCII_FONT_TERM:
			InnerGrid->res = vectorCreate(7, 13);
			fixDim();
			return ASCII_GRID_SUCCESS;
		default:
			error(ASCII_GRID_BAD_ARGUMENT);
	}
	InnerGrid->res = vectorCreate(InnerGrid->font->w, InnerGrid->font->h);
	fixDim();

	InnerGrid->img = gdImageCreateTrueColor(InnerGrid->dim.x, InnerGrid->dim.y);
	if(!InnerGrid->img)
		error(ASCII_GRID_OUT_OF_MEMORY);

	return ASCII_GRID_SUCCESS;
}

ASCIIGridStatus gridSetCharset(ASCIICharSet set){
	if(set > 1 || set < 0)
		error(ASCII_GRID_BAD_ARGUMENT);

	InnerGrid->charSet = set;
	return ASCII_GRID_SUCCESS;
}

ASCIIGridStatus gridSetFrameDelay(Delay del){
	if(!InnerGrid)
		error(ASCII_GRID_NOT_OPEN);

	if((int)del < 0)
		error(ASCII_GRID_BAD_ARGUMENT);

	InnerGrid->frameDelay = del;
	return ASCII_GRID_SUCCESS;
}


inline ASCIIGridStatus fixDim(){
	Dimention scaledDown = DimentionScale(InnerGrid->dim, InnerGrid->res);
	scaledDown.x = (int)(scaledDown.x);
	scaledDown.y = (int)(scaledDown.y);
	if(scaledDown.x == 0 || scaledDown.y == 0)
		error(ASCII_GRID_TOO_SMALL);

	InnerGrid->dim.x = scaledDown.x * InnerGrid->res.x;	
	InnerGrid->dim.y = scaledDown.y * InnerGrid->res.y;	
	return ASCII_GRID_SUCCESS;
}

//  DRAW FUNCTIONS

ASCIIGridStatus gridDrawPoint(Position pos, Color col){
	if(!InnerGrid)
		error(ASCII_GRID_NOT_OPEN);
	pos = DimentionScale(pos, InnerGrid->res); 
	Dimention scaledDown = DimentionScale(InnerGrid->dim, InnerGrid->res); 
	if(pos.x < 0 || pos.x >= scaledDown.x)
		return ASCII_GRID_SUCCESS;

	if(pos.y < 0 || pos.y >= scaledDown.y)
		return ASCII_GRID_SUCCESS;

	InnerGrid->pixels[INDEX(pos.x, pos.y, scaledDown.x)] = col; 
	return ASCII_GRID_SUCCESS;
}

ASCIIGridStatus gridDrawEllipse(Position centre, Dimention dim, Color col){
	if(!InnerGrid)
		error(ASCII_GRID_NOT_OPEN);
	Position p1, p2, p3, p4;
	for(double ang = 0; ang <= M_PI/2; ang += 0.01){
		//use symmetry to reduce sin, cos calls!
		p1 = vectorCreate(dim.x * cos(ang), dim.y * sin(ang));
		p2 = vectorCreate(p1.x, -p1.y);
		p3 = vectorCreate(-p1.x, p1.y);
		p4 = vectorCreate(-p1.x, -p1.y);
		//offset them 	
		p1 = vectorAdd(centre, p1); p2 = vectorAdd(centre, p2);
		p3 = vectorAdd(centre, p3); p4 = vectorAdd(centre, p4);
	
		//fill in a straight line 
		gridDrawLine(p1, p2, col);
		gridDrawLine(p3, p4, col);
	}

	return ASCII_GRID_SUCCESS;
}

ASCIIGridStatus gridDrawLine(Position start, Position end, Color col){

	double offX = ((int)start.x/(int)InnerGrid->res.x) * (int)InnerGrid->res.x,
		   offY = ((int)start.y/(int)InnerGrid->res.y) * (int)InnerGrid->res.y;


	start = vectorCreate(offX, offY);

	offX = ((int)end.x/(int)InnerGrid->res.x) * (int)InnerGrid->res.x;
	offY = ((int)end.y/(int)InnerGrid->res.y) * (int)InnerGrid->res.y;

	end = vectorCreate(offX, offY);




	if(end.x == start.x){
		for(double k = minf(start.y, end.y); k < maxf(start.y, end.y); k += InnerGrid->res.y)
			gridDrawPoint(vectorCreate(start.x, k), col);
		return ASCII_GRID_SUCCESS;
	}

	if(end.y == start.y){
		for(double k = minf(start.x, end.x); k < maxf(start.x, end.x); k += InnerGrid->res.x)
			gridDrawPoint(vectorCreate(k, start.y), col);
		return ASCII_GRID_SUCCESS;
	}
	while(1){
		gridDrawPoint(start, col);

		double gradX = (end.y - start.y) / (end.x - start.x);
		double gradY = (end.x - start.x) / (end.y - start.y);
		double x = InnerGrid->res.x * signf(end.x - start.x);
		double y = InnerGrid->res.y * signf(end.y - start.y);
		double movX = gradX * x;
		double movY = gradY * y;
		if(fabs(movY) < fabs(movX)){
			start.x += movY;
			start.y += y;
		}else{
			start.y += movX;
			start.x += x; 
		}

		int x1 = ((int)start.x / (int)InnerGrid->res.x) * (int)InnerGrid->res.x,
		    x2 = ((int)end.x / (int)InnerGrid->res.x) * (int)InnerGrid->res.x,
			y1 = ((int)start.y / (int)InnerGrid->res.y) * (int)InnerGrid->res.y,
		 	y2 = ((int)end.y / (int)InnerGrid->res.y) * (int)InnerGrid->res.y;

		if(x1 == x2 && y1 == y2)
			break;
	}
	return ASCII_GRID_SUCCESS;

}
