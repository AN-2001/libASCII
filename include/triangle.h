#ifndef RASTER_TRIANGLE_H
#define RASTER_TRIANGLE_H

#include "ray.h"
#include "mat2x3.h"
typedef struct triangle_t{
	Ray sides[3];
}Triangle;

Triangle triangleCreate(Vector vertecies[3]);
double triangleDistToPoint(Triangle triangle, Vector point);
int triangleIsPointInside(Triangle triangle, Vector point);
void triangleTransform(Triangle * const triangle, Mat2x3 const * const mat);

Triangle triangleTransformed(Triangle triangle, Mat2x3 const * const mat);
#endif //RASTER_TRIANGLE_H
