#include "mat2x3.h"

Mat2x3 mat2x3Create(Vector columns[3]){
	Mat2x3 res;
	for(int i = 0; i < 3;i++){
		res.columns[i] = columns[i];
	}
	return res;
}
Mat2x3 mat2x3Add(Mat2x3 m1, Mat2x3 m2){
	Mat2x3 res;
	for(int i = 0; i < 3;i++){
		res.columns[i] = vectorAdd(m1.columns[i], m2.columns[i]);	
	}
	return res;
}
Mat2x3 mat2x3Scale(Mat2x3 m1, double a){
	Mat2x3 res;
	for(int i = 0; i < 3;i++){
		res.columns[i] = vectorScale(m1.columns[i], a);	
	}
	return res;
}
Mat2x3 mat2x3Sub(Mat2x3 m1, Mat2x3 m2){
	return mat2x3Add(m1, mat2x3Scale(m2, -1));
}
Vector mat2x3MultVec(Mat2x3 mat, Vector vec){
	Vector res = vectorCreate(0, 0);
	double scalars[3] = {vec.x, vec.y, 1};
	for(int i = 0; i < 3;i++){
		res = vectorAdd(res, vectorScale(mat.columns[i], scalars[i]));
	}
	return res;
}
