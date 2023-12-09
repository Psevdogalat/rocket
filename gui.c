#include <gui.h>
#include <stdlib.h>
#include <GL/freeglut.h>

#define guiDefaultSize vector2f(1.0, 1.0);

GUIElement rootElement;


void guiInit(){
	initGUIElement(&rootElement);
};

void guiFree(){

};

void drawElement(GUIElement * const el, const Matrix3f transform){
	GUIElement * childChain;
	Matrix3f objectTransform;

	objectTransform = mmul3fm(transform, mtrp3f(el->position))	
	objectTransform = mmul3fm(objectTransform, el->transform);
	el->cbDraw(el, objectTransform);

	childChain = el->child;
	while(childChain != NULL){
		drawElement(childChain, objectTransform);
		childChain = childChain->next;
	};
};

void guiDraw(){
	drawElement(&rootElement, imatrix3f);
};

void chainGUIElement(GUIElement * const el, GUIElement * const parent){
	GUIElement * childChain;
	
	if(parent->child == NULL){
		parent->child = el;	
		el->next = NULL;
		el->last = NULL;
	}else{

		childChain = parent->child;
		while(childChain->next != NULL)
			childChain = childChain->next;

		childChain->next = el;
		el->next = NULL;
		el->last = childChain;
	}

	el->parent = parent;
};

void initGUIElement(GUIElement * const el){
	el->parent = NULL;
	el->child = NULL;
	el->next = NULL;
	el->last = NULL;
	el->background = colorGray;
	el->foreground = colorBlack;
	el->transform = imatrix3f;
	el->position = vector2f(0.0, 0.0);
	el->size = vector2f(1.0, 1.0);
	el->cbDraw = guiDefaultDrawFunc;
	el->cbResize = guiDefaultResizeFunc;
	el->cbKey = guiDefaultKeyFunc;
	el->cbMouse = guiDefaultMouseFunc;
};

void guiSetPosition(GUIElement * const el, const Vector2f pos){
	el->position = pos;
};

void guiSetSize(GUIElement * const el, const Vector2f size){
	if(size.x <= 0.0 || size.y <= 0.0){
		el->size = guiDefaultSize;
		return;
	}

	el->size = size;
};

void guiSetForegroundColor(GUIElement * const el, const ColorRGBf color){
	el->foreground = color;
};

void guiSetBackgroundColor(GUIElement * const el, const ColorRGBf color){
	el->background = color;
};

void guiSetDrawFunc(GUIElement * const el, GUIDrawFunc callback){
	if(callback != NULL){
		el->cbDraw = callback;
		return;
	};

	el->cbDraw = guiDefaultDrawFunc;
};

void guiSetResizeFunc(GUIElement * const el, GUIResizeFunc callback){
	if(callback != NULL){
		el->cbResize = callback;
		return;
	};

	el->cbResize = guiDefaultResizeFunc;
};

void guiSetKeyFunc(GUIElement * const el, GUIKeyFunc callback){
	if(callback != NULL){
		el->cbKey = callback;
		return;
	};

	el->cbKey = guiDefaultKeyFunc;
};

void guiSetMouseFunc(GUIElement * const el, GUIMouseFunc callback){
	if(callback != NULL){
		el->cbMouse = callback;
		return;
	};

	el->cbMouse = guiDefaultMouseFunc;
};



void guiDefaultDrawFunc(GUIElement * const el, const Matrix3f transform){
	Vector2f verts[4];
	unsigned int i;

	verts[0] = vector2f(0.0, 0.0);
	verts[1] = vector2f(0.0, el->size.y);
	verts[2] = vector2f(el->size.x, 0.0);
	verts[3] = vector2f(el->size.x, el->size.y);

	for(i=0; i < 4; i++)
		verts[i] = vadd2f(verts[i], el->position);
	
	glColor3f(el->background.r, el->background.g, el->background.b);
	glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i < 4; i++)
			glVertex3f(verts[i].x, verts[i].y, 0.0f);
	glEnd();	

};

void guiDefaultResizeFunc(GUIElement * const element, const Vector2f size){

};

void guiDefaultMouseFunc(GUIElement * const element, const Vector2f coord,
	const unsigned int buttons){

};

void guiDefaultKeyFunc(GUIElement * const element, const char key, 
	const unsigned int mod){

};

void drawTriangle(const ColorRGBf color, const Vector2f position, 
const Vector2f normal)
{
	Vector2f vertices[3];
	unsigned int i;
	float angle;

	angle = M_PI / 2.0;
	for(i = 0; i < 3; i++)
	{
		vertices[i] = vadd2f(vrot2fn(vmul2fs(vnorm2fa(angle), 2), normal), 
			position); 
		angle += 2.0 * M_PI / 3.0;
	}


	glColor3f(color.r, color.g, color.b);
	glBegin(GL_TRIANGLES);

		for(i = 0; i < 3; i++)
			glVertex3f(vertices[i].x, vertices[i].y, 0.0f);

	glEnd();	
};

