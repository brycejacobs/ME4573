#ifndef GLOBJECTS_H
#define GLOBJECTS_H

#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#include <OpenGL/gl.h>


struct model { 
	GLuint vbo[1];
	GLuint ebo[1];
};


struct vert { // Our Vertice
	GLfloat v[3];
	GLfloat n[3];
};


struct elem { //Our Triangles
	GLuint i[3];
};











#endif