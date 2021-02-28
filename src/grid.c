#ifdef ASCII_USE_GD
#include <gd.h>
#include <gdfontmb.h>
#include <gdfontg.h>
#include <gdfonts.h>
#include <gdfontt.h>
#endif
#include "grid.h"
#include <stdlib.h>
#include <stdio.h>
#include "utill.h"
#include "outputProgress.h"
#include <unistd.h>
#include <string.h>
#include <signal.h>

static struct asciiGrid{
	Dimention dim;
	Frame currentFrame, maxFrame;
	Pixels pixels;
	Dimention res;
	Generator generator;
	Update update;
	Setup setup;
#ifdef ASCII_USE_GD
	gdFontPtr font;
	gdImagePtr img;
#endif
	ASCIICharSet charSet;
	Delay frameDelay;
}* InnerGrid = NULL;
extern inline char colorToChar(Color col);

#ifdef ASCII_USE_GD
static ASCIIGridStatus gridSetFont(ASCIIFont font);
#endif

#define error(type) return _error(type, __func__,__FILE__, __LINE__)

static ASCIIGridStatus _error(ASCIIGridStatus status, const char *func, const char *file, int line);
static void handleInterrupt(int sig);
static void setupSignals();
static const char *statusToStr(ASCIIGridStatus status);

ASCIIGridStatus gridOpen(unsigned width, unsigned height
#ifdef ASCII_USE_GD
		, ASCIIFont font
#endif
		, Setup setup, Update update, Generator gen){
	if(width == 0 || height == 0)
		error(ASCII_GRID_BAD_ARGUMENT);

	if(gen == NULL)
		error(ASCII_GRID_BAD_ARGUMENT);

	InnerGrid = malloc(sizeof(*InnerGrid));	
	if(!InnerGrid)
		error(ASCII_GRID_OUT_OF_MEMORY);

	InnerGrid->dim = vectorCreate(width, height);
	InnerGrid->currentFrame = 0;
	InnerGrid->maxFrame = 1;
	InnerGrid->generator = gen;
	InnerGrid->setup = setup;
	InnerGrid->update = update;
	InnerGrid->charSet = ASCII_SET_BIG;
	InnerGrid->frameDelay = 0;
	InnerGrid->res = vectorCreate(7, 13);

#ifdef ASCII_USE_GD
	if(font != ASCII_FONT_TERM){
		ASCIIGridStatus status;
		if((status = gridSetFont(font)) != ASCII_GRID_SUCCESS){
			gridClose();
			error(status);
		}
	}
#endif

	Dimention scaledDown = DimentionScale(InnerGrid->dim, InnerGrid->res);
	InnerGrid->pixels = malloc(sizeof(Color) * (scaledDown.x + 1) * (scaledDown.y + 1));
	if(!InnerGrid->pixels)
		error(ASCII_GRID_OUT_OF_MEMORY);

	write(fileno(stdout), ASCII_RESET_TERM ASCII_TURN_CURSOR_OFF, ASCII_RESET_TERM_SIZE + ASCII_TURN_CURSOR_OFF_SIZE);

	setupSignals();
	return ASCII_GRID_SUCCESS;
}

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
static ASCIIGridStatus _error(ASCIIGridStatus status, const char *func, const char *file, int line){
#ifdef ASCII_DEBUG
	if(status == ASCII_GRID_SUCCESS)
		return status;
	printf(ASCII_TEXT_COLOR(255, 0, 0) "error: " ASCII_TEXT_COLOR(255,255,0)"%s" ASCII_TEXT_COLOR(255,0,0)" at line %d in %s %s\n", statusToStr(status), line, func, file);
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
	Color val = InnerGrid->generator(pos, InnerGrid->dim, InnerGrid->currentFrame);
	Color maxVal = colorCreate(255, 255, 255);
	double r = val.r / maxVal.r;
	double g = val.g / maxVal.g;
	double b = val.b / maxVal.b;
	r = clamp(r, 0.0f, 1.0f); 
	g = clamp(g, 0.0f, 1.0f); 
	b = clamp(b, 0.0f, 1.0f); 

	return colorCreate(r, g, b);
}

static ASCIIGridStatus generatePixels(){
	if(!InnerGrid)
		error(ASCII_GRID_NOT_OPEN);

	if( !InnerGrid->pixels)
		error(ASCII_GRID_ERROR);

	Dimention scaledDown = DimentionScale(InnerGrid->dim, InnerGrid->res); 
	for(int j = 0; j < scaledDown.y; j ++){
		for(int i = 0; i < scaledDown.x; i ++){
			Position pos = {i, j};
			int index = posToIndex(pos, scaledDown.x);
			pos.x *= InnerGrid->res.x;
			pos.y *= InnerGrid->res.y;
			Color color = generateValue(pos); 
			if(index > InnerGrid->dim.x * InnerGrid->dim.y)
				return ASCII_GRID_OUT_OF_BOUNDS;
			InnerGrid->pixels[index] = color;
		}
	}
	return ASCII_GRID_SUCCESS;
}

