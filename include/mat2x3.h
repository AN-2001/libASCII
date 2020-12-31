#ifndef RASTER_TRANSFORMATION_H
#define RASTER_TRANSFORMATION_H
#include "vector.h"
//each transformation matrix is 3 columns, first two are for rotation/scaling third is for translation
typedef struct matrix2x3_t{
	Vector columns[3];
}Mat2x3;
Mat2x3 mat2x3Create(Vector columns[3]);
Mat2x3 mat2x3Add(Mat2x3 m1, Mat2x3 m2);
Mat2x3 mat2x3Sub(Mat2x3 m1, Mat2x3 m2);
Mat2x3 mat2x3Scale(Mat2x3 m1, double a);
Vector mat2x3MultVec(Mat2x3 mat, Vector vec);
#endif // RASTER_TRANSFORMATION_H
