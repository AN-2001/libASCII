#ifndef ASCII_GRID_H
#define ASCII_GRID_H
#include "ascii/vector.h"
#include "ascii/color.h"
#include "enums.h"

typedef struct asciiGrid *ASCIIGrid;
typedef int Frame;
typedef Color* Pixels;
typedef Color (*Generator)(Position pos);
typedef void (*Update)();
typedef void (*Setup)();
typedef unsigned Delay;


/**
 * opens the grid, always call this before doing anything
 * it initializes the library!
 * */
ASCIIGridStatus gridOpen(unsigned width, unsigned height
#ifdef ASCII_USE_GD
		, ASCIIFont font
#endif
		, Setup setup, Update update, Generator gen);

#ifdef ASCII_USE_GD

#define gridOpen_(w, h, s, u, g) gridOpen(w, h, ASCII_FONT_MEDIUM_BOLD, s, u, g)

#endif

#define error(type) return _error(type, __func__,__FILE__, __LINE__)

ASCIIGridStatus _error(ASCIIGridStatus status, const char *func, const char *file, int line);

/**
 * draws the grid
 * @filepath: the file path for the output image, if set to NULL output will be 
 * 			  to stdout
 * */
ASCIIGridStatus gridDraw(
#ifdef ASCII_USE_GD
		const char* filepath
#else
		void
#endif
		);

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
 * returns the current frame 
 * */
Frame gridGetCurrentFrame();


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
#endif //ASCII_GRID_H
