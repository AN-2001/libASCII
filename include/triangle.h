#ifndef ASCII_TRIANGLE_H
#define ASCII_TRIANGLE_H

#include "ray.h"
#include "mat2x3.h"
typedef struct triangle_t{
	Ray sides[3];

}Triangle;

//creates a new triangle
Triangle triangleCreate(Position vertecies[3]);
//returns a point's ditance from a triangle(distance as in the distance between the point and the closest side of the triangle)
double triangleDistToPoint(Triangle triangle, Position point);
//returns a positive int if the point is inside the triangle
int triangleIsPointInside(Triangle triangle, Position point);
//multiplies a triangle by a matrix
void triangleTransform(Triangle * const triangle, Mat2x3 const * const mat);
//copies a triangle, mulitplies it by a matrix, returns the copy
Triangle triangleTransformed(Triangle triangle, Mat2x3 const * const mat);

#endif //ASCII_TRIANGLE_H
