#ifndef ASCII_DRAWING_H
#define ASCII_DRAWING_H
#include "color.h"
#include "vector.h"
#include "enums.h"
//TODO: should the colour state be saved?
typedef struct {
	Position vertices[3];
	union{	
		struct {
			unsigned char e1 : 1;
			unsigned char e2 : 1;
			unsigned char e3 : 1;
		};
		unsigned char val;
	}flags;

} Triangle;

Triangle gridCreateTriangle(Position p1, Position p2, Position p3);
void gridTriangleSetFlags(Triangle *triangle, unsigned char flags);

ASCIIGridStatus gridDrawPoint(Position pos, Color col);

ASCIIGridStatus gridDrawEllipse(Position centre, Dimention dim, Color col);

#define gridDrawCircle(position, radius, col) gridDrawEllipse(position, vectorCreate(radius, radius), col)

ASCIIGridStatus gridDrawLine(Position start, Position end, Color col);

ASCIIGridStatus gridDrawTriangle(Triangle triangle, Color col);

#endif //ASCII_DRAWING_H
