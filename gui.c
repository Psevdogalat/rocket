#include <gui.h>
#include <stdlib.h>
#include <GL/freeglut.h>

#define guiDefaultSize vector2f(1.0, 1.0);

GUIElement rootElement;

GUIElement * guiGetRootElement(){
	return &rootElement;
};

void drawRoot(GUIElement * const el, const Matrix3f transform){
	Vector2f verts[4];
	unsigned int i;
/*
	verts[0] = vector2f(-(el->size.x/2.0), -(el->size.y/2.0));
	verts[1] = vector2f(-(el->size.x/2.0),  (el->size.y/2.0));
	verts[2] = vector2f( (el->size.x/2.0), -(el->size.y/2.0));
	verts[3] = vector2f( (el->size.x/2.0),  (el->size.y/2.0));
*/
	verts[0] = vector2f(0.0, 0.0);
	verts[1] = vector2f(0.0, el->size.y);
	verts[2] = vector2f(el->size.x, 0.0);
	verts[3] = vector2f(el->size.x, el->size.y);

	for(i=0; i < 4; i++)
			verts[i] = mmul3fv(transform ,verts[i]);
	
	glColor3f(el->background.r, el->background.g, el->background.b);
	glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i < 4; i++)
			glVertex3f(verts[i].x, verts[i].y, 0.0f);
	glEnd();	

};

void guiInit(){
	initGUIElement(&rootElement);
	guiSetDrawFunc(&rootElement, drawRoot);
};

void guiFree(){

};

void drawElement(GUIElement * const el, const Matrix3f transform, 
	unsigned int level){
	Vector2f verts[4];
	unsigned int i;
	GUIElement * childChain;
	Matrix3f objectTransform;

	objectTransform = mmul3fm(mtrp3f(el->position), el->transform);
	objectTransform = mmul3fm(transform, objectTransform);

	verts[0] = vector2f(0.0, 0.0);
	verts[1] = vector2f(0.0, el->size.y);
	verts[2] = vector2f(el->size.x, 0.0);
	verts[3] = vector2f(el->size.x, el->size.y);

	for(i=0; i < 4; i++)
		verts[i] = mmul3fv(objectTransform, verts[i]);

	glStencilFunc(GL_EQUAL, level, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	glStencilMask(0xFF);
	glBlendFunc(GL_ZERO, GL_ONE);
	glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i < 4; i++)
			glVertex3f(verts[i].x, verts[i].y, 0.0f);
	glEnd();	
	level++;

	glStencilFunc(GL_EQUAL, level, 0xFF);
	glStencilMask(0x00);
	glBlendFunc(GL_ONE, GL_ZERO);
	el->cbDraw(el, objectTransform);

	childChain = el->child;
	while(childChain != NULL){
		drawElement(childChain, objectTransform, level);
		childChain = childChain->next;
	};
	
	
	glStencilFunc(GL_EQUAL, level, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	glStencilMask(0xFF);
	glBlendFunc(GL_ZERO, GL_ONE);
	glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i < 4; i++)
			glVertex3f(verts[i].x, verts[i].y, 0.0f);
	glEnd();	

};

void guiDraw(){
	drawElement(&rootElement, imatrix3f, 0);
};

void guiChainChild(GUIElement * const parent, GUIElement * const child){
	GUIElement * childChain;

	guiUnchainChild(child);

	if(parent->child == NULL){
		parent->child = child;	
		child->next = NULL;
		child->last = NULL;
	}else{

		childChain = parent->child;
		while(childChain->next != NULL)
			childChain = childChain->next;

		childChain->next = child;
		child->next = NULL;
		child->last = childChain;
	}

	child->parent = parent;
};

void guiUnchainChild(GUIElement * const child){
	GUIElement * chainChild, * last, * next, * parent;
	parent = child->parent;
	if(parent != NULL){
		chainChild = parent->child;
		while(chainChild != child)
			chainChild = chainChild->next;	
	
		last = chainChild->last;
		next = chainChild->next;

		if(chainChild == parent->child){
			parent->child = next;
			if(next != NULL) 
				next->last = NULL;
		}else{
			last->next = next;
			if(next != NULL)
				next->last = last;
		};

		child->parent = NULL;
		child->next = NULL;
		child->last = NULL;
	}
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

void guiSetVisible(GUIElement * const el, const bool visible){
	el->visible = visible;
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
		verts[i] = mmul3fv(transform ,verts[i]);
	
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

/*
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
*/

GUIElement * createGUIElement(GUIElement * const parent, 
	const Vector2f position, const Vector2f size){
	GUIElement * newElement;

	newElement = (GUIElement*)malloc(sizeof(GUIElement));
	if(newElement == NULL)
		return NULL;

	initGUIElement(newElement);
	if(parent == NULL){
		guiChainChild(&rootElement, newElement);
	}else{
		guiChainChild(parent, newElement);
	};

	guiSetPosition(newElement, position);
	guiSetSize(newElement, size);

	return newElement;
};

void freeGUIElement(GUIElement * const el){

	guiUnchainChild(el);
	while(el->child != NULL)
		freeGUIElement(el->child);
	
	free(el);
};
