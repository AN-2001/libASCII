#include "include/grid.h"
#include "include/drawing.h"
#include "include/utill.h"
#include "include/joystick.h"
#include "include/outputProgress.h"
#include "include/input.h"
#include <gd.h>
#include <gdfontmb.h>
#include <gdfontg.h>
#include <gdfonts.h>
#include <gdfontt.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
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
static const char *statusToStr(ASCIIGridStatus status);
static gdFont *ASCIIFontToGDFont(ASCIIFont font);
static struct asciiGrid{
	Dimension dim;
	Frame currentFrame, maxFrame;
	Pixels pixels;
	Dimension fontDim;
	Generator generator;
	Setup setup;
	Update update;
	Cleanup cleanup;
	gdFontPtr font;
	gdImagePtr img;
	ASCIICharSet charSet;
	Delay frameDelay;
	ASCIIJoyStick joystick;
	int shouldClear;
	Color clearColor;
}* InnerGrid = NULL;


ASCIIGridStatus __gridOpen__(unsigned width, unsigned height, ASCIIFont font, Setup setup, Update update, Cleanup cleanup, Generator gen){
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
	InnerGrid->cleanup = cleanup;
	InnerGrid->charSet = ASCII_SET_BIG;
	InnerGrid->frameDelay = 0;
	InnerGrid->shouldClear = 1;

	ASCIIGridStatus status;
	if((status = setupFont(font)) != ASCII_GRID_SUCCESS){
		gridClose();
		error(status);
	}

	InnerGrid->pixels = malloc(sizeof(Color) * (InnerGrid->dim.x) * (InnerGrid->dim.y));
	if(!InnerGrid->pixels){
		gridClose();
		error(ASCII_GRID_OUT_OF_MEMORY);
	}

	memset(InnerGrid->pixels, 0, sizeof(Color) * (width * height));

	write(fileno(stdout), ASCII_RESET_TERM ASCII_TURN_CURSOR_OFF, ASCII_RESET_TERM_SIZE + ASCII_TURN_CURSOR_OFF_SIZE);
	
	InnerGrid->joystick = ASCIIJoyStickCreate();

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
	if(InnerGrid->cleanup)
		InnerGrid->cleanup();

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

	for(int j = 0; j < InnerGrid->dim.y; j ++){
		for(int i = 0; i < InnerGrid->dim.x ; i ++){
			Position pos = {i, j};
			int index = INDEX(pos.x, pos.y, InnerGrid->dim.x);
			if(index >= (InnerGrid->dim.x) * (InnerGrid->dim.y))
				error(ASCII_GRID_OUT_OF_BOUNDS);
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

	for(int j = 0; j < InnerGrid->dim.y; j ++){
		for(int i = 0; i < InnerGrid->dim.x ; i ++){
			int index = INDEX(i, j, InnerGrid->dim.x);
			if(index >= (InnerGrid->dim.x) * (InnerGrid->dim.y))
				error(ASCII_GRID_OUT_OF_BOUNDS);

			Color col = InnerGrid->pixels[index];
			char colorChar = colorToChar(col);
			Color bg = InnerGrid->clearColor;
			gdImageFilledRectangle(InnerGrid->img, i * InnerGrid->fontDim.x, j * InnerGrid->fontDim.y, (i+1) * InnerGrid->fontDim.x, (j+1) * InnerGrid->fontDim.y, gdTrueColor(bg.r, bg.g, bg.b));
			gdImageChar(InnerGrid->img, InnerGrid->font, i * InnerGrid->fontDim.x , j * InnerGrid->fontDim.y, colorChar, gdTrueColor(col.r, col.g, col.b));
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

	size_t size = (InnerGrid->dim.x + 1) * (InnerGrid->dim.y) + 1;
	char *output = malloc(20*size);
	if(!output)
		error(ASCII_GRID_OUT_OF_MEMORY);

	char *start = output;
	Color lastColor = colorCreate(0, 0, 0);



	for(int j = 0; j < InnerGrid->dim.y; j ++){
		for(int i = 0; i < InnerGrid->dim.x ; i ++){
			int index = INDEX(i, j, InnerGrid->dim.x);
			if(index >= (InnerGrid->dim.x) * (InnerGrid->dim.y))
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

	if(InnerGrid->joystick)
		ASCIIJoyStickStart(InnerGrid->joystick);
	do{

		if(InnerGrid->shouldClear)
			clear();

		if(InnerGrid->update)
			if(InnerGrid->update(InnerGrid->currentFrame))
				break;

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

	if(InnerGrid->cleanup)
		InnerGrid->cleanup();

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


	for(int j = 0; j < InnerGrid->dim.y; j ++){
		for(int i = 0; i < InnerGrid->dim.x ; i ++){
			int index = INDEX(i, j, InnerGrid->dim.x);
			if(index >= (InnerGrid->dim.x) * (InnerGrid->dim.y))
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

	if(InnerGrid->joystick){
		ASCIIJoyStickStop(InnerGrid->joystick);
		ASCIIJoyStickDestroy(InnerGrid->joystick);
		InnerGrid->joystick = NULL;
	}

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
static gdFont *ASCIIFontToGDFont(ASCIIFont font){
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
		default:
			return NULL;
	}
	return NULL;
}
static ASCIIGridStatus setupFont(ASCIIFont font){
	InnerGrid->font = ASCIIFontToGDFont(font);
	if(!InnerGrid->font){
		InnerGrid->fontDim = vectorCreate(1, 1);
		return ASCII_GRID_SUCCESS;
	}
	InnerGrid->fontDim = vectorCreate(InnerGrid->font->w, InnerGrid->font->h);
	InnerGrid->img = gdImageCreateTrueColor(InnerGrid->dim.x * InnerGrid->fontDim.x, InnerGrid->dim.y * InnerGrid->fontDim.y);
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



//  DRAW FUNCTIONS

ASCIIGridStatus gridDrawPoint(Position pos, Color col){
	if(!InnerGrid)
		error(ASCII_GRID_NOT_OPEN);
	if(pos.x < 0 || pos.x >= InnerGrid->dim.x)
		return ASCII_GRID_SUCCESS;

	if(pos.y < 0 || pos.y >= InnerGrid->dim.y)
		return ASCII_GRID_SUCCESS;
	int index = INDEX(pos.x, pos.y, InnerGrid->dim.x);
	//this is just a redundency to make sure it doesn't break!
	if(index < 0 || index > InnerGrid->dim.x * InnerGrid->dim.y)
		return ASCII_GRID_SUCCESS;
	InnerGrid->pixels[index] = col; 
	return ASCII_GRID_SUCCESS;
}

ASCIIGridStatus gridDrawEllipse(Position centre, Dimension dim, Color col){
	if(!InnerGrid)
		error(ASCII_GRID_NOT_OPEN);
	Position p1, p2, p3, p4;
	for(double ang = 0; ang <= M_PI/2; ang += 0.01f){
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

	if(end.x == start.x){
		for(double k = minf(start.y, end.y); k < maxf(start.y, end.y); k ++)
			gridDrawPoint(vectorCreate(start.x, k), col);
		return ASCII_GRID_SUCCESS;
	}

	if(end.y == start.y){
		for(double k = minf(start.x, end.x); k < maxf(start.x, end.x); k ++)
			gridDrawPoint(vectorCreate(k, start.y), col);
		return ASCII_GRID_SUCCESS;
	}

	int dirX = signf(end.x - start.x),
		dirY = signf(end.y - start.y);

	Vector stepX ,stepY;
	int signX, signY;
	double gradX, gradY;


	gradX = (end.y - start.y) / (end.x - start.x);
	gradY = (end.x - start.x) / (end.y - start.y);

	stepX = vectorCreate(dirX, gradX  * dirX);
	stepY = vectorCreate(gradY * dirY, dirY);

	while(1){


		gradX = (end.y - start.y) / (end.x - start.x);
		gradY = (end.x - start.x) / (end.y - start.y);
		if(vectorMag(stepX) < vectorMag(stepY)){
			start = vectorAdd(start, stepX); 
			stepY = vectorSub(stepY, stepX);
			stepX = vectorCreate(dirX, gradX * dirX);
		}else{
			start = vectorAdd(start, stepY); 
			stepX = vectorSub(stepX, stepY);
			stepY = vectorCreate(gradY * dirY, dirY);
		}

		signX = signf(end.x - start.x);
		signY = signf(end.y - start.y);
		if(signX != dirX || signY != dirY)
			break;

		gridDrawPoint(start, col);
	}
	return ASCII_GRID_SUCCESS;

}


//triangle stuff

Triangle gridCreateTriangle(Position p1, Position p2, Position p3){
	Triangle ret;
	ret.vertices[0] = p1; ret.vertices[1] = p2; ret.vertices[2] = p3;
	ret.flags.val = 0x07;
	return ret;
}

void gridTriangleSetFlags(Triangle *triangle, unsigned char flags){
	triangle->flags.val = flags;
}
static int vectorSortByY(const void *v1, const void *v2){
	return ((Vector*)v2)->y - ((Vector*)v1)->y ;
}
ASCIIGridStatus gridDrawTriangle(Triangle triangle, Color col){
	Vector cpy[3];	
	memcpy(cpy, triangle.vertices, sizeof(cpy));
	qsort(cpy, 3, sizeof(Vector), vectorSortByY);
		
	if(triangle.flags.e1)
		gridDrawLine(triangle.vertices[0], triangle.vertices[1], col);

	if(triangle.flags.e2)
		gridDrawLine(triangle.vertices[1], triangle.vertices[2], col);

	if(triangle.flags.e3)
		gridDrawLine(triangle.vertices[2], triangle.vertices[0], col);
	return ASCII_GRID_SUCCESS;
}



//input stuff
ASCIIBtnState gridGetButton(ASCIIBtn button){
	if(!InnerGrid)
		return 0;
	if(!InnerGrid->joystick)
		return 0;
	return ASCIIJoyStickGetButton(InnerGrid->joystick, button);
}

ASCIIAxisState gridGetAxis(ASCIIAxisState axis){
	if(!InnerGrid)
		return 0;
	if(!InnerGrid->joystick)
		return 0;
	return ASCIIJoyStickGetAxis(InnerGrid->joystick, axis);
}

Dimension gridGetDim(){
	if(!InnerGrid)
		return vectorCreate(0, 0);
	return InnerGrid->dim;
}
Dimension gridGetFontDim(ASCIIFont font){
	gdFont *gdfont = ASCIIFontToGDFont(font);
	return vectorCreate(gdfont->w, gdfont->h);
}

