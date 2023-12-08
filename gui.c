#include <gui.h>
#include <stdlib.h>
#include <GL/freeglut.h>

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

