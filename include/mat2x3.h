#ifndef ASCII_MAT2X3_H
#define ASCII_MAT2X3_H
#include "vector.h"
//each transformation matrix is 3 columns, first two are for rotation/scaling third is for translation
typedef struct matrix2x3_t{
	Vector columns[3];
}Mat2x3;
//crates a new matrix
Mat2x3 mat2x3Create(Vector columns[3]);
//does matrix addition
Mat2x3 mat2x3Add(Mat2x3 m1, Mat2x3 m2);
//matrix subtraction
Mat2x3 mat2x3Sub(Mat2x3 m1, Mat2x3 m2);
//does scalar matrix multiplication
Mat2x3 mat2x3Scale(Mat2x3 m1, double a);
//does vector matrix multiplication
Vector mat2x3MultVec(Mat2x3 mat, Vector vec);
#endif // ASCII_MAT2X3_H
