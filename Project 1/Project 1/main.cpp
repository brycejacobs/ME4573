#include <iostream>
#include <string>
#include "FileLoader.h"
#include "GLHandler.h"

using namespace std;

static void display(void);
static void resize(int w, int h);

int main(int argc, char *argv[]) {
	cout << argc << endl;
	cout << argv[1] << endl;

	LoadedObject *object = new LoadedObject();
	FileLoader *loader = new FileLoader(object, argv);
	GLHandler *handler = new GLHandler(object);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(640, 480);
    glutInit(&argc, argv);

    glutCreateWindow(argv[0]);

    glutReshapeFunc(resize);
    handler->setupDisplayCallback();
 

    glutMainLoop();

	return 0;
}

static void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0, 0.0, -5.0);
}

static void resize(int w, int h) {
	GLdouble x = 0.5 * (GLdouble) w / (GLdouble) h;
	GLdouble y = 0.5;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-x, x, -y, y, 1.0, 10.0);

	glViewport(0, 0, w, h);
}
