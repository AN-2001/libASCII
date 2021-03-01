#include "color.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//constants
#define CHAR_COUNT_BIG 70.0
#define CHAR_COUNT_SMALL 10.0
static const char* const colors_big = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
static const char* const colors_small = " .:-=+*#%@";

//TODO: add xmacro here
static const char* const sets[2] = {colors_big, colors_small};
static const unsigned sizes[2] = {CHAR_COUNT_BIG, CHAR_COUNT_SMALL};

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

Color colorCreate(double r, double g, double b){
	Color res;
	res.r = r;
	res.g = g;
	res.b = b;
	return res;
}

Color colorMult(Color c1, double a){
	return colorCreate(c1.r * a, c1.g * a, c1.b * a);
}


static unsigned setFGColor(char* buff, Color foreground){
	char s[64];
	sprintf(s, ASCII_START_ATTR ASCII_FG ASCII_RGB ";%d;%d;%d"ASCII_END_ATTR,
						(int)(foreground.r*255), (int)(foreground.g*255), (int)(foreground.b*255));
	sprintf(buff, "%s", s);	
	return strlen(s);
}

unsigned colorPrint(char *buff, const char *content, Color fg){
	unsigned colorCount = setFGColor(buff, fg);
	buff += colorCount;
	return colorCount + sprintf(buff, "%s", content);
}

Color colorHSVToRGB(int h, int s_, int v_){
	double s = s_ / 255.0;	
	double v = v_ / 255.0;

	double c = v * s;	
	double x = c * ( 1 - abs((h / 60) % 2 - 1));
	double m = v - c;
	Color rgbPrime;
	if(h >= 0 && h <= 60)
		rgbPrime = colorCreate(c, x, 0);
	if(h >= 60 && h <= 120)
		rgbPrime = colorCreate(x, c, 0);
	if(h >= 120 && h <= 180)
		rgbPrime = colorCreate(0, c, x);
	if(h >= 180 && h <= 240)
		rgbPrime = colorCreate(0, x, c);
	if(h >= 240 && h <= 300)
		rgbPrime = colorCreate(x, 0, c);
	if(h >= 300 && h <= 360)
		rgbPrime = colorCreate(c, 0, x);
	return colorCreate((rgbPrime.r + m) * 255, (rgbPrime.g + m) * 255, (rgbPrime.b + m) * 255);
}
