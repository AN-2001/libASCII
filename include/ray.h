#ifndef ASCII_RAY_H
#define ASCII_RAY_H
#include "vector.h"

typedef struct ray{
	Position position;
	Direction direction;
	double length;
} Ray;


//generates a vector from the ray
Vector rayGenerate(Ray ray, double a);


//makes a new ray
Ray rayCreate(Position pos, Direction dir, double length);

//returns the closest point on a ray to an other point
double rayClosestToPoint(Ray ray, Vector point);

//returns the distance between a point and a ray
double rayDistToPoint(Ray ray, Vector point);

//returns 1 if the point is on the right of the ray -1  if it's on the left and 0 if it's on the line
double rayPointDir(Ray ray, Vector Point);

#endif // ASCII_RAY_H
