#include <stdlib.h>
#include <stdio.h>
#include <GL/freeglut.h>

#include <gui.h>
#define _USE_MATH_DEFINES
#include <math.h>

float angle = 0.0;

ColorRGBf colors[6];
GUIElement elements[6];

void initGuiElements(){
	unsigned int i;
	ColorRGBf colors[6] = {
		colorRed,
		colorBlue,
		colorGreen,
		colorYelow,
		colorGray,
		colorLGray
	};

	for(i = 0; i < 6; i++){
		initGUIElement(elements + i);
		guiSetBackgroundColor(elements + i, colors[i]);
	};
};

void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);
	Vector2f pos;
	float angle2;
	unsigned int i;
	
	angle2 = angle;
	for (i=0; i < 6; i++, angle2 += (2*M_PI/6)){
		pos = vmul2fs(vnorm2fa(angle2), 3.0); 
		guiSetPosition(elements + i, pos);
		elements[i].cbDraw(elements + i);

		//drawTriangle(colArr[i], pos, vortor2f(vnorm2f(pos))); 	
	}

	glutSwapBuffers();
}

void timerFunc(int id)
{
	angle += M_PI/128;

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
	glOrtho(-10 * ratio, 10 * ratio, -10, 10, -10, 10);

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

	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);
	glutKeyboardFunc(keyboardFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(mouseActiveMotionFunc);
	timerFunc(100);

	glClearColor(0,0,0,0);

	initGuiElements();

	glutMainLoop();
	return 0;
};
