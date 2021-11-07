#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <random>

#ifdef __APPLE__
#  include <OpenGL/gl.h>
#  include <OpenGL/glu.h>
#  include <GLUT/glut.h>
#else
#  include <GL/gl.h>
#  include <GL/glu.h>
#  include <GL/freeglut.h>
#endif

using namespace std;

const float csize = 5.0;
const float pi = 3.14159;

bool quad = false;
bool light = true;
bool flat = false;
bool l1 = true;
bool l2 = true;
int winid;
int iterations;
int wire = 0;
int gridsizex = 30;
int gridsizez = 30;
int newgridsizex = 30;
int newgridsizez = 30;
float mx = 0.0;
float cpos[] = {0, 0, 0};//camera position
float l1pos[] = {0, 5, 0, 1};
float l2pos[] = {(float)gridsizex, 5, (float)gridsizez, 1};
float cangle[] = {0, 0};//camera angle
float amb[] = {0.1, 0.2, 0.3, 1.0};
float dif[] = {0.8, 0.8, 0.75, 1.0};
float spec[] = {0.85, 0.85, 0.8, 1.0};
float ambh[] = {0.3, 0.0, 0.0, 1.0};
float difh[] = {0.8, 0.0, 0.0, 1.0};
float spech[] = {1.0, 0.0, 0.0, 1.0};
float thingpos[] = {0, 0};
bool thingmove[] = {false, false};

random_device r;
uniform_int_distribution<int> rd1(0, gridsizex + 1);
uniform_int_distribution<int> rd2(0, gridsizez + 1);

class Mesh{
	float verts[500][500];
	float norms[499][499][2][3];//polygon surface normals
	float vnorms[500][500][3];//vertex normals
	public:
	Mesh();
	void setnorms(){
		float u[3];
		float v[3];
		float h1, h2, h3, h4;
		for(int i = 0; i <= gridsizez; i++){
			for(int j = 0; j <= gridsizex; j++){
				u[0] = 0;
				u[1] = verts[j][i + 1] - verts[j][i];
				u[2] = 1;
				v[0] = 1;
				v[1] = verts[j + 1][i] - verts[j][i];
				v[2] = 0;
				h1 = (u[1] * v[2]) - (u[2] * v[1]);
				h2 = (u[2] * v[0]) - (u[0] * v[2]);
				h3 = (u[0] * v[1]) - (u[1] * v[0]);
				h4 = sqrt(pow(h1, 2.0) + pow(h2, 2.0) + pow(h3, 2.0));
				norms[j][i][0][0] = h1 / h4;
				norms[j][i][0][1] = h2 / h4;
				norms[j][i][0][2] = h3 / h4;
				u[0] = 0;
				u[1] = verts[j + 1][i] - verts[j + 1][i + 1];
				u[2] = -1;
				v[0] = -1;
				v[1] = verts[j][i + 1] - verts[j + 1][i + 1];
				v[2] = 0;
				h1 = (u[1] * v[2]) - (u[2] * v[1]);
				h2 = (u[2] * v[0]) - (u[0] * v[2]);
				h3 = (u[0] * v[1]) - (u[1] * v[0]);
				h4 = sqrt(pow(h1, 2.0) + pow(h2, 2.0) + pow(h3, 2.0));
				norms[j][i][1][0] = h1 / h4;
				norms[j][i][1][1] = h2 / h4;
				norms[j][i][1][2] = h3 / h4;
			}
		}
		h1 = 0;
		h2 = 0;
		h3 = 0;
		for(int i = 0; i <= gridsizex; i++){
			for(int j = 0; j <= gridsizez; j++){
				if(i > 0 && j > 0){
					h1 = norms[i - 1][j - 1][1][0];
					h2 = norms[i - 1][j - 1][1][1];
					h3 = norms[i - 1][j - 1][1][2];
				}
				if(i > 0 && j <= gridsizez){
					h1 += norms[i - 1][j][0][0];
					h2 += norms[i - 1][j][0][1];
					h3 += norms[i - 1][j][0][2];
					h1 += norms[i - 1][j][1][0];
					h2 += norms[i - 1][j][1][1];
					h3 += norms[i - 1][j][1][2];
				}
				if(i <= gridsizex && j > 0){
					h1 += norms[i][j - 1][0][0];
					h2 += norms[i][j - 1][0][1];
					h3 += norms[i][j - 1][0][2];
					h1 += norms[i][j - 1][1][0];
					h2 += norms[i][j - 1][1][1];
					h3 += norms[i][j - 1][1][2];
				}
				if(i <= gridsizex && j <= gridsizez){
					h1 += norms[i][j][0][0];
					h2 += norms[i][j][0][1];
					h3 += norms[i][j][0][2];
				}
				h4 = sqrt(pow(h1, 2.0) + pow(h2, 2.0) + pow(h3, 2.0));
				vnorms[i][j][0] = h1 / h4;
				vnorms[i][j][1] = h2 / h4;
				vnorms[i][j][2] = h3 / h4;
			}
		}
	}
	void reset(){
		for(int i = 0; i <= gridsizex; i++){
			for(int j = 0; j <= gridsizez; j++){
				verts[i][j] = 0;
			}
		}
	}
	float getvnormsx(int x, int z){
		return vnorms[x][z][0];
	}
	float getvnormsy(int x, int z){
		return vnorms[x][z][1];
	}
	float getvnormsz(int x, int z){
		return vnorms[x][z][2];
	}
	void sety(int x, int z, float y){
		verts[x][z] = y;
	}
	float gety(int x, int z){
		return verts[x][z];
	}
};

