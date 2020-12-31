#include "color.h"

// " .:-=+*#%@" for 10 characters
// " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$" for 70 chars

#define CHAR_COUNT 70.0
static const char* colors = " .'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$";

char colorToChar(Color col){
	double brightness = col.r*0.299 + col.g*0.587 + col.b*0.114;
	int index = (int)(brightness * CHAR_COUNT );
	return 	colors[index];
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
