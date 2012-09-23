#include "GLHandler.h"
#include <stdlib.h>
#include <assert.h>



GLHandler::GLHandler(LoadedObject *obj) {
	this->object = obj;
}

void GLHandler::initializeVBO(LoadedObject *obj) {
	vert *verts;
	elem *elems;
	model *model;
	verts = obj->getVerts();
	elems = obj->getElems();
	model = obj->getModel();

	glBindBuffer(GL_ARRAY_BUFFER, model->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);
}

void GLHandler::renderObject(model *obj) {
	const size_t sz = sizeof(GLfloat);

	assert(obj);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);


	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ebo[0]);

	size_t i;

	glVertexPointer(3, GL_FLOAT, sz * 6, (GLvoid *) (0));
	glNormalPointer(GL_FLOAT, sz * 6, (GLvoid *) (sz * 3));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_TEXTURE_2D);


}

void GLHandler::displayFunction(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0, 0.0, -5.0);

	model *obj;
	obj = this->createModel();
	this->renderObject(obj);
}


model *GLHandler::createModel(void) {
	model *obj;
	obj = object->getModel();

	if((obj = (model *) malloc(sizeof (model)))) {
		glGenBuffers (1, obj->vbo);
		glGenBuffers (1, obj->ebo);

		initializeVBO(this->object);
	}

	return obj;
}

void displayCallback() {
		currentInstance->displayFunction();
}

void GLHandler::setupDisplayCallback() {
	::currentInstance = this;
	::glutDisplayFunc(::displayCallback);
}

