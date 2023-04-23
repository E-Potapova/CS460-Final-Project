#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <stdlib.h>
#include <cmath>
using namespace std;

class Vector { // essentially a struct but with constructors
	public:
		double x;
		double y;
		double z;
		Vector() 
			: x(0), y(0), z(0) {}
		Vector(double x, double y, double z)
			: x(x), y(y), z(z) {}
};

// initializing globals
int mainWindowID;
int width = 1300;
int height = 800;
// model rotation vars
double aboutB2 = 0.0; // whole model, except s1 and b2, spins around s1 (y-axis)
double aboutB1 = 0.0; // s4, b4 spin around b1
double aboutB3 = 0.0; // s5, b5 spin around b3
// flight sim vars
Vector viewPoint(0.0, 0.0, 40.0);
Vector u(1.0, 0.0, 0.0); // x-view
Vector v(0.0, 1.0, 0.0); // y-view; up vector
Vector n(0.0, 0.0, -1.0); //z-view

void display(); // declaration, definition below

void roll(double angle) { // Q,E
	// rotation about n axis
	angle = angle * (M_PI / 180); // convert to radians
	double cosine = cos(angle);
	double sine = sin(angle);
	Vector oldU(u.x, u.y, u.z);
	Vector oldV(v.x, v.y, v.z);
	u.x = (cosine * oldU.x) - (sine * oldV.x);
	u.y = (cosine * oldU.y) - (sine * oldV.y);
	u.z = (cosine * oldU.z) - (sine * oldV.z);
	v.x = (sine * oldU.x) + (cosine * oldV.x);
	v.y = (sine * oldU.y) + (cosine * oldV.y);
	v.z = (sine * oldU.z) + (cosine * oldV.z);
	display();
}

void pitch(double angle) { // W,S
	// rotation about u axis
	angle = angle * (M_PI / 180); // convert to radians
	double cosine = cos(angle);
	double sine = sin(angle);
	Vector oldV(v.x, v.y, v.z);
	Vector oldN(n.x, n.y, n.z);
	v.x = (cosine * oldV.x) - (sine * n.x);
	v.y = (cosine * oldV.y) - (sine * n.y);
	v.z = (cosine * oldV.z) - (sine * n.z);
	n.x = (sine * oldV.x) + (cosine * oldN.x);
	n.y = (sine * oldV.y) + (cosine * oldN.y);
	n.z = (sine * oldV.z) + (cosine * oldN.z);
	display();
}

void yaw(double angle) { // A,D
	// rotation about v axis
	angle = angle * (M_PI / 180); // convert to radians
	double cosine = cos(angle);
	double sine = sin(angle);
	Vector oldU(u.x, u.y, u.z);
	Vector oldN(n.x, n.y, n.z);
	u.x = (sine * oldN.x) + (cosine * oldU.x);
	u.y = (sine * oldN.y) + (cosine * oldU.y);
	u.z = (sine * oldN.z) + (cosine * oldU.z);
	n.x = (cosine * oldN.x) - (sine * oldU.x);
	n.y = (cosine * oldN.y) - (sine * oldU.y);
	n.z = (cosine * oldN.z) - (sine * oldU.z);
	display();
}

void slideAlongU(int distance) { // H,K
	// move sideways
	viewPoint.x += u.x * distance;
	viewPoint.y += u.y * distance;
	viewPoint.z += u.z * distance;
	display();
}

void slideAlongV(int distance) { // Y,I
	// move up, down
	viewPoint.x += v.x * distance;
	viewPoint.y += v.y * distance;
	viewPoint.z += v.z * distance;
	display();
}

void slideAlongN(int distance) { // U,J
	// move forward, backward
	viewPoint.x += n.x * distance;
	viewPoint.y += n.y * distance;
	viewPoint.z += n.z * distance;
	display();
}

void drawModel() {
	// spin whole model
	glPushMatrix();
		glRotatef(aboutB2, 0.0, 1.0, 0.0);
		// s2,s3
		glPushMatrix();
			glTranslatef(0.0, 0.0, -10.0);
			glColor3f(0.074f, 0.556f, 0.556f); // lighter dark blue-green
			gluCylinder(gluNewQuadric(), 0.8, 0.8, 20, 20, 20);
		glPopMatrix();
		// spin s5, b5 about b3
		glPushMatrix();
			glRotatef(aboutB3, 0.0, 0.0, 1.0);
			// s5
			glPushMatrix();
				glTranslatef(0.0, 0.0, -10.0);
				glRotatef(90.0, 1.0, 0.0, 0.0);
				glColor3f(0.031f, 0.423f, 0.478f); // dark blue-green
				gluCylinder(gluNewQuadric(), 0.2, 0.8, 10, 20, 20);
			glPopMatrix();
			// b5
			glPushMatrix();
				glTranslatef(0.0, -10.0, -10.0);
				glColor3f(0.701f, 0.380f, 0.807f); // lighter purple
				gluSphere(gluNewQuadric(), 2.3, 20, 20);
			glPopMatrix();
		glPopMatrix();
		// b3
		glPushMatrix();
			glTranslatef(0.0, 0.0, -10.0);
			glColor3f(0.572f, 0.270f, 0.619f); // more purple
			gluSphere(gluNewQuadric(), 1.5, 20, 20);
		glPopMatrix();
		// spin s4, b4 about b1
		glPushMatrix();
			glRotatef(aboutB1, 0.0, 0.0, 1.0);
			// s4
			glPushMatrix();
				glTranslatef(0.0, 0.0, 10.0);
				glRotatef(90.0, 1.0, 0.0, 0.0);
				glColor3f(0.031f, 0.423f, 0.478f); // dark blue-green
				gluCylinder(gluNewQuadric(), 0.2, 0.8, 10, 20, 20);
			glPopMatrix();
			// b4
			glPushMatrix();
				glTranslatef(0.0, -10.0, 10.0);
				glColor3f(0.8f, 0.309f, 0.537f); // lighter pink
				gluSphere(gluNewQuadric(), 2.3, 20, 20);
			glPopMatrix();
		glPopMatrix();
		// b1
		glPushMatrix();
			glTranslatef(0.0, 0.0, 10.0);
			glColor3f(0.666f, 0.298f, 0.470f); // more pink
			gluSphere(gluNewQuadric(), 1.5, 20, 20);
		glPopMatrix();
	glPopMatrix();
	// s1
	glPushMatrix();
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glColor3f(0.031f, 0.423f, 0.478f); // dark blue-green
		gluCylinder(gluNewQuadric(), 0.8, 0.8, 20, 20, 20);
	glPopMatrix();
	// b2
	glColor3f(0.576f, 0.282f, 0.505f); // pinkish-purple
	gluSphere(gluNewQuadric(), 1.5, 20, 20);
}

