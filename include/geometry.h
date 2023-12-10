#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#define _USE_MATH_DEFINES
#include <math.h>

#ifdef __cplusplus
extern "C"{
#endif

typedef struct{float x,y;}Vector2f;
typedef struct{float m[3][3];} Matrix3f;

#ifdef _USE_VECTOR2F_MACRO
#define vector2f(x, y) 	({Vector2f ret = {x, y}; ret;})
#else
Vector2f vector2f(const float x, const float y);
#endif

#define imatrix3f ({Matrix3f ret = {1,0,0, 0,1,0, 0,0,1}; ret;})
#define normal2f vector2f(1.0, 0.0) 
#define vortor2f(v) vector2f(v.y, -v.x)

#ifdef _USE_VECTOR2F_MACRO
#define vadd2f(v1, v2) vector2f(v1.x + v2.x, v1.y + v2.y)
#define vmul2fs(v, s) vector2f(v.x * s, v.y * s)
#define vrot2fa(v, a) {vector2f(cos(a) * v.x - sin(a) * v.y, \
	cos(a) * v.x + sin(a) * v.y)
#define vrot2fn(v, n) vector2f(n.x * v.x - n.y * v.y, n.y * v.x + n.x * v.y)
#define vmod2f(v) sqrt(v.x * v.x + v.y * v.y)
#define vnorm2fa(a) vector2f(cos(a), sin(a))
#else
Vector2f vadd2f(const Vector2f v1, const Vector2f v2);
Vector2f vmul2fs(const Vector2f vector, const float scalar);
Vector2f vrot2fa(const Vector2f vector, const float angle);
Vector2f vrot2fn(const Vector2f vector, const Vector2f normal);
float	 vmod2f(const Vector2f vector);
Vector2f vnorm2fa(const float angle);
#endif

Vector2f vnorm2f(const Vector2f vector);

Vector2f mmul3fv(const Matrix3f mt, const Vector2f vec);
Matrix3f mmul3fm(const Matrix3f m1, const Matrix3f m2);
#define mtrp3f(v) ({Matrix3f ret = {1,0,v.x, 0,1,v.y, 0,0,1}; ret;})

#ifdef __cplusplus
}
#endif

#endif
