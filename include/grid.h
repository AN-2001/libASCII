#ifndef ASCII_GRID_H
#define ASCII_GRID_H
#include "vector.h"
#include "color.h"

typedef enum{
	ASCII_GRID_OUT_OF_MEMORY,
	ASCII_GRID_SUCCESS,
	ASCII_GRID_BAD_ARGUMENT,
	ASCII_GRID_ERROR
} ASCIIGridStatus;

typedef enum{
	ASCII_GRID_TINY = 0,
	ASCII_GRID_MEDIUM_BOLD,
	ASCII_GRID_GIANT,
	ASCII_GRID_SMALL,
} ASCIIFont;

typedef struct asciiGrid *ASCIIGrid;
typedef int Frame;
typedef Color* Pixels;
typedef Color (*Generator)(Position pos, Dimention dim, Frame frame);
typedef void (*Update)(Frame frame);
typedef void (*Setup)();



//makes a grid and sets the color black
ASCIIGridStatus gridOpen(unsigned width, unsigned height, Setup setup, Update update, Generator gen);

//draws a a grid into an image
ASCIIGridStatus gridDrawToImage(const char* filepath);

//clears a pixel grid's memory
ASCIIGridStatus gridClose(void);

//sets the max frame in a grid
ASCIIGridStatus gridSetMaxFrame(Frame max);

//clears the ASCII image in a grid
ASCIIGridStatus gridClear(void);
//sets the grid's font
ASCIIGridStatus gridSetFont(ASCIIFont font);
//sets the char set 
ASCIIGridStatus gridSetCharset(ASCIICharSet set);
//prints the grid in text form
void printGrid();
#endif //ASCII_GRID_H
