#include "vector.h"
#include "math.h"
Vector vectorCreate(double x, double y){
	Vector res;
	res.x = x;
	res.y = y;
	return res;
}
Vector vectorAdd(Vector first, Vector second){
	Vector res;
	res.x = first.x + second.x;
	res.y = first.y + second.y;
	return res;
}

Vector vectorSub(Vector first, Vector second){
	return vectorAdd(first, vectorScale(second, -1));
}

Vector vectorScale(Vector vec, double a){
	Vector res;
	res.x = vec.x * a;
	res.y = vec.y * a;
	return res;
}

double vectorDot(Vector v1, Vector v2){
	return (v1.x * v2.x + v1.y * v2.y);
}

double vectorMag(Vector vec){
	double squared = vectorDot(vec, vec);
	return sqrt(squared);
}

Vector vectorNormalized(Vector vec){
	double mag = vectorMag(vec);
	if(mag == 0)
		return vectorCreate(0, 0);
	return vectorScale(vec, 1.0 / mag);
}
