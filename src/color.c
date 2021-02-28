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