Mesh::Mesh(){
	reset();
}

Mesh m;

void circlealgorithm(){
	int x, z, l1, l2, l3, l4;
	float pd, dx, yo, u;
	for(int i = 0; i < iterations; i++){
		x = rd1(r);
		z = rd2(r);
		l1 = x - csize;
		l2 = x + csize;
		l3 = z - csize;
		l4 = z + csize;
		while(l1 < 0){
			l1++;
		}
		while(l2 > gridsizex){
			l2--;
		}
		while(l3 < 0){
			l3++;
		}
		while(l4 > gridsizez){
			l4--;
		}
		m.sety(x, z, m.gety(x, z) + 1.0);
		for(int j = l1; j <= l2; j++){
			for(int k = l3; k <= l4; k++){
				yo = m.gety(j, k);
				dx = sqrt(pow(x - j, 2.0) + pow(z - k, 2.0));
				if(!(j == x && k == z) && dx < csize - 1.0){
					pd = dx * 2.0 / csize;
					yo += 0.5 + sin(pd * pi) * 0.5;
					m.sety(j, k, yo);
				}
			}
		}
	}
	m.setnorms();
	for(int i = 0; i <= gridsizex; i++){
		for(int j = 0; j <= gridsizez; j++){
			u = m.gety(i, j);
			if(u > mx){
				mx = u;
			}
		}
	}
}

void init(void){
	float l1amb[] = {0.1, 0.2, 0.3, 1.0};
	float l1dif[] = {0.8, 0.8, 0.1, 1.0};
	float l1spec[] = {0.85, 0.85, 0.3, 1.0};
	float l2amb[] = {0.1, 0.2, 0.3, 1.0};
	float l2dif[] = {0.1, 0.1, 0.8, 1.0};
	float l2spec[] = {0.2, 0.2, 0.9, 1.0};
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 10.0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, l1amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l1dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, l1spec);
	glLightfv(GL_LIGHT1, GL_AMBIENT, l2amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, l2dif);
	glLightfv(GL_LIGHT1, GL_SPECULAR, l2spec);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, 1.0, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
}

void reshape(int w, int h){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (float)w / (float)h, 1.0, 500.0);
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, w, h);
}

