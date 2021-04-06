#include "color.h"
#include "utill.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//constants
#define CHAR_COUNT_BIG 70.0
#define CHAR_COUNT_SMALL 10.0
static const char* const colors_big = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
static const char* const colors_small = " .:-=+*#%@";

//TODO: add xmacro here
static const char* const sets[2] = {colors_big, colors_small};
static const unsigned sizes[2] = {CHAR_COUNT_BIG, CHAR_COUNT_SMALL};
extern inline int colorIsEqual(Color col, Color col1);

//to avoid using too many if statemnts
const char* current_set = colors_big;
unsigned current_size = CHAR_COUNT_BIG;

void colorSetCharset(ASCIICharSet set){
	current_set = sets[set];	
	current_size = sizes[set];
}




Color colorAdd(Color c1, Color c2){
	return colorCreate(c1.r + c2.r, c1.g + c2.g, c1.b + c2.b);
}

Color colorCreate(int r, int g, int b){
	Color res;
	res.r = r;
	res.g = g;
	res.b = b;
	return res;
}
Color colorCreate_(int col){
	Color res;
	res.packed = col;
	return res;
}

Color colorMult(Color c1, double a){
	return colorCreate(c1.r * a, c1.g * a, c1.b * a);
}

static unsigned setFGColor(Color foreground){
	return printf( ASCII_START_ATTR ASCII_FG ASCII_RGB ";%d;%d;%d"ASCII_END_ATTR,
						foreground.r, foreground.g, foreground.b);
}
unsigned colorPrintChar(char content, Color col){
	unsigned colorCount = setFGColor(col);
	return colorCount + putc(content, stdout);
}

Color colorHSVToRGB(int h_, int s_, int v_){
	double s = s_ / 255.0;	
	double v = v_ / 255.0;
	double h = fmod(h_ + ceil(fabs(h_/360.0)) * 360,  360);
	s = clamp(s, 0, 1.0); 
	v = clamp(v, 0, 1.0); 

	double c = v * s;	
	double h_norm = (h / 60.0);
	double x = c * ( 1 - fabs(fmod(h_norm, 2) - 1));
	double m = v - c;

	struct _col{
		double r,g,b;
	}col;

	if(h >= 0 && h <= 60)
		col = (struct _col){c, x, 0};
	if(h >= 60 && h <= 120)
		col = (struct _col){x, c, 0};
	if(h >= 120 && h <= 180)
		col = (struct _col){0, c, x};
	if(h >= 180 && h <= 240)
		col = (struct _col){0, x, c};
	if(h >= 240 && h <= 300)
		col = (struct _col){x, 0, c};
	if(h >= 300 && h <= 360)
		col = (struct _col){c, 0, x};

	return colorCreate((col.r + m) * 255, (col.g + m) * 255, (col.b + m) * 255);
}

