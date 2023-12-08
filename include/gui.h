#ifndef __GUI_H__
#define __GUI_H__

#define _USE_VECTOR2F_MACRO
#include <geometry.h>
#

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{float r,g,b;}ColorRGBf;
#define colorRGBf(r,g,b) ({ColorRGBf ret = {r, g, b}; ret;})

typedef void (* GuiElementDraw)	(void * const element);
typedef void (* GuiElementMouse)(void * const element, const Vector2f coord,
	const unsigned int buttons);
typedef void (* GuiElementKey)	(void * const element, const char key, 
	const unsigned int mod); 
typedef void (* GuiElementResize) (void * const element, const Vector2f size); 


typedef struct{
	ColorRGBf background;
	ColorRGBf foreground;
	Vector2f position;
	Vector2f size;
	GuiElementDraw 		cbDraw;
	GuiElementResize 	cbResize;
	GuiElementKey		cbKey;
	GuiElementMouse 	cbMouse;
}GuiBaseElement;

void drawTriangle(const ColorRGBf color, const Vector2f position, 
	const Vector2f normal);

#ifdef __cplusplus
}
#endif

#endif
