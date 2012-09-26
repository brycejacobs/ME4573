#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <assert.h>
#include "GLObjects.h"
//#include "LoadedObject.h"
#include "FileLoader.h"

using namespace std;

/* Prototypes */
void displayFunction();
void resize(int w, int h);
void startup();   


int      last_time;

GLdouble zoom;
GLdouble rotation_x;
GLdouble rotation_y;
GLdouble position_x;
GLdouble position_y;
GLdouble position_z;

GLdouble keyboard_dx;
GLdouble keyboard_dy;
GLdouble keyboard_dz;

int      click_button;
GLdouble click_zoom;
GLdouble click_rotation_x;
GLdouble click_rotation_y;
GLdouble click_nx;
GLdouble click_ny;
vector<vert> verts;
vector<elem> elems;
model *ourobj;
LoadedObject *object;



int main(int argc, char *argv[]) {
	cout << argc << endl;
	cout << argv[1] << endl;
	
	object = new LoadedObject();
	new FileLoader(object, argv);
	
	//GLHandler *handler = new GLHandler(object);

	verts = object->getVerts();
	elems = object->getElems();

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(640, 480);
   	
   	glutInit(&argc, argv);
	glutCreateWindow(argv[0]);

	startup();

    glutReshapeFunc(resize);
    glutDisplayFunc(displayFunction);
 	glutMainLoop();
    

	return 0;
}

void resize(int w, int h) {
	GLdouble x = 0.5 * (GLdouble) w / (GLdouble) h;
	GLdouble y = 0.5;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-x, x, -y, y, 1.0, 10.0);

	glViewport(0, 0, w, h);
}

void displayFunction() {
	//glFlush();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0, 0.0, -5.0);

	
	
	glPushMatrix();
		glDisable(GL_LIGHTING);
		object->renderObject(ourobj);
		glEnable(GL_LIGHTING);
	glPopMatrix();

	glutSwapBuffers();
	
	
}

void startup()                                                             
{                                                                              
    ourobj = object->createModel();                                                         
                                                             
    glEnable(GL_DEPTH_TEST);                                                   
    glEnable(GL_LIGHTING);                                                     
    glEnable(GL_LIGHT0);                                                     
                                                                               
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);                                      
}  




