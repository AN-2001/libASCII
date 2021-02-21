#ifndef ASCII_COLOR_H
#define ASCII_COLOR_H
typedef struct color_t{
	double r,g,b;
} Color;
typedef enum{
	ASCII_GRID_BIG_ASCII = 0,
	ASCII_GRID_SMALL_ASCII
} ASCIICharSet;
//sets the charset
void colorSetCharset(ASCIICharSet set);
//adds two colors 
Color colorAdd(Color c1, Color c2);

//creates a new color
Color colorCreate(double r, double g, double b);

//multiplies a color by a scalar
Color colorMult(Color c1, double a);

//turns a color into a char
char colorToChar(Color col);
#endif //ASCII_COLOR_H
