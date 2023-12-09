#ifndef __GUI_H__
#define __GUI_H__

#define _USE_VECTOR2F_MACRO
#include <geometry.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{float r,g,b;}ColorRGBf;
#define colorRGBf(r,g,b) ({ColorRGBf ret = {r, g, b}; ret;})
#define colorRed 	colorRGBf(1.0, 0.0, 0.0)
#define	colorGreen 	colorRGBf(0.0, 1.0, 0.0)
#define colorBlue 	colorRGBf(0.0, 0.0, 1.0)
#define colorGray 	colorRGBf(0.5, 0.5, 0.5)
#define colorWhite 	colorRGBf(1.0, 1.0, 1.0)
#define colorBlack 	colorRGBf(0.0, 0.0, 0.0)
#define colorYelow	colorRGBf(1.0, 1.0, 0.0)
#define colorLGray 	colorRGBf(0.8, 0.8, 0.8)

typedef struct __GUIElement GUIElement;

typedef void (* GUIDrawFunc)(GUIElement * const el, const Matrix3f transform);
typedef void (* GUIMouseFunc)(GUIElement * const el, const Vector2f coord,
	const unsigned int buttons);
typedef void (* GUIKeyFunc)	(GUIElement * const el, const char key, 
	const unsigned int mod); 
typedef void (* GUIResizeFunc) (GUIElement * const el, const Vector2f size); 


typedef struct __GUIElement{
	GUIElement * parent;
	GUIElement * child;
	GUIElement * next;
	GUIElement * last;
	ColorRGBf 	background;
	ColorRGBf 	foreground;
	Matrix3f	transform;
	Vector2f 	position;
	Vector2f 	size;
	GUIDrawFunc 	cbDraw;
	GUIResizeFunc 	cbResize;
	GUIKeyFunc		cbKey;
	GUIMouseFunc 	cbMouse;
}GUIElement;

void initGUIElement(GUIElement * const el);

void guiSetPosition(GUIElement * const el, const Vector2f pos);
void guiSetSize(GUIElement * const el, const Vector2f size);
void guiSetForegroundColor(GUIElement * const el, const ColorRGBf color);
void guiSetBackgroundColor(GUIElement * const el, const ColorRGBf color);
void guiSetDrawFunc(GUIElement * const el, GUIDrawFunc callback);
void guiSetResizeFunc(GUIElement * const el, GUIResizeFunc callback);
void guiSetKeyFunc(GUIElement * const el, GUIKeyFunc callback);
void guiSetMouseFunc(GUIElement * const el, GUIMouseFunc callback);

void guiDefaultDrawFunc(GUIElement * const el,  const Matrix3f transform);
void guiDefaultMouseFunc(GUIElement * const el, const Vector2f coord,
	const unsigned int buttons);
void guiDefaultKeyFunc(GUIElement * const el, const char key, 
	const unsigned int mod); 
void guiDefaultResizeFunc(GUIElement * const el, const Vector2f size); 


void guiInit();
void guiFree();
void guiDraw();

void chainGUIElement(GUIElement * const element, GUIElement * const parent);

void drawTriangle(const ColorRGBf color, const Vector2f position, 
	const Vector2f normal);

#ifdef __cplusplus
}
#endif

#endif
