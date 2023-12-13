#include <geometry.h>
#include <math.h>

Vector2f vector2f(const float x, const float y){
	Vector2f ret = {x, y};
	return ret;
};

Vector2f vnorm2fn(){
	Vector2f ret = {1.0, 0.0};
	return ret;
};

Vector2f vmul2fs(const Vector2f vector, const float scalar){
	Vector2f ret = {vector.x * scalar, vector.y * scalar};
	return ret;
};

Vector2f vadd2f(const Vector2f v1, const Vector2f v2){
	return vector2f(v1.x + v2.x , v1.y + v2.y);
};

float vmod2f(const Vector2f v){
	return sqrt(v.x * v.x + v.y * v.y);
};

Vector2f vnorm2fa(const float angle){
	Vector2f ret = {cos(angle), sin(angle)};
	return ret;
};

Vector2f vnorm2f(const Vector2f vector)
{
	float module = vmod2f(vector);
	if(module == 0)
		return normal2f;

	return vmul2fs(vector, 1/module);
};

Vector2f mmul3fv(const Matrix3f mt, const Vector2f vec){
	return vector2f(
		mt.m[0][0] * vec.x + mt.m[0][1] * vec.y + mt.m[0][2],
		mt.m[1][0] * vec.x + mt.m[1][1] * vec.y + mt.m[1][2]
	);
};

Matrix3f mmul3fm(const Matrix3f m1, const Matrix3f m2){
	Matrix3f ret;
	unsigned int i,j;

	for(i=0; i<3; i++){
		for(j=0; j<3; j++){
			ret.m[i][j] = 
				m1.m[i][0]*m2.m[0][j] + 
				m1.m[i][1]*m2.m[1][j] + 
				m1.m[i][2]*m2.m[2][j];
		};
	};

	return ret;
};
