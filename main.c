#include <stdlib.h>
#include <stdio.h>
#include <GL/freeglut.h>

#include <gui.h>
#define _USE_MATH_DEFINES
#include <math.h>

float angle = 0.0;
Vector2f rootSize;
ColorRGBf colors[6];
GUIElement * elements[6];

void initGuiElements(){
	unsigned int i;
	GUIElement * root;

	ColorRGBf colors[6] = {
		colorRed,
		colorBlue,
		colorGreen,
		colorYelow,
		colorGray,
		colorLGray
	};

	root = guiGetRootElement();
	rootSize = vector2f(20 , 20);
	guiSetSize(root, rootSize);
	root->transform = mtrp3f(vmul2fs(rootSize, 0.5));

	for(i = 0; i < 6; i++){
		elements[i] = createGUIElement(NULL, vector2f(0.0, 0.0), 
			vector2f(1.0, 1.0));
		guiSetBackgroundColor(elements[i], colors[i]);
	};
};


void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);
	guiDraw();
	glutSwapBuffers();
}

void timerFunc(int id)
{
	Vector2f pos;
	float angle2;
	unsigned int i;

	angle += M_PI/128;
	angle2 = angle;
	for (i=0; i < 6; i++, angle2 += (2*M_PI/6)){
		pos = vmul2fs(vnorm2fa(angle2), 3.0); 
		guiSetPosition(elements[i], pos);
	}


	glutPostRedisplay();
    glutTimerFunc(1000/60, timerFunc, 0);
}

void reshapeFunc(int width, int height)
{

	if(height == 0) height = 1;
	float ratio = (float)width/(float)height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, rootSize.x * ratio, 0, rootSize.y, -10, 10);
	GUIElement * root = guiGetRootElement();

}

void keyboardFunc(unsigned char key, int x, int y)
{
	printf("keyPressed %d on %d %d\n", key, x, y); 
}

void mouseFunc(int button, int state, int x, int y)
{
	printf("mousePressed on %d %d\n", x, y);
}

void mouseActiveMotionFunc(int x, int y)
{
	printf("activeMotion %d %d\n", x, y);
}


int main(unsigned int argc, char * argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(400,400);
	glutInitWindowPosition(200,200);
	
	glutCreateWindow("Main window");

	guiInit();
	initGuiElements();

	timerFunc(100);
	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(mouseActiveMotionFunc);
	glClearColor(0,0,0,0);
	glutMainLoop();

	guiFree();
	return 0;
};
