#ifndef RASTER_GRID_H
#define RASTER_GRID_H
#include "vector.h"
#include "color.h"

typedef struct pixelGrid *PixelGrid;
typedef int Frame_t;
typedef Color* Pixels;
typedef Color (*Generator)(Position pos, Dimention dim, Frame_t frame);
typedef void (*Update)(PixelGrid grid, Frame_t frame);
typedef void (*Setup)();



//makes a grid and sets the color black
PixelGrid gridCreate(unsigned width, unsigned height, Setup setup, Update update, Generator gen);

//draws a a grid into an image
void gridDrawToImage(PixelGrid grid, const char* filepath);

//clears a pixel grid's memory
void gridFree(PixelGrid grid);

//sets the max frame in a grid
void gridSetMaxFrame(PixelGrid grid, Frame_t max);

//clears the ASCII image in a grid
void gridClear(PixelGrid grid);

//turns an x,y pair into an index
unsigned posToIndex(Position pos, unsigned width);

#endif //RASTER_GRID_H
