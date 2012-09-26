#include <iostream>
#include <assert.h>
#include "LoadedObject.h"

using namespace std;

LoadedObject::LoadedObject() {

}

void LoadedObject::addVert(vert *ourVert) {
	verts.push_back(*ourVert);
	printVert(ourVert);

}

void LoadedObject::addElem(elem *ourElem) {
	elems.push_back(*ourElem);
	printElem(ourElem);
}

model *LoadedObject::createModel() {
	/* Create a new object */
	model *modelobj;
	modelobj = new model;

	/* Generate our Buffers for the VBO */
	glGenBuffers (1, modelobj->vbo);
	glGenBuffers (1, modelobj->ebo);
		
	initializeVBO(modelobj);
		
	return modelobj;
}

const vector<elem> &LoadedObject::getElems() const {
	return elems;
}

const vector<vert> &LoadedObject::getVerts() const{
	return verts;
}

void LoadedObject::initializeVBO(model *obj) {
	std::cout << "Initializing VBO" << std::endl;
	
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(vert), verts.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ebo[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elems.size() * sizeof(elem)  , elems.data() , GL_STATIC_DRAW);

}



void LoadedObject::printVert(vert *ourVert) {
	cout<< endl;
	cout << "**********************************************************" << endl;
	cout << "Vertice" << endl;
	cout << "Vertice Values: " << ourVert->v[0] << " " << ourVert->v[1] << " " << ourVert->v[2] << endl;
	cout << "Normal Values: " << ourVert->n[0] << " " <<  ourVert->n[1] << " " << ourVert->n[2] << endl;
	cout << "**********************************************************" << endl;
	cout << endl;
}

void LoadedObject::printElem(elem *ourElem) {
	cout<< endl;
	cout << "**********************************************************" << endl;
	cout << "Element" << endl;
	cout << "Element Values: " << ourElem->i[0] << " " << ourElem->i[1] << " " << ourElem->i[2] << endl;
	cout << "**********************************************************" << endl;
	cout << endl; 
}

void LoadedObject::renderObject(model *obj) {
	const size_t sz = sizeof(GLfloat);

	assert(obj);

	//glDisableClientState(GL_COLOR_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glColor4f(1.0f, 0.8f, 0.8f, 1.0f);
	glBindBuffer(GL_ARRAY_BUFFER, obj->vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ebo[0]); 
	{

		glVertexPointer(3, GL_FLOAT, sizeof(vert), 0);
		glNormalPointer(GL_FLOAT, sizeof(vert), (GLvoid *) (sz * 3));

		glDrawElements(GL_TRIANGLES, elems.size() * 3  , GL_UNSIGNED_INT, 0);
	}

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    
    
}





