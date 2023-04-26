#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <iostream>
#include <random> // for permutation randomization
#include <vector>
#include <cmath>
using namespace std;

class Vector { // essentially a struct but with constructors
	public:
		double x;
		double y;
		double z;
		double w;
		Vector() 
			: x(0), y(0), z(0), w(0) {}
		Vector(double x, double y, double z)
			: x(x), y(y), z(z), w(0) {}
		Vector(double x, double y, double z, double w)
			: x(x), y(y), z(z), w(w) {}
};

// window parameters
int MAIN_WINDOW_ID;
const unsigned int WIN_WIDTH = 1300;
const unsigned int WIN_HEIGHT = 800;
const Vector WIN_POSITION(0,0,0);

// Perlin noise vars
float NOISE_DENSITY = 0.05;
int NOISE_MAP_WIDTH = 200;
int LANDSCAPE_WIDTH = 200;
int NUMBLOCKS = 4; //4x4 grid of towers
int MAXHEIGHT = 40; //Max height of a fully white pixel in the height map
vector<vector<int>> TOWER_HEIGHTS;

vector<vector<float>> NOISE_MAP;
// a randomized list of number 0-255 (inclusive); used in original implementation
vector<unsigned char> PERMUTATIONS = {
	151,160,137,91,90,15,
	131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
	190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
	88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
	77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
	102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
	135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
	5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
	223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
	129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
	251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
	49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
	138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

// landscape vars
 enum NoiseValGetPoint {
 	UpperLeft,
 	Midpoint,
 	Average
 };

// viewing vars
double ROTATION_ANGLE = 0;
bool VIEW_LOCK = false;

// 3 helper functions for Perlin noise generation
// the "the new and improved, C(2) continuous interpolant"
float fade(float t) {
	return (t*t*t*(t* (t*6 - 15) + 10));
}
// more interpolation
float lerp(float t, float a, float b) {
	return ((a) + (t)*((b)-(a)));
}
// "to compute gradients-dot-residualvectors"; generates gradients
float grad2(int hash, float x, float y) {
    int h = hash & 7; // Convert low 3 bits of hash code
    float u = h<4 ? x : y;  // into 8 simple gradient directions,
    float v = h<4 ? y : x;  // and compute the dot product with (x,y).
    return ((h&1)? -u : u) + ((h&2)? -2.0*v : 2.0*v);
}

// original 2D Perlin noise implementation
float noise2D(float x, float y) {
	int x0_int, y0_int, x1_int, y1_int;
	float x0_frac, y0_frac, x1_frac, y1_frac;
	float s, t, nx0, nx1, n0, n1;

	x0_int = floor(x); // integer part of x
	y0_int = floor(y); // integer part of y
	x0_frac = x - x0_int; // fractional part of x
	y0_frac = y - y0_int; // fractional part of y
	x1_frac = x0_frac - 1.0f;
	y1_frac = y0_frac - 1.0f;
	x1_int = (x0_int + 1) & 255;  // wrap to 0..255
	y1_int = (y0_int + 1) & 255;  // same as doing % 255 ?? not sure
	x0_int = x0_int & 255;
	y0_int = y0_int & 255;

	t = fade(y0_frac);
	s = fade(x0_frac);
	//cout << "Permutation array accesses happening w vars " << x0_int << " " << x1_int << " " << y0_int << " " << y1_int << "\n";
	//cout << "Ok I'm gonna try doing the four permutation accesses in order line by line LOL!! 1: ";
	nx0 = grad2(PERMUTATIONS[(x0_int + PERMUTATIONS[y0_int]) % 255], x0_frac, y0_frac);
	//cout << nx0 << ", 2: ";
	nx1 = grad2(PERMUTATIONS[(x0_int + PERMUTATIONS[y1_int])%255], x0_frac, y1_frac);
	//cout << nx1 << ", 3: ";
	n0 = lerp(t, nx0, nx1);


	nx0 = grad2(PERMUTATIONS[(x1_int + PERMUTATIONS[y0_int])%255], x1_frac, y0_frac);
	//cout << nx0 << ", 4: ";
	nx1 = grad2(PERMUTATIONS[(x1_int + PERMUTATIONS[y1_int])%255], x1_frac, y1_frac);
	//cout << nx1 << ", done\n";
	n1 = lerp(t, nx0, nx1);

	return (0.507f * (lerp(s, n0, n1)));
}

void calcNoiseMap() {
	// calculate all values and store which is the highest, lowest we encounter
	NOISE_MAP.clear();
	float u = 0.0;
	float v = 0.0;
	float min = 9999;
	float max = -9999;
	NOISE_MAP.resize(NOISE_MAP_WIDTH);
	for (int i = 0; i < NOISE_MAP_WIDTH; i++) {
		vector<float> row;
		row.resize(NOISE_MAP_WIDTH);	
		for (int j = 0; j < NOISE_MAP_WIDTH; j++) {
			float noiseVal = noise2D(u,v) * 0.5 + 0.5; // normalize from 0 to 1
			//cout << "Noise val at [" << i << ", " << j << "] is " << noiseVal << "\n";
			row[j] = (noiseVal);
			if (noiseVal < min)
				min = noiseVal;
			else if (noiseVal > max)
				max = noiseVal;
			v += NOISE_DENSITY;
		}
		NOISE_MAP[i] = (row);
		u += NOISE_DENSITY;
		v = 0.0;
	}

	// we want to maximize the entire range of 0-1,
	// so convert all vals from the min-max range to 0-1
	for (int i = 0; i < NOISE_MAP_WIDTH; i++){
		for (int j = 0; j < NOISE_MAP_WIDTH; j++) {
			NOISE_MAP[i][j] = (NOISE_MAP[i][j] - min)/(max - min);
		}
	}
}

void randomizePermutations() {
	PERMUTATIONS.clear();
	for (int i = 0; i <= 255; i++) {
		PERMUTATIONS.push_back(i);
	}
	random_device random_dev;
    mt19937 generator(random_dev());
    shuffle(PERMUTATIONS.begin(), PERMUTATIONS.end(), generator);
	calcNoiseMap();
}

void drawNoiseMap() {
	for (int i = 0; i < NOISE_MAP_WIDTH; i++){
		for (int j = 0; j < NOISE_MAP_WIDTH; j++) {
			glColor3f(NOISE_MAP[i][j], NOISE_MAP[i][j], NOISE_MAP[i][j]);
			glBegin(GL_POINTS);
				glVertex2d(i, NOISE_MAP_WIDTH-j);
			glEnd();
		}
	}
}

void getHeightsFromNoiseVals(NoiseValGetPoint type) {
	TOWER_HEIGHTS.clear();
	float scale = (float)NOISE_MAP_WIDTH / (float)NUMBLOCKS;
	for (int i = 0; i < NUMBLOCKS; i++) { //Num towers to calculate the heights of = width / numtowers
		vector<int> empty;
		empty.resize(NUMBLOCKS); //Empty vector that holds #blocks heights for this row
		TOWER_HEIGHTS.push_back(empty);
		for (int j = 0; j < NUMBLOCKS; j++) {
			if (type == Average) {

			}
			else if (type == Midpoint) {

			}
			else if (type == UpperLeft) {
				int index = floor(i * scale);
				int jndex = floor(j * scale);
				TOWER_HEIGHTS[i][j] = (NOISE_MAP[index][jndex] * MAXHEIGHT);
			}
		}
	}
}

void drawPlane() {
	float scale = (float)LANDSCAPE_WIDTH / (float)NUMBLOCKS;
	glPushMatrix(); //dupe current matrix to hopefully not lose camera and also to not shift anything else but this plane I hope
	glTranslatef(-(LANDSCAPE_WIDTH/2), -20, -(LANDSCAPE_WIDTH/2));
	glColor3f(0, 0, 1);
	//glLineWidth(2);
	for (int i = 0; i < NUMBLOCKS ; i++) {
		for (int j = 0; j < NUMBLOCKS; j++) {
			glBegin(GL_LINE_STRIP);
			glVertex3f((i * scale), 0, (j * scale));
			glVertex3f((i * scale) + scale, 0, (j * scale));
			glVertex3f((i * scale) + scale, 0, (j * scale) + scale);
			glVertex3f((i * scale), 0, (j * scale) + scale);
			glVertex3f((i * scale), 0, (j * scale));
			glEnd();
		}
	}
	glPopMatrix();
}

void drawTower(int h) {

}

void drawLandscape() {
	float scale = (float)LANDSCAPE_WIDTH / (float)NUMBLOCKS;
	glPushMatrix(); //dupe current matrix to hopefully not lose camera and also to not shift anything else but this plane I hope
	glTranslatef(-(LANDSCAPE_WIDTH / 2), -20, -(LANDSCAPE_WIDTH / 2));
	//glColor3f(1, 0, 0);
	//glLineWidth(2);
	for (int i = 0; i < NUMBLOCKS; i++) {
		for (int j = 0; j < NUMBLOCKS; j++) {
			//cout << "Drawing\n";
			//glPushMatrix();
			glColor3f((float)TOWER_HEIGHTS[i][j]/MAXHEIGHT, (float)TOWER_HEIGHTS[i][j]/MAXHEIGHT, (float)TOWER_HEIGHTS[i][j]/ MAXHEIGHT);
			glBegin(GL_QUADS);
			glVertex3f((i * scale), TOWER_HEIGHTS[i][j], (j * scale));
			glVertex3f((i * scale) + scale, TOWER_HEIGHTS[i][j], (j * scale));
			glVertex3f((i * scale) + scale, TOWER_HEIGHTS[i][j],  (j * scale) + scale);
			glVertex3f((i * scale), TOWER_HEIGHTS[i][j],  (j * scale) + scale);
			//glVertex3f((i * scale), TOWER_HEIGHTS (j * scale));
			glEnd();
		}
	}
	glPopMatrix();
}

void display() {
	  // for 3D map stuff
	//glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST); //May need to set up culling stuff later

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(45.0, (GLfloat)(WIN_WIDTH) / (GLfloat)(WIN_HEIGHT), 0.1f, 500.0); //Swappin to THREE DIMENSIONS BABEY!
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (VIEW_LOCK) {
		gluLookAt(0, 450, 0, 0, 0, 0, 0, 0, -1);
	}
	else {
		gluLookAt(300, 150, 0, 0, 0, 0, 0, 1, 0);
		glRotatef(ROTATION_ANGLE, 0, 1, 0);
	}
	
	calcNoiseMap();
	getHeightsFromNoiseVals(UpperLeft);
	drawLandscape();
	//drawPlane();
	drawLandscape();

	// set up noisemap viewport	
	glMatrixMode(GL_PROJECTION);
	//glViewport(0, 0, WIN_WIDTH, WIN_HEIGHT);

	glLoadIdentity();
	glOrtho(0.0, WIN_WIDTH, 0.0, WIN_HEIGHT, -1.0, 1.0); // 2D for perlin noise map
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	drawNoiseMap();



    glutSwapBuffers();
}

void parseKeys(unsigned char key, int x, int y) {
	if (key == 27) {
			glutDestroyWindow(MAIN_WINDOW_ID);
			exit(0);
	}
	else if (key == ' ') { // changes the perlin noise map
		randomizePermutations();
	}
	else if (key == 'z') {
		NOISE_DENSITY -= 0.01;
		calcNoiseMap();
	}
	else if (key == 'x') {
		NOISE_DENSITY += 0.01;
		calcNoiseMap();
	}
	else if (key == '.') { // > key, step forward for model rotation
		ROTATION_ANGLE += 10;
	}
	else if (key == ',') { // < key, step back for model rotation
		ROTATION_ANGLE -= 10;
	}
	else if (key == '[') { // [ and ] will increase and decrease the scaling of the height map, increasing and decreasing the number of towers per row
		NUMBLOCKS /=2;
		if (NUMBLOCKS < 4) { NUMBLOCKS = 4; }
	}
	else if (key == ']') { // [ and ] will increase and decrease the scaling of the height map, increasing and decreasing the number of towers per row
		NUMBLOCKS *= 2;
		if (NUMBLOCKS > 200) { NUMBLOCKS = 200; }
	}
	else if (key == 'l') {
		VIEW_LOCK = !VIEW_LOCK;
	}

	display();
}

int main(int argc, char** argv) {
	// create window
	glutInit(&argc, argv);
	// glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // for 3D map stuff
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(WIN_POSITION.x, WIN_POSITION.y);
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);
	MAIN_WINDOW_ID = glutCreateWindow("Final Project");

	glClearColor(0.9648f, 0.9531f, 0.8476f, 1.0f); // light yellow; offwhite

	// callbacks
	glutDisplayFunc(display);
	glutKeyboardFunc(parseKeys);

	// event processing loop
	// glEnable(GL_DEPTH_TEST); // for 3d stuff
	glutMainLoop();
	return 0; // to keep compiler happy
}