#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <GL/freeglut.h>
#define _USE_MATH_DEFINES
#include <math.h>
#define _USE_VECTOR2F_MACRO
#include <geometry.h>
#include <windows.h>

#define timerDelta 1000/60
#define coordAreaXMin -20.0
#define coordAreaXMax 20.0
#define coordAreaYMin -20.0
#define coordAreaYMax 20.0

Vector2f rocketPosition;
#define earthRadius 6371000.0
#define hiOrbit (37000000.0 + earthRadius)
#define loOrbit (160000.0 + earthRadius)
#define loopMultiplier 180


void displayFunc();
void timerFunc(int id);
void reshapeFunc(int width, int height);
void init();
void calc(double time);
void draw();

typedef struct vector {
	double x;
	double y;
} Vector;
typedef struct turning {
	double time;
	double angle;
}turning;
typedef struct node {
	turning t;
	struct node* next;
	struct node* last;
}node;
typedef struct list {
	node* head;
	node* tail;
	int size;
}list;

double countMod(Vector a);
double countAngle(Vector vec);
double normAngle(double angle);
int countQuarter(Vector a);
int findDirection(int startQ, int nextQ);
void initList(list* l);
void put(list* l, turning t);
turning parse(char * str);
void initRocket();
void printInfo();
list*  getProgram();
void initConsole();
double calcRocketMass();
void startOrbitTrace();
int orbitTrace();
node* getTeta(node* command, double time, double* teta);

/*постоянные и характеристики ракеты, зависящие от конструкции*/
double GM = 398345740000000;//масса Земли на гравитационную постоянную
double r0 = 6371000;//средний радиус Земли (м)
int workTime1 = 162, workTime2 = 397; //время работы первой и второй ступени
double absF1 = 8227000, absF2 = 981000; //модуль силы тяги первой и второй ступени в вакууме
double mFuel1 = 409500, mFuel2 = 107500; //масса топлива на старте первой и второй ступени
double mUseful = 5800 + 10000;
double mRocket, mStep1 = 22200, mStep2 = 4000;/*стартовая масса заправленой ракеты,
* масса конструкции первой и второй ступени*/

/*определение стартовой позиции*/
double deltaT = 0.1; //0.1с, временной шаг
double t = 0;//время
double teta = 0; //угол между вектором силы тяги и радиус-вектором ракеты, в радианах
//уменьшение массы за итерацию первой и второй ступени
double deltaM1, deltaM2;

//координаты ракеты
Vector F, a, g, v, r;

//программа полета
list userProgram;
list defaultProgram;
node * command;

//флаги
int count = 0;
int go = 0;
int step = 1;
bool abortFlg = false;

//проверка выхода на орбиту
double startFi;
int startQuarter;
int nextQuarter;
int result;
int direction;

double maxRMod;

int main(unsigned int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE); //включение двойной буферизации
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(200, 200);
	glutCreateWindow("Main window");

	timerFunc(100);
	glutDisplayFunc(displayFunc);
	glutReshapeFunc(reshapeFunc);

	glClearColor(0, 0, 0, 0);
	glutMainLoop();

	return 0;
};

void startOrbitTrace() {
	startFi = countAngle(r);
	startQuarter = countQuarter(r);//начальная координатная четверть
	nextQuarter = 0;
	result = 0;
};

int orbitTrace() {
	double modR;
	double fi;
	int q;

	modR = countMod(r);
	fi = countAngle(r);
	if (modR < loOrbit)
		return -1;

	if (modR > hiOrbit)
		return -2;

	q = countQuarter(r);
	if (nextQuarter == 0) {
		if (q != startQuarter) {
			nextQuarter = q;
			direction = findDirection(startQuarter, nextQuarter);
		}
	}
	else {
		if (q == startQuarter) {
			switch (direction) {
			case -1:
				if (fi < startFi)
					return 1;
				break;
			case 1:
				if (fi > startFi)
					return 1;
				break;
			default:
				printf("Error\n");
				return -1;
				break;
			}
		}
	}

	return 0;
};

node* getTeta(node* command, double time, double * teta) {
	if (command != NULL && time >= command->t.time) {
		*teta = normAngle(command->t.angle);
		return command->next;
	};

	return command;
};

double calcRocketMass()
{
	return mUseful + mStep1 + mFuel1 + mStep2 + mFuel2;
};

void initConsole() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
}

