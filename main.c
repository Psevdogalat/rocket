#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <GL/freeglut.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define _USE_VECTOR2F_MACRO
#include <geometry.h>

#define timerDelta 1000/60
#define coordAreaXMin -20.0
#define coordAreaXMax 20.0
#define coordAreaYMin -20.0
#define coordAreaYMax 20.0

Vector2f rocketPosition;
#define earthRadius 6371000.0
#define hiOrbit (35786000.0 + earthRadius)
#define loOrbit (160000.0 + earthRadius)


void displayFunc();
void timerFunc(int id);
void reshapeFunc(int width, int height);

void init();
void calc(double time);
void draw();

int main(unsigned int argc, char * argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(400,400);
	glutInitWindowPosition(200,200);
	
	glutCreateWindow("Main window");

	timerFunc(100);
	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);

	glClearColor(0,0,0,0);
	glutMainLoop();

	return 0;
};

void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);
	draw();
	glutSwapBuffers();
};

void timerFunc(int id)
{
	if(id = 100){
		init();
	}else{
		calc(timerDelta);
	}
    glutTimerFunc(timerDelta, timerFunc, 0);
	glutPostRedisplay();
};

void reshapeFunc(int width, int height)
{

	if(height == 0) height = 1;
	float ratio = (float)width/(float)height;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(coordAreaXMin * ratio, coordAreaXMax * ratio, 
		coordAreaYMin, coordAreaYMax, -10, 10);
};


void init()
{
	rocketPosition = vector2f(0.0, 0.0);

};

void calc(double cycleTime){

};


void drawCircle(const Vector2f position, float radius, unsigned int segments, 
	bool filled)
{
	unsigned int i;
	float alfa, delta;
	Vector2f vertex;

	if(segments < 8) segments = 8;
	delta = (2.0 * M_PI / (float)segments);

	if(filled){
		glBegin(GL_TRIANGLE_FAN);
		vertex = position;
		glVertex3f(vertex.x, vertex.y, 0.0);

		for(i = 0, alfa = 0.0; i <= segments; i++){
			vertex = vadd2f(
				vrot2fa(vector2f(radius, 0.0), alfa), 
				position);

			glVertex3f(vertex.x, vertex.y, 0.0);
			alfa += delta; 
		};
		glEnd();
	}else{
		glBegin(GL_LINE_LOOP);
		for(i = 0, alfa = 0.0; i < segments; i++){
			vertex = vadd2f(
				vrot2fa(vector2f(radius, 0.0), alfa), 
				position);

			glVertex3f(vertex.x, vertex.y, 0.0);
			alfa += delta; 
		};
		glEnd();
	}

};

void drawTriangle(const Vector2f position, float radius){
	unsigned int i;
	float alfa;
	Vector2f vertex;

	glBegin(GL_TRIANGLES);
	for(i = 0, alfa = M_PI/2.0; i < 3; i++){
		vertex = vadd2f( vrot2fa(vector2f(radius, 0.0), alfa), 
			position);
		glVertex3f(vertex.x, vertex.y, 0.0);
		alfa += (2.0 * M_PI / 3.0);
	};
	glEnd();
};

#define useGLC_LGreen() glColor3f(0.0, 0.9, 0.0)
#define useGLC_Green() glColor3f(0.0, 0.5, 0.0)
#define useGLC_LYellow() glColor3f(0.8, 0.8, 0.0)
#define useGLC_LRed() glColor3f(0.9, 0.0, 0.0)

void draw(){
	double scale = coordAreaYMax / (hiOrbit + 1000000.0);

	useGLC_Green();
	drawCircle(vector2f(0.0, 0.0), scale * earthRadius , 32, true);

	useGLC_LGreen();
	drawCircle(vector2f(0.0, 0.0), scale * hiOrbit, 32, false);
	drawCircle(vector2f(0.0, 0.0), scale * loOrbit, 32, false);

	useGLC_LRed();
	drawTriangle(vmul2fs(rocketPosition, scale), 1);

};

