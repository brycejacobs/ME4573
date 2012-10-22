#ifndef GLOBJECTS_H
#define GLOBJECTS_H

#ifdef __APPLE__
#	include <GLUT/glut.h>
#	include <OpenGL/glu.h>
#	include <OpenGL/gl.h>
#else /// Includes for Windows

//#include <Windows.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/freeglut.h>
#endif




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