void keyboard(unsigned char key, int xin, int yin){
	if(key == '0'){
		l1pos[1] = l1pos[1] + 0.5;
	}else if(key == 'p'){
		l1pos[1] = l1pos[1] - 0.5;
	}else if(key == '8'){
		l2pos[0] = l2pos[0] + 0.5;
	}else if(key == '9'){
		l2pos[0] = l2pos[0] - 0.5;
	}else if(key == 'i'){
		l2pos[2] = l2pos[2] + 0.5;
	}else if(key == 'o'){
		l2pos[2] = l2pos[2] - 0.5;
	}else if(key == '-'){
		if(l1 == false){
			glEnable(GL_LIGHT0);
			l1 = true;
		} else {
			glDisable(GL_LIGHT0);
			l1 = false;
		}
	}else if(key == '['){
		if(l2 == false){
			glEnable(GL_LIGHT1);
			l2 = true;
		} else {
			glDisable(GL_LIGHT1);
			l2 = false;
		}
	}else if(key == 'h'){
		if(wire == 0){
			wire = 1;
		}else if(wire == 1){
			wire = 2;
		} else {
			wire = 0;
		}
	}else if(key == 't'){
		quad = false;
	}else if(key == 'y'){
		quad = true;
	}else if(key == 'l'){
		if(light == false){
			glEnable(GL_LIGHTING);
			light = true;
		} else {
			glDisable(GL_LIGHTING);
			light = false;
		}
	}else if(key == 'f'){
		if(flat == false){
			glShadeModel(GL_FLAT);
			flat = true;
		} else {
			glShadeModel(GL_SMOOTH);
			flat = false;
		}
	}else if(key == 'r'){
		gridsizex = newgridsizex;
		gridsizez = newgridsizez;
		rd1 = uniform_int_distribution<int>(0, gridsizex);
		rd2 = uniform_int_distribution<int>(0, gridsizez);
		iterations = (gridsizex * gridsizez) / 10;
		m.reset();
		circlealgorithm();
	}else if(key == 'w'){
		cpos[0] = cpos[0] + (sin(cangle[1] * (pi / 180.0)) / 2);
		cpos[2] = cpos[2] + (cos(cangle[1] * (pi / 180.0)) / 2);
	}else if(key == 's'){
		cpos[0] = cpos[0] - (sin(cangle[1] * (pi / 180.0)) / 2);
		cpos[2] = cpos[2] - (cos(cangle[1] * (pi / 180.0)) / 2);
	}else if(key == 'a'){
		cpos[0] = cpos[0] + (cos(cangle[1] * (pi / 180.0)) / 2);
		cpos[2] = cpos[2] - (sin(cangle[1] * (pi / 180.0)) / 2);
	}else if(key == 'd'){
		cpos[0] = cpos[0] - (cos(cangle[1] * (pi / 180.0)) / 2);
		cpos[2] = cpos[2] + (sin(cangle[1] * (pi / 180.0)) / 2);
	}else if(key == 'q'){
		cpos[1] = cpos[1] + 0.5;
	}else if(key == 'e'){
		cpos[1] = cpos[1] - 0.5;
	}else if(key == 'b'){
		newgridsizex++;
		printf("grid x = %i\n", newgridsizex);
	}else if(key == 'v'){
		newgridsizex--;
		printf("grid x = %i\n", newgridsizex);
	}else if(key == 'm'){
		newgridsizez++;
		printf("grid z = %i\n", newgridsizez);
	}else if(key == 'n'){
		newgridsizez--;
		printf("grid z = %i\n", newgridsizez);
	}
	glutPostRedisplay();
}

void special(int key, int xin, int yin){
	if(key == GLUT_KEY_UP){
		if(cangle[0] < 90){
			cangle[0] = cangle[0] + 2;
		}
	}else if(key == GLUT_KEY_DOWN){
		if(cangle[0] > -90){
			cangle[0] = cangle[0] - 2;
		}
	}else if(key == GLUT_KEY_LEFT){
		cangle[1] = cangle[1] + 2;
	}else if(key == GLUT_KEY_RIGHT){
		cangle[1] = cangle[1] - 2;
	}
	if(cangle[1] < 0.0){
		cangle[1] += 360.0;
	}else if(cangle[1] >= 360.0){
		cangle[1] -= 360.0;
	}
	glutPostRedisplay();
}

float findy(float x, float z){//interpolate between 4 points to find height
	int x1, z1, x2, z2;
	float tx, tz;
	float y11, y12, y21, y22;
	float yx1, yx2, yz;
	float fp1, fp2, fp3;
	float p;
	x1 = (int)floor(x);
	z1 = (int)floor(z);
	x2 = (int)ceil(x);
	z2 = (int)ceil(z);
	tx = x - floor(x);//between 0 and 1
	tz = z - floor(z);
	y11 = m.gety(x1, z1);
	y12 = m.gety(x1, z2);
	y21 = m.gety(x2, z1);
	y22 = m.gety(x2, z2);
	yx1 = y21 - y11;
	yx2 = y22 - y12;
	fp1 = (tx * yx1) + y11;
	fp2 = (tx * yx2) + y12;
	yz = fp2 - fp1;
	fp3 = (tz * yz) + fp1;
	return fp3;
}

void movething(int){
	if(thingmove[0] == false){
		thingpos[0] += 0.1;
	} else {
		thingpos[0] -= 0.1;
	}
	if(thingmove[1] == false){
		thingpos[1] += 0.1;
	} else {
		thingpos[1] -= 0.1;
	}
	if(thingpos[0] <= 0.0){
		thingmove[0] = false;
	}else if(thingpos[0] >= gridsizex){
		thingmove[0] = true;
	}
	if(thingpos[1] <= 0.0){
		thingmove[1] = false;
	}else if(thingpos[1] >= gridsizez){
		thingmove[1] = true;
	}
	glutPostRedisplay();
	glutTimerFunc(1000.0/60.0, movething, 0);
}

void drawlight(){
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0, -0.5, 0.0);
	glVertex3f(0.0, 0.0, 0.5);
	glVertex3f(-0.5, 0.0, 0.0);
	glVertex3f(0.0, 0.0, -0.5);
	glVertex3f(0.5, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.5);
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0, 0.5, 0.0);
	glVertex3f(0.0, 0.0, 0.5);
	glVertex3f(0.5, 0.0, 0.0);
	glVertex3f(0.0, 0.0, -0.5);
	glVertex3f(-0.5, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.5);
	glEnd();
}

