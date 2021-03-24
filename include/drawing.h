#ifndef ASCII_DRAWING_H
#define ASCII_DRAWING_H
#include "color.h"
#include "vector.h"
#include "enums.h"
//TODO: should the colour state be saved?
ASCIIGridStatus gridDrawPoint(Position pos, Color col);

ASCIIGridStatus gridDrawEllipse(Position centre, Dimention dim, Color col);

#define gridDrawCircle(position, radius, col) gridDrawEllipse(position, vectorCreate(radius, radius), col)

ASCIIGridStatus gridDrawLine(Position start, Position end, Color col);

#endif //ASCII_DRAWING_H
