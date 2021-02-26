#ifndef ASCII_COLOR_H
#define ASCII_COLOR_H
#define ASCII_START_ATTR "\033["
#define ASCII_FG "38"
#define ASCII_BG "48"
#define ASCII_RGB ";2"
#define ASCII_END_ATTR "m"
extern const char* current_set;
extern unsigned current_size;
typedef struct color_t{
	double r,g,b;
} Color;

typedef enum{
	ASCII_SET_BIG = 0,
	ASCII_SET_SMALL
} ASCIICharSet;


unsigned setFGColor(char* buff, Color foreground);
unsigned setBGColor(char* buff, Color background);
void resetColorMode(char* buff);


//sets the charset
void colorSetCharset(ASCIICharSet set);
//adds two colors 
Color colorAdd(Color c1, Color c2);

//creates a new color
Color colorCreate(double r, double g, double b);

//multiplies a color by a scalar
Color colorMult(Color c1, double a);


inline char colorToChar(Color col){
	double brightness = col.r*0.299 + col.g*0.587 + col.b*0.114;
	int index = (int)(brightness * current_size );
	return 	current_set[index];
}

#endif //ASCII_COLOR_H
