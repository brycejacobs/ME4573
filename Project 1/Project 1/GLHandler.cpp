#include "GLHandler.h"
#include <stdlib.h>
#include <assert.h>
#include <iostream>



GLHandler::GLHandler(LoadedObject *obj) {
	this->object = obj;
}

void GLHandler::initializeVBO(struct model *obj) {
	std::cout << "Initializing VBO" << std::endl;
	//std::cout << *object->obj->vbo[0] << std::endl;
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(LoadedObject::getVerts()), LoadedObject::getVerts(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(LoadedObject::getElems()), LoadedObject::getElems(), GL_STATIC_DRAW);

	std::cout << "DIDNT MAKE IT TOO FAR" << std::endl;
}

void GLHandler::renderObject(struct model *obj) {
	const size_t sz = sizeof(GLfloat);

	assert(obj);

	//glBindBuffer(GL_ARRAY_BUFFER, obj->vbo[0]);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ebo[0]);

	size_t i;

	glVertexPointer(3, GL_FLOAT, sz * 6, (GLvoid *) (0));
	glNormalPointer(GL_FLOAT, sz * 6, (GLvoid *) (sz * 3));

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	std::cout << "Drawing" << std::endl;
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
	std::cout << "Made it back from drawing" << std::endl;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	//glDisable(GL_TEXTURE_2D);


}

void GLHandler::displayFunction(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0.0, 0.0, -5.0);
	std::cout <<"MADE IT THIS FAR!!" << std::endl;
	model *obj;
	obj = this->createModel();
	this->renderObject(obj);

	//glDrawElements(GL_TRIANGLES, object->getElemCount() * 3, GL_UNSIGNED_INT, 0);
	glutSwapBuffers();
}


model *GLHandler::createModel(void) {
	model *obj;
	//std::cout << "MODEL INFO" << object->obj->vbo[0] << std::endl;
	if((obj = (model *) malloc(sizeof (model)))) {
		glGenBuffers (1, obj->vbo);
		glGenBuffers (1, obj->ebo);

		this->initializeVBO(obj);
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

