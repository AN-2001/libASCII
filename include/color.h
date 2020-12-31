#ifndef RASTER_COLOR_H
#define RASTER_COLOR_H

typedef struct color_t{
	double r,g,b;
} Color;

//adds two colors 
Color colorAdd(Color c1, Color c2);

//creates a new color
Color colorCreate(double r, double g, double b);

//multiplies a color by a scalar
Color colorMult(Color c1, double a);

//turns a color into a char
char colorToChar(Color col);
#endif //RASTER_COLOR_H