list * getProgram() {
	//задание углов поворота
	printf("Введите информацию о повороте ракеты в формате:\n\
секунда на которой изменяется угол, угол относительно направления движения в радианах.\n\
Например: 127.3, 1.28. Означает, что на 127.3 секунде ракета повернет на 1.28 радиан против часовой стрелки.\n\
Обратите внимание, что максимальное число секунд равно %d.\n\
После этого двигатели отключаются и ракета теряет управление.\n\
Чтобы закончить ввод введите q на отдельной строке.\n", workTime1 + workTime2);
	/*строчка для исправления форматирования*/

	initList(&userProgram);
	initList(&defaultProgram);

	char buf[100];
	turning turn;
	do {
		fgets(buf, 100, stdin);
		turn = parse(buf);
		if (turn.time == -2) {
			printf("Введите строку правильно: %s", buf);
		}
		else if (turn.time >= 0) {
			put(&userProgram, turn);
		}
	} while (turn.time != -1);

	if (userProgram.head == NULL)
	{
		return &defaultProgram;
	}

	return &userProgram;
};

void printInfo() {
	printf("t: %lf\n", t);
	printf("fi: %lf, teta: %lf\n", countAngle(r), teta);
	printf("X: %lf, Y: %lf, H: %lf\n", r.x, r.y, countMod(r) - r0);
	printf("Vx: %lf, Vy: %lf, V: %lf\n", v.x, v.y, countMod(v));
	printf("gx: %lf, gy: %lf, g: %lf\n", g.x, g.y, countMod(g));
	printf("Fx: %lf, Fy: %lf, F: %lf, absF: %lf\n", F.x, F.y, countMod(F), absF1);
	printf("Ax: %lf, Ay: %lf, A: %lf\n", a.x, a.y, countMod(a));
	printf("Mrocket: %lf, MFuel1: %lf, MFuel2: %lf\n\n", mRocket, mFuel1, mFuel2);
};

void initRocket(list * program) {
	deltaM1 = (mFuel1 / workTime1) * deltaT;
	deltaM2 = (mFuel2 / workTime2) * deltaT;

	//инициализация ракеты
	F = (Vector){ 0, 0 }; //вектор силы тяги
	a = (Vector){ 0, 0 }; //вектор ускорения ракеты
	g = (Vector){ 0, 0 }; //вектор ускорения свободного падения 
	v = (Vector){ 0,0 }; //вектор скорости
	r = (Vector){ r0,0 }; //радиус-вектор ракеты

	command = program->head;
	teta = 0.0;
	mRocket = calcRocketMass();
};

double countAngle(Vector vec) {
	double angle = atan2(vec.y, vec.x);
	return normAngle(angle);
}

double normAngle(double angle) {
	while (angle > 2 * M_PI) {
		angle -= 2 * M_PI;
	}
	while (angle < 0) {
		angle += 2 * M_PI;
	}
	return angle;
}

double countMod(Vector a) {
	return sqrt(a.x * a.x + a.y * a.y);
}

int countQuarter(Vector a) {
	if (a.x > 0 && a.y >= 0) {
		return 1;
	}
	if (a.x <= 0 && a.y > 0) {
		return 2;
	}
	if (a.x < 0 && a.y <= 0) {
		return 3;
	}
	if (a.x >= 0 && a.y < 0) {
		return 4;
	}
}

int findDirection(int startQ, int nextQ) {
	switch (startQ) {
	case 1: switch (nextQ) {
	case 2: return 1;
		break;
	case 4: return -1;
		break;
	default:
		return 0;
	}
		break;
	case 2:switch (nextQ) {
	case 3: return 1;
		break;
	case 1: return -1;
		break;
	default:
		return 0;
	}
		break;
	case 3:switch (nextQ) {
	case 4: return 1;
		break;
	case 2: return -1;
		break;
	default:
		return 0;
	}
		break;
	case 4:switch (nextQ) {
	case 1: return 1;
		break;
	case 3: return -1;
		break;
	default:
		return 0;
	}
		break;
	default:
		return 0;
	}
}

