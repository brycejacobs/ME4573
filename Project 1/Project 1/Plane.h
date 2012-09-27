#ifndef PLANE_H
#define PLANE_H

#include "GLObjects.h"

class Plane {

	private:
	/* We will stick the planeobject vert in here in the future */

	public:
		GLushort *add_rect(GLushort *e, GLushort i0, GLushort i1, GLushort i2, GLushort i3);
		vert *add_vert(vert *v, GLfloat x, GLfloat z);
		planeobject *plane_create(int);
		void   plane_delete(planeobject *);
		void   plane_render(planeobject *);
		void init_verts(GLuint vbo[1], GLushort n);
		void init_elems(GLuint ebo[2], GLushort n);

};


#endif