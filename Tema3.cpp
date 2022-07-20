#define _USE_MATH_DEFINES
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "glut.h"
using namespace std;


double pi = M_PI;
unsigned char prevKey;


class CartesianGrid {
public:
	int nr_linii;
	int nr_coloane;
	double x_origin; //
	double y_origin; // x_origin si y_origin dau, la inceput, punctul din stanga jos
	double lungime; //marime pe coloana a unui patratel
	double latime; //marime pe linie a unui patratel
	double eps; //distanta de la canvas la patratul mare
	double raza; //raza punctului(cercului)

	CartesianGrid(int nr_linii, int nr_coloane) {
		this->eps = 0.1;
		this->x_origin = -1 + this->eps;
		this->y_origin = -1 + this->eps;
		this->nr_linii = nr_linii;
		this->nr_coloane = nr_coloane;
		this->lungime = (2 - 2 * this->eps) / (nr_coloane - 1);
		this->raza = this->lungime / 3;
		this->latime = (2 - 2 * this->eps) / (nr_linii - 1);

		this->deseneaza_grila();
	}

	void deseneaza_grila() { //desenarea liniilor negre
		double x = this->x_origin;
		double y = this->y_origin;

		glColor3f(0.0, 0.0, 0.0);
		glBegin(GL_LINES);

		for (int iterator = 1; iterator <= this->nr_coloane; iterator++) {
			glVertex2f(x, this->y_origin);
			glVertex2f(x, 1 - this->eps);
			x += this->lungime;
		}

		for (int iterator = 1; iterator <= this->nr_linii; iterator++) {
			glVertex2f(this->x_origin, y);
			glVertex2f(-this->x_origin, y);
			y += this->latime;
		}
		glEnd();
	}

	void drawSmallCircle(double valueX, double valueY, int numberOfSegments) {//deseneaza cercurile
		glColor3ub(100, 100, 100);
		glBegin(GL_TRIANGLE_FAN);
		glVertex2f(valueX, valueY);

		for (int iterator = 0; iterator < numberOfSegments; iterator++) {
			//link explicatie circle using triangle_fan -> https://www.youtube.com/watch?v=YDCSKlFqpaU
			float x_aux = valueX + (this->raza * cos(iterator * 2 * pi / numberOfSegments));
			float y_aux = valueY + (this->raza * sin(iterator * 2 * pi / numberOfSegments));
			glVertex2f(x_aux, y_aux);
		}
		glEnd();
	}


	void writePixel(int x, int y) {
		if (x >= 0 && x < nr_coloane && y >= 0 && y < nr_linii) {
			double x_pixel = this->x_origin + x * this->lungime;
			double y_pixel = this->y_origin + y * this->latime;
			drawSmallCircle(x_pixel, y_pixel, 10000);
		}
	}

	void ScanConvertSegments3(int x0, int y0, int xn, int yn, bool multiple_segments = false) {
		double dx = abs(xn - x0);//distanta pe care se poate desena
		double dy = abs(yn - y0);

		int d = 2 * dy - dx; //daca e pozitiv, e mai sus latime dreapta, daca e negativ, e mai jos latime dreapta 
		int dE = 2 * dy; //pentru mutarea la Est
		int dNE = 2 * (dy - dx); // pentru mutarea la Nord-Est
		int x = x0, y = y0;


		if (y < this->nr_linii) {
			this->writePixel(x, y);
			if (multiple_segments)
			{
				this->writePixel(x, y + 1);
				this->writePixel(x, y - 1);
			}
		}

		while (x < xn) {
			if (d <= 0) {
				d += dE;
				x++;
			}
			else {
				d += dNE;
				x++;
				if (x0 < xn && y0 < yn) {
					y++;
				}
				else {
					y--;
				}
			}
			if (y < this->nr_linii) {
				this->writePixel(x, y);
				if (multiple_segments)
				{
					this->writePixel(x, y + 1);
					this->writePixel(x, y - 1);
				}
			}
		}
	}

	void drawRedLine(int x0, int y0, int xn, int yn) {//desenarea segmentului rosu
		glColor3f(1.0, 0.1, 0.1);
		glBegin(GL_LINES);
		
		//punct initial
		double x1 = this->x_origin + x0 * this->lungime; 
		double y1 = this->y_origin + y0 * this->latime;
		glVertex2f(x1, y1);

		//punct final
		x1 = this->x_origin + xn * this->lungime;
		y1 = this->y_origin + yn * this->latime;
		glVertex2f(x1, y1);
		
		glEnd();
	}
};

void Init(void) {
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glLineWidth(1);
	glPointSize(4);
	glPolygonMode(GL_FRONT, GL_LINE);
}

void display1() {
	int nr_linii = 16;
	int nr_coloane = 16;
	CartesianGrid* cartesianGrid = new CartesianGrid(nr_linii, nr_coloane);

	//linia de sus
	cartesianGrid->ScanConvertSegments3(0, 15, 15, 10, true);
	cartesianGrid->drawRedLine(0, 15, 15, 10); //stanga sus - dreapta jos - prima linie rosie

	//linia de jos
	cartesianGrid->ScanConvertSegments3(0, 0, 15, 7);
	cartesianGrid->drawRedLine(0, 0, 15, 7);
}

void Display(void) {
	glClear(GL_COLOR_BUFFER_BIT);
	switch (prevKey) {
	case '1':
		display1();
	default:
		break;
	}
	glFlush();
}

void Reshape(int w, int h) {
	printf("Call Reshape : latime = %d, inaltime = %d\n", w, h);
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
}

void KeyboardFunc(unsigned char key, int x, int y) {
	printf("Ati tastat <%c>. Mouse-ul este in pozitia %d, %d.\n", key, x, y);
	prevKey = key;
	if (key == 27)
		exit(0);

	glutPostRedisplay();
}

void MouseFunc(int button, int state, int x, int y) {
	printf("Call MouseFunc : ati %s butonul %s in pozitia %d %d\n",
		(state == GLUT_DOWN) ? "apasat" : "eliberat",
		(button == GLUT_LEFT_BUTTON) ?
		"stang" :
		((button == GLUT_RIGHT_BUTTON) ? "drept" : "mijlociu"),
		x, y);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(300, 300);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

	glutCreateWindow(argv[0]);

	Init();
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(KeyboardFunc);
	glutMouseFunc(MouseFunc);
	glutDisplayFunc(Display);
	glutMainLoop();

	return 0;
}