#include "triangle.h"
#include "utill.h"
#include "limits.h"
Triangle triangleCreate(Vector vertecies[3]){
	Triangle res;
	for(int i = 0; i < 3;i++){
		int next = i + 1;
		if(next >= 3) next = 0;
		Vector dir = vectorSub(vertecies[next], vertecies[i]);
		double len = vectorMag(dir);
		dir = vectorNormalized(dir);
		res.sides[i] = rayCreate(vertecies[i], dir, len);	
	}
	return res;
}

double triangleDistToPoint(Triangle triangle, Vector point){
	double min = INT_MAX;
	int found = 0;
	for(int i = 0; i < 3;i++){
		double len = rayDistToPoint(triangle.sides[i], point);	
		if(len < 0)
			continue;

		if(len < min)
			min = len;

		found = 1;
	}

	if(found == 0)
		return -1;

	return min;
}

//TODO: remove this and implement it properly
static double crossThree (Vector p1, Vector p2, Vector p3)
{
    return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
}

int triangleIsPointInside (Triangle trig, Vector point)
{
    double d1 = crossThree(point, trig.sides[0].position, trig.sides[1].position);
    double d2 = crossThree(point, trig.sides[1].position, trig.sides[2].position);
    double d3 = crossThree(point, trig.sides[2].position, trig.sides[0].position);
    int hn = (d1 < 0) || (d2 < 0) || (d3 < 0);
    int hp = (d1 > 0) || (d2 > 0) || (d3 > 0);
    return !(hn && hp);
}	

void triangleTransform(Triangle * const triangle, Mat2x3 const * const mat){
	for(int i = 0; i < 3;i++){
		triangle->sides[i].position = mat2x3MultVec(*mat, triangle->sides[i].position); 
	}
	Vector vertecies[3] = {triangle->sides[0].position, triangle->sides[1].position, triangle->sides[2].position};
	*triangle = triangleCreate(vertecies);
}

Triangle triangleTransformed(Triangle triangle, Mat2x3 const * const mat){
	triangleTransform(&triangle, mat);
	return triangle;
}
