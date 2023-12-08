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

