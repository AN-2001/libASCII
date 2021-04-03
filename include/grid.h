#ifndef ASCII_GRID_H
#define ASCII_GRID_H
#include "vector.h"
#include "color.h"
#include "enums.h"
		
typedef struct asciiGrid *ASCIIGrid;
typedef int Frame;
typedef Color* Pixels;
typedef Color (*Generator)(Position pos);
typedef int (*Update)(Frame frame);
typedef void (*Cleanup)();
typedef void (*Setup)(void);
typedef unsigned Delay;
//TODO: add macro to make making a grid at a certain resolution easier

/**
 * opens the grid, always call this before doing anything
 * it initializes the library!
 * */
ASCIIGridStatus gridOpen(unsigned width, unsigned height, ASCIIFont font, Setup setup, Update update, Cleanup cleanup, Generator gen);
#define gridOpenImg(width, height, setup, update, cleanup, generate) gridOpen(width, height, ASCII_FONT_MEDIUM_BOLD, setup, update, cleanup, generate)
#define gridOpenTerm(width, height, setup, update, cleanup, generate) gridOpen(width, height, ASCII_FONT_TERM, setup, update, cleanup, generate)

#define error(type) return _error(type, __func__,__FILE__, __LINE__)

ASCIIGridStatus _error(ASCIIGridStatus status, const char *func, const char *file, int line);

/**
 * draws the grid
 * @filepath: the file path for the output image, if set to NULL output will be 
 * 			  to stdout
 * */
ASCIIGridStatus gridDraw(const char *filepath);

/**
 * frees the grid's memory
 * */
ASCIIGridStatus gridClose(void);

/**
 * sets the number of images that the grid should generate
 * */
ASCIIGridStatus gridSetMaxFrame(Frame max);

/**
 * clears the grid by overwriting everything with the background colour
 * */
ASCIIGridStatus gridClear(Color background);


/**
 * changes the grid's charset
 * @set: the set that we want to use
 * keep in mind that this is an expensive call right now,
 * I'd advise specifying the set in the openGrid function!
 * */
ASCIIGridStatus gridSetCharset(ASCIICharSet set);

/**
 * sets the delay between frames when printing out to terminal
 * */
ASCIIGridStatus gridSetFrameDelay(Delay del);

/**
 * returns the dimensions of the grid
 * */
Dimention gridGetDim();


//TODO: add a function that returns the resolution of a font and add a proper look up table for fonts
#endif //ASCII_GRID_H