turning parse(char* str) {
	int j = 0;
	turning t=(turning){0,0};
	int num = 0, size = 0;
	int comma = 0; /*0-заполняем t, 1-заполняем a*/
	int dot = 0; /*0-еще не встретили в этом числе точку, 1-встретили*/
	int flag = 0;/*0-начало, 1-цифра, 2-точка, 3-запятая, 4-пробел*/
	while (str[j] != '\n') {
		if (str[j] >= '0' && str[j] <= '9') {
			if (num != 0 && flag == 4) {
				t.time = -2;
				return t;
			}
			else {
				num = num * 10 + str[j] - '0';
				if (dot) {
					size++;
				}
			}
			flag = 1;
		}
		else if (str[j] == '.') {
			if ((flag == 0 || flag == 1) && dot == 0) {
				dot = 1;
			}
			else {
				t.time = -2;
				return t;
			}
			flag = 2;
		}
		else if (str[j] == ',') {
			if (comma >= 1) {
				t.time = -2;
				return t;
			}
			if (flag != 1 && flag != 4) {
				t.time = -2;
				return t;
			}
			t.time = (double)num;
			for (int k = 0; k < size; k++) { t.time = t.time * 0.1; }
			comma = 1;
			dot = 0;
			flag = 3;
			num = 0;
			size = 0;
		}
		else if (str[j] == 'q') {
			if (flag == 0 || flag == 4) {
				t.time = -1;
				return t;
			}
			else {
				t.time = -2;
				return t;
			}
		}
		else if (str[j] == ' ') {
			flag = 4;
		}
		else {
			t.time = -2;
			return t;
		}
		j++;
	}
	if ((flag == 1 || flag == 4) && comma==1) {
		t.angle = (double)num;
		for (int k = 0; k < size; k++) { t.angle = t.angle * 0.1; }
		t.angle = (t.angle / 180) * M_PI;
	}
	else {
		t.time = -2;
		return t;
	}
	return t;
}

void initList(list* l) {
	l->head = NULL;
	l->tail = NULL;
	l->size = 0;
}

void put(list* l, turning t) {
	node* n = (node*)malloc(sizeof(node));
	if (n == NULL)
		return;

	n->t = t;
	n->next = NULL;
	n->last = NULL;
	if (l->head == NULL) {
		l->head = n;
		l->tail = n;
	}
	else {
		int f = 0;
		node* prev, * cur;
		cur = l->tail;
		while (cur != NULL && n->t.time < cur->t.time) {
			cur = cur->last;
		}
		if (cur != NULL && n->t.time == cur->t.time) {
			cur->t = n->t;
		}
		else {
			if (cur == NULL) { //вставка в начало
				l->head->last = n;
				n->next = l->head;
				l->head = n;
			}
			else if (cur->next == NULL) {//вставка в конец
				cur->next = n;
				n->last = cur;
				l->tail = n;
			}
			else { //вставка в середину
				prev = cur->last;
				n->last = prev;
				n->next = cur;
				prev->next = n;
				cur->last = n;
			}
		}
	}
	l->size++;
};

void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);
	draw(); //орисовка обьектов
	glutSwapBuffers(); //подмена буферов отрисовки для двойной буферизации
};

void timerFunc(int id)
{
	if (id == 100) {
		init();
	}
	else {
		calc(timerDelta * 0.001);
	}
	glutTimerFunc(timerDelta, timerFunc, 0);
	glutPostRedisplay();
};

void reshapeFunc(int width, int height)
{
	if (height == 0) height = 1;
	float ratio = (float)width / (float)height; //вычисляем отношение ширины к высоте окна

	glViewport(0, 0, width, height); //устанавливаем область окна в которую будет рисовать opengl
	
	//устанавливаем координатную сетку вершин
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(coordAreaXMin * ratio, coordAreaXMax * ratio,
		coordAreaYMin, coordAreaYMax, -10, 10);
};


