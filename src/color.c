#include <stdlib.h>
#include "color.h"


//constants
#define CHAR_COUNT_BIG 70.0
#define CHAR_COUNT_SMALL 10.0
static const char* const colors_big = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";
static const char* const colors_small = " .:-=+*#%@";

//TODO: add xmacro here
static const char* const sets[2] = {colors_big, colors_small};
static const size_t sizes[2] = {CHAR_COUNT_BIG, CHAR_COUNT_SMALL};

//to avoid using too many if statemnts
static const char* current_set = colors_big;
static size_t current_size = CHAR_COUNT_BIG;

void colorSetCharset(ASCIICharSet set){
	current_set = sets[set];	
	current_size = sizes[set];
}

char colorToChar(Color col){
	double brightness = col.r*0.299 + col.g*0.587 + col.b*0.114;

	int index = (int)(brightness * current_size );
	return 	current_set[index];
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