void drawthing(){
	glPushMatrix();
	glTranslatef(thingpos[0], findy(thingpos[0], thingpos[1]), thingpos[1]);
	glColor3f(1, 0, 0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambh);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, difh);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spech);
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.5, 0.5);
	glVertex3f(-0.5, 0.5, 0.0);
	glVertex3f(0.0, 0.5, -0.5);
	glVertex3f(0.5, 0.5, 0.0);
	glVertex3f(0.0, 0.5, 0.5);
	glEnd();
	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0, 2.0, 0.0);
	glVertex3f(0.0, 0.5, 0.5);
	glVertex3f(0.5, 0.5, 0.0);
	glVertex3f(0.0, 0.5, -0.5);
	glVertex3f(-0.5, 0.5, 0.0);
	glVertex3f(0.0, 0.5, 0.5);
	glEnd();
	glPopMatrix();
}

void rendertri(){
	float u, v;
	for(int i = 0; i < gridsizez; i++){
		glBegin(GL_TRIANGLE_STRIP);
		for(int j = 0; j <= gridsizex; j++){
			u = m.gety(j, i);
			v = m.gety(j, i + 1);
			glColor3f(u / mx, u / mx, u / mx);
			glNormal3f(m.getvnormsx(j, i), m.getvnormsy(j, i), m.getvnormsz(j, i));
			glVertex3f((float)j, u, (float)i);
			glColor3f(v / mx, v / mx, v / mx);
			glNormal3f(m.getvnormsx(j, i + 1), m.getvnormsy(j, i + 1), m.getvnormsz(j, i + 1));
			glVertex3f((float)j, v, (float)i + 1);
		}
		glEnd();
	}
}

void renderquad(){
	float u, v;
	for(int i = 0; i < gridsizez; i++){
		glBegin(GL_QUAD_STRIP);
		for(int j = 0; j <= gridsizex; j++){
			u = m.gety(j, i);
			v = m.gety(j, i + 1);
			glColor3f(u / mx, u / mx, u / mx);
			glNormal3f(m.getvnormsx(j, i), m.getvnormsy(j, i), m.getvnormsz(j, i));
			glVertex3f((float)j, u, (float)i);
			glColor3f(v / mx, v / mx, v / mx);
			glNormal3f(m.getvnormsx(j, i + 1), m.getvnormsy(j, i + 1), m.getvnormsz(j, i + 1));
			glVertex3f((float)j, v, (float)i + 1);
		}
		glEnd();
	}
}

void display(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPushMatrix();
	glRotatef(-cangle[0], 1, 0, 0);
	glRotatef(-cangle[1], 0, 1, 0);
	glTranslatef((-gridsizex / 2) + cpos[0], -10 + cpos[1], (-gridsizez / 2) + cpos[2]);
	glLightfv(GL_LIGHT0, GL_POSITION, l1pos);
	glPushMatrix();
	glTranslatef(l1pos[0], l1pos[1], l1pos[2]);
	drawlight();
	glPopMatrix();
	glLightfv(GL_LIGHT1, GL_POSITION, l2pos);
	glPushMatrix();
	glTranslatef(l2pos[0], l2pos[1], l2pos[2]);
	drawlight();
	glPopMatrix();
	if(wire < 2){
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if(quad == false){
			rendertri();
		} else {
			renderquad();
		}
	}
	if(wire > 0){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if(quad == false){
			rendertri();
		} else {
			renderquad();
		}
	}
	drawthing();
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, dif);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
	glPopMatrix();
	glutSwapBuffers();
}

int main(int argc, char** argv){
	iterations = (gridsizex * gridsizez) / 10;
	circlealgorithm();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	glutInitWindowPosition(50, 50);
	winid = glutCreateWindow("Terrain");
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(special);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutTimerFunc(1000.0/60.0, movething, 0);
	init();
	printf("Controls:\nWASD: move horizontally\nArrow Keys: rotate the camera\n0, p: move camera 1 along the y axis\n8, 9: move camera 2 along the x axis\ni, o: move camera 2 along the z axis\n-: toggle camera 1\n[: toggle camera 2\nf: toggle gouraud shading\nr: recreate map\nh:toggle wireframe mode (the wireframes are rendering in the second mode, they're just the same colour as the mesh)\nl: toggle lighting\nt: render with triangles\ny: render with quadrilaterals\nb, v, m, n: change the x and z size of the grid, respectively. takes effect on map reset\n");
	glutMainLoop();
	return(0);
}//Greg Kitching 400036589
