#include "ray.h"
#include "utill.h"
double rayClosestToPoint(Ray ray, Vector point){
	ray.direction = vectorNormalized(ray.direction);
	Vector sub = vectorSub(point, ray.position);
	double a = vectorDot(ray.direction, sub);
	return a;
}

double rayDistToPoint(Ray ray, Vector point){
	double closest = rayClosestToPoint(ray, point);

	if(closest < 0 || closest > ray.length){
		return -1;
	}

	Vector closestPoint = rayGenerate(ray, closest);
	Vector delta = vectorSub(point, closestPoint);
	return vectorMag(delta);
}

Ray rayCreate(Position pos, Direction dir, double len){
	Ray newRay;
	newRay.position = pos;
	newRay.direction = vectorNormalized(dir);
	newRay.length = len;
	return newRay;
}

Vector rayGenerate(Ray ray, double a){
	ray.direction = vectorNormalized(ray.direction);
	return vectorAdd(ray.position, vectorScale(ray.direction, a));
}

double rayPointDir(Ray ray, Vector point){
	double closest = rayClosestToPoint(ray, point);
	Vector closestPoint = rayGenerate(ray, closest);
	Vector perp = vectorSub(closestPoint, point);
	Vector diff = vectorSub(ray.position, point);
	double first = perp.y * diff.x;
	double second = perp.x * diff.y;
	return signf(first - second);
}
