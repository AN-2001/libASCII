#ifndef ASCII_VECTOR_H
#define ASCII_VECTOR_H

typedef struct vector{
	double x,y;
} Vector;


typedef struct vector Direction;
typedef struct vector Position;
typedef struct vector Dimention;



//creates a vector!
Vector vectorCreate(double x, double y);
//adds two vectors
Vector vectorAdd(Vector src, Vector dest);
//subtracts two vectors
Vector vectorSub(Vector src, Vector dest);
//finds the dot product between two vectors
double vectorDot(Vector src, Vector dest);
//returns the magnitude of a vector
double vectorMag(Vector vec);
//returns the normalized version of the vector
Vector vectorNormalized(Vector vec);
//returns the angle between two vectors
double vectorAngBetween(Vector v1, Vector v2);
//multiplies the vector by the scalar
Vector vectorScale(Vector dest, double a);

//scales the dimentions down to dim
Dimention DimentionScale(Dimention dim, Dimention scaledDown);
#endif //ASCII_VECTOR_H