void init()
{
	initConsole();

	initList(&defaultProgram);

	put(&defaultProgram, (turning){ 20.0,	  00.0 * (M_PI/180)});
	put(&defaultProgram, (turning){ 30.0,	  00.0 * (M_PI/180)});
	put(&defaultProgram, (turning){ 40.0,	  10.0 * (M_PI/180)});
	put(&defaultProgram, (turning){ 50.0,	  10.0 * (M_PI/180)});
	put(&defaultProgram, (turning){ 55.0,	  10.0 * (M_PI/180)});
	put(&defaultProgram, (turning){ 60.0,	  15.0 * (M_PI/180)});
	put(&defaultProgram, (turning){ 70.0,	  15.0 * (M_PI/180)});
	put(&defaultProgram, (turning){ 80.0,	  25.0 * (M_PI/180)});
	put(&defaultProgram, (turning){ 180.0, 	  45.0 * (M_PI/180)});
	put(&defaultProgram, (turning){ 190.0, 	  45.0 * (M_PI/180)});
	put(&defaultProgram, (turning){ 200.0, 	  70.0 * (M_PI/180)});
	put(&defaultProgram, (turning){ 250.0, 	  90.0 * (M_PI/180)});
	put(&defaultProgram, (turning){ 300.0, 	  90.0 * (M_PI/180)});
	put(&defaultProgram, (turning){ 400.0, 	  99.0 * (M_PI/180)});
	put(&defaultProgram, (turning){ 500.0, 	  90.0 * (M_PI/180)});
		
	initRocket(&defaultProgram);
	printInfo();
	rocketPosition = vector2f(0.0, 0.0);
	maxRMod = loOrbit;
};

void calc(double cycleTime) {
	double fi;
	int i;
	node * nextComm;

	for (i = 0; i < loopMultiplier; i++) {
		if (abortFlg)break;

		t += deltaT;
		fi = countAngle(r);
		nextComm = getTeta(command, t, &teta); 
		if(nextComm != command) {
			printInfo();
		};
		command = nextComm;

		//до взлета
		if (go == 0) {
			//силы тяги не достаточно для взлета
			if (((teta < M_PI / 2) || (teta > 3 * M_PI / 2)) && (mFuel1 > 0)) {
				if (absF1 * cos(teta) > mRocket * (GM / (r.x * r.x + r.y * r.y)) * cos(fi)) {
					go = 1;

					printf("Go\n");
					g.x = (GM / (r.x * r.x + r.y * r.y)) * cos(fi + M_PI);
					g.y = (GM / (r.x * r.x + r.y * r.y)) * sin(fi + M_PI);
					//сила тяги
					F.x = absF1 * cos(teta + fi);
					F.y = absF1 * sin(teta + fi);
					//ускорение ракеты
					a.x = g.x + F.x / mRocket;
					a.y = g.y + F.y / mRocket;
				}
			}

			mFuel1 -= deltaM1;
			if (mFuel1 <= 0 && go == 0)
			{
				mFuel1 = 0;
				printf("End step1\n");

				//Завершаем вычисления
				printf("fail launch!\n");
				abortFlg = true;
			}

		}
		else {
			if (step == 1) { //первая ступень
				F.x = absF1 * cos(teta + fi);
				F.y = absF1 * sin(teta + fi);
				mFuel1 -= deltaM1;
				if (mFuel1 <= 0)
				{
					mFuel1 = 0;
					mStep1 = 0;
					printf("End step1\n");
					step = 2;
					printInfo();
				}
			}
			else if (step == 2) { //вторая ступень
				F.x = absF2 * cos(teta + fi);
				F.y = absF2 * sin(teta + fi);
				mFuel2 -= deltaM2;
				if (mFuel2 <= 0)
				{
					mFuel2 = 0;
					mStep2 = 0;
					F = (Vector){ 0.0, 0.0 };
					printf("End step2\n");
					step = 3;
					printInfo();
					startOrbitTrace();
				}
			}
			else {

				//проверка выхода на орбиту
				result = orbitTrace();
				switch (result) {
				case 1:
					printf("Ракета успешно облетела вокруг Земли\n");
					startOrbitTrace();
					//abortFlg = true;
					break;
				case -2:
					printf("Ракета покинула околоземную орбиту и улетела в далекий космос\n");
					abortFlg = true;
					break;
				case -1:
					printf("Ракета опустилась нижее устойчивой орбиты и потерпела крушение\n");
					abortFlg = true;
					break;
				default:
					break;
				}
			}

			r.x += v.x * deltaT + a.x * deltaT * deltaT / 2;
			r.y += v.y * deltaT + a.y * deltaT * deltaT / 2;
			v.x += a.x * deltaT;
			v.y += a.y * deltaT;
			g.x = (GM / (r.x * r.x + r.y * r.y)) * cos(fi + M_PI);
			g.y = (GM / (r.x * r.x + r.y * r.y)) * sin(fi + M_PI);
			a.x = g.x + F.x / mRocket;
			a.y = g.y + F.y / mRocket;


		}

		mRocket = calcRocketMass();
		double rMod;
		rMod = countMod(r);
		if(rMod > maxRMod)
			maxRMod = rMod;

		if (rMod < r0) {
			abortFlg = true;
			printf("столкновение с землей!\n");
			printInfo();
		}
	}

	rocketPosition = vector2f(r.x, r.y);
};


