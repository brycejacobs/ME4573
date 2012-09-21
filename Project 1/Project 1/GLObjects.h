#ifndef GLOBJECTS_H
#define GLOBJECTS_H

#include <Windows.h>
#include <GL\glew.h>
#include <GL\GL.h>
#include <GL\glut.h>

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