void drawPlane(int yHeight) {
	Vector planeLL(-100, yHeight, -100);
	int boxWidth = 5;
	int numBoxesInWidth = 40;
	glColor3f(0.709f, 0.404f, 0.522f); // dark pink
	for (int r = 0; r < numBoxesInWidth; r++) {
		for (int c = 0; c < numBoxesInWidth; c++) {
			glBegin(GL_LINE_STRIP);
				glVertex3f(planeLL.x+(c*boxWidth), planeLL.y, planeLL.z+(r*boxWidth));
				glVertex3f(planeLL.x+(c*boxWidth)+boxWidth, planeLL.y, planeLL.z+(r*boxWidth));
				glVertex3f(planeLL.x+(c*boxWidth)+boxWidth, planeLL.y, planeLL.z+(r*boxWidth)+boxWidth);
				glVertex3f(planeLL.x+(c*boxWidth), planeLL.y, planeLL.z+(r*boxWidth)+boxWidth);
				glVertex3f(planeLL.x+(c*boxWidth), planeLL.y, planeLL.z+(r*boxWidth));
			glEnd();
		}
	}
}

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// upper left viewport
    glViewport(0, height/2, width/2, height/2);
    glLoadIdentity();
    gluLookAt(40.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	drawPlane(-20);
    drawModel();
	// upper right viewport
    glViewport(width/2, height/2, width/2, height/2);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 40.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	drawPlane(-20);
    drawModel();
	// lower left viewport
    glViewport(0, 0, width/2, height/2);
    glLoadIdentity();
    gluLookAt(0.0, 40.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -1.0);
	drawPlane(-20);
    drawModel();
	// lower right viewport
    glViewport(width/2, 0, width/2, height/2);
    glLoadIdentity();
    gluLookAt(viewPoint.x, viewPoint.y, viewPoint.z, (viewPoint.x + n.x), (viewPoint.y + n.y), (viewPoint.z + n.z), v.x, v.y, v.z);
	drawPlane(-10);
	glColor3f(0.709f, 0.404f, 0.522f); // dark pink
    glutWireTeapot(10);

    glutSwapBuffers();
}

void parseKeys(unsigned char key, int x, int y) {
	if (key == 27) {
			glutDestroyWindow(mainWindowID);
			exit(0);
	}
	else if (key == '.') { // > key, step forward for model rotation
		aboutB2 += 10;
		if (aboutB2 >= 360) aboutB2 -= 360;
		aboutB1 += 10;
		if (aboutB1 >= 360) aboutB1 -= 360;
		aboutB3 += -10;
		if (aboutB3 >= 360) aboutB3 -= 360;
		display();
	}
	else if (key == ',') { // < key, step back for model rotation
		aboutB2 -= 10;
		if (aboutB2 <= 360) aboutB2 += 360;
		aboutB1 -= 10;
		if (aboutB1 <= 360) aboutB1 += 360;
		aboutB3 -= -10;
		if (aboutB3 <= 360) aboutB3 += 360;
		display();
	}
	else if (key == 'q') {
		roll(-10);
	}
	else if (key == 'e') {
		roll(10);
	}
	else if (key == 'a') {
		yaw(10);
	}
	else if (key == 'd') {
		yaw(-10);
	}
	else if (key == 'w') {
		pitch(10);
	}
	else if (key == 's') {
		pitch(-10);
	}
	else if (key == 'h') {
		slideAlongU(-4);
	}
	else if (key == 'k') {
		slideAlongU(4);
	}
	else if (key == 'y') {
		slideAlongV(-4);
	}
	else if (key == 'i') {
		slideAlongV(4);
	}
	else if (key == 'u') {
		slideAlongN(4);
	}
	else if (key == 'j') {
		slideAlongN(-4);
	}
	
}

int main(int argc, char** argv) {
	// create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 80);
	glutInitWindowSize(width, height);
	mainWindowID = glutCreateWindow("Assignment 3");

	// set up window properties
	glClearColor(0.9648f, 0.9531f, 0.8476f, 1.0f); // light yellow; offwhite
	glShadeModel(GL_FLAT); // wireframe?
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(55, (double(width)/height), 2, 100.0);
    glMatrixMode(GL_MODELVIEW);

	// callbacks
	glutDisplayFunc(display);
	glutKeyboardFunc(parseKeys);

	// event processing loop
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
	return 0; // to keep compiler happy
}