#ifdef ASCII_USE_GD
static ASCIIGridStatus generateImage(){
	if(InnerGrid == NULL)
		error(ASCII_GRID_NOT_OPEN);

	if(!InnerGrid->font || !InnerGrid->pixels)
		error(ASCII_GRID_ERROR);

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

	gdImageFile(InnerGrid->img, path); 

	return ASCII_GRID_SUCCESS;
}
#endif //ASCII_USE_GD

static ASCIIGridStatus printToTerm(){
	if(InnerGrid == NULL)
		error(ASCII_GRID_BAD_ARGUMENT);
	Dimention scaledDown = DimentionScale(InnerGrid->dim, InnerGrid->res); 
	size_t size = (scaledDown.x + 1) * (scaledDown.y + 1) + 1;
	char *output = malloc(20*size);
	if(!output)
		error(ASCII_GRID_OUT_OF_MEMORY);

	char *start = output;

	for(int j = 0; j < scaledDown.y; j++){
		for(int i = 0; i < scaledDown.x; i++){
			Position pos = {i, j};
			int index = posToIndex(pos, scaledDown.x);
			Color col = InnerGrid->pixels[index];
			char colorChar = colorToChar(col);
			char str[2] = {colorChar, '\0'};
			output += colorPrint(output, str, col);  
		}
		sprintf(output++, "\n");
	}

	write(fileno(stdout), ASCII_MOVE_CURSOR(1, 1), ASCII_RESET_CURSOR_SIZE);
	write(fileno(stdout), start, strlen(start));
	free(start);
	return ASCII_GRID_SUCCESS;
}

ASCIIGridStatus gridDraw(
#ifdef ASCII_USE_GD
		const char* filepath
#else
		void
#endif
		){
	if(!InnerGrid)
		error(ASCII_GRID_NOT_OPEN);
	gridClear();
	if(InnerGrid->setup)
		InnerGrid->setup();

	do{
		if(InnerGrid->update)
			InnerGrid->update(InnerGrid->currentFrame);

		generatePixels();

#ifdef ASCII_USE_GD
		if(!filepath){
			printToTerm();
			usleep(InnerGrid->frameDelay);
		}
		else{
			drawToImage(filepath);
			writeOutProgress(InnerGrid->currentFrame, InnerGrid->maxFrame);
		}
#else
		printToTerm();
		usleep(InnerGrid->frameDelay);
#endif //ASCII_USE_GD
		InnerGrid->currentFrame++;


	}while(InnerGrid->currentFrame != InnerGrid->maxFrame);

	return ASCII_GRID_SUCCESS;
}

ASCIIGridStatus gridClear(){
	if(!InnerGrid)
		error(ASCII_GRID_NOT_OPEN);
#ifdef ASCII_USE_GD
	if(!InnerGrid->img)
		error(ASCII_GRID_ERROR);

	for(int x = 0; x < InnerGrid->img->sx; x++){
		for(int y = 0; y < InnerGrid->img->sy; y++){
			gdImageSetPixel(InnerGrid->img, x, y, gdTrueColorAlpha(0, 0, 0, 0));
		}
	}
#endif
	return ASCII_GRID_SUCCESS;
}
ASCIIGridStatus gridClose(){
	if(InnerGrid == NULL)
		error(ASCII_GRID_NOT_OPEN);

#ifdef ASCII_USE_GD
	gdImageDestroy(InnerGrid->img);
#endif
	free(InnerGrid->pixels);
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
#ifdef ASCII_USE_GD
static ASCIIGridStatus gridSetFont(ASCIIFont font){
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
		default:
			error(ASCII_GRID_BAD_ARGUMENT);
	}
	InnerGrid->res = vectorCreate(InnerGrid->font->w, InnerGrid->font->h);

	InnerGrid->img = gdImageCreateTrueColor(InnerGrid->dim.x, InnerGrid->dim.y);
	if(!InnerGrid->img)
		error(ASCII_GRID_OUT_OF_MEMORY);

	return ASCII_GRID_SUCCESS;
}
#endif //ASCII_USE_GD

ASCIIGridStatus gridSetCharset(ASCIICharSet set){
	if(set > 1 || set < 0)
		error(ASCII_GRID_BAD_ARGUMENT);

	InnerGrid->charSet = set;
	return ASCII_GRID_SUCCESS;
}

ASCIIGridStatus gridSetFrameDelay(Delay del){
	if(InnerGrid == NULL)
		error(ASCII_GRID_NOT_OPEN);

	InnerGrid->frameDelay = del;
	return ASCII_GRID_SUCCESS;
}