void drawCircle(const Vector2f position, float radius, unsigned int segments,
	bool filled)
{
	unsigned int i;
	float alfa, delta;
	Vector2f vertex;

	if (segments < 8) segments = 8;
	delta = (2.0 * M_PI / (float)segments);

	if (filled) {
		glBegin(GL_TRIANGLE_FAN);
		vertex = position;
		glVertex3f(vertex.x, vertex.y, 0.0);

		for (i = 0, alfa = 0.0; i <= segments; i++) {
			vertex = vadd2f(
				vrot2fa(vector2f(radius, 0.0), alfa),
				position);

			glVertex3f(vertex.x, vertex.y, 0.0);
			alfa += delta;
		};
		glEnd();
	}
	else {
		glBegin(GL_LINE_LOOP);
		for (i = 0, alfa = 0.0; i < segments; i++) {
			vertex = vadd2f(
				vrot2fa(vector2f(radius, 0.0), alfa),
				position);

			glVertex3f(vertex.x, vertex.y, 0.0);
			alfa += delta;
		};
		glEnd();
	}

};

void drawTriangle(const Vector2f position, float radius) {
	unsigned int i;
	float alfa;
	Vector2f vertex;

	glBegin(GL_TRIANGLES);
	for (i = 0, alfa = M_PI / 2.0; i < 3; i++) {
		vertex = vadd2f(vrot2fa(vector2f(radius, 0.0), alfa),
			position);
		glVertex3f(vertex.x, vertex.y, 0.0);
		alfa += (2.0 * M_PI / 3.0);
	};
	glEnd();
};

#include <string.h>

void drawString(const Vector2f position, const char * const str){
	size_t strLen;
	unsigned int i;
	const char * pch;

	strLen = strlen(str);
	glRasterPos2f(position.x, position.y);
	for(i = 0, pch = str; i < strLen; i++)
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *(pch++));

};

void drawInfo(const Vector2f position){
	static char buffer[256];
	size_t strLen;
	unsigned int i;
	float lineGap = 1.5;

	double rMod;
	Vector2f putPos;

	putPos = position;
	
	putPos.y -= lineGap;
	sprintf(buffer, "time %.2f h", t / 3600.0); 
	drawString(putPos, buffer);

	putPos.y -= lineGap;
	rMod = countMod(r);
	sprintf(buffer, "height %.2f km", (rMod - r0)/1000.0 ); 
	drawString(putPos, buffer);

	putPos.y -= lineGap;
	sprintf(buffer, "max height %.2f km", (maxRMod - r0)/1000.0); 
	drawString(putPos, buffer);
};

//use graphic library color macro
#define useGLC_LRed() glColor3f(0.9, 0.0, 0.0)
#define useGLC_LGreen() glColor3f(0.0, 0.9, 0.0)
#define useGLC_Green() glColor3f(0.0, 0.2, 0.0)
#define useGLC_Blue() glColor3f(0.0, 0.0, 0.2)
#define useGLC_LYellow() glColor3f(0.8, 0.8, 0.0)
#define useGLC_White() glColor3f(1.0, 1.0, 1.0)
#define useGLC_LGray() glColor3f(0.8, 0.8, 0.8)
#define useGLC_Gray() glColor3f(0.5, 0.5, 0.5)

void draw() {
	/* коэфициент перевода реальных координат в рисуемые */
	double scale = coordAreaYMax / (maxRMod + maxRMod * 0.1); 

	//рисуем землю
	useGLC_Green();
	drawCircle(vector2f(0.0, 0.0), scale * earthRadius, 32, true);

	//рисуем орбиты
	useGLC_LRed();
	drawCircle(vector2f(0.0, 0.0), scale * hiOrbit, 32, false);
	drawCircle(vector2f(0.0, 0.0), scale * loOrbit, 32, false);

	useGLC_LYellow();
	drawCircle(vector2f(0.0, 0.0), scale * maxRMod, 32, false);

	//рисуем ракету
	useGLC_LRed();
	drawTriangle(vmul2fs(rocketPosition, scale), 1);

	useGLC_LYellow();
	drawInfo(vector2f(-coordAreaYMax + 0.5, coordAreaYMax));	

};
