#include <iostream>
#include "LoadedObject.h"

using std::cout;
using std::endl;

LoadedObject::LoadedObject() {
	/* Initialize all Fields */
	_vertCount = 0;
	_elemCount = 0;


	_verticeIndexCount = 0;
	_elementIndexCount = 0;
	_safetyCount = 0;

	obj = new model;

}

void LoadedObject::addVert(struct vert *ourVert) {
	if(isSafe(1)) {
		cout << "Vertice Index: " << _verticeIndexCount << endl;
		verts[_verticeIndexCount] = *ourVert;
		_verticeIndexCount++;
		_safetyCount++;
		this->printVert(ourVert);
	}
}

void LoadedObject::addElem(struct elem *ourElem) {
	if(isSafe(2)) {
		cout << "Element Count: " << _elementIndexCount << endl;
		elems[_elementIndexCount]  = *ourElem;
		_elementIndexCount++;
		_safetyCount++;
		this->printElem(ourElem);
	}
}

model *LoadedObject::getModel() {
	return this->obj;
}
elem *LoadedObject::getElems() {
	return this->elems;
}
vert *LoadedObject::getVerts() {
	return this->verts;
}

void LoadedObject::printVert(struct vert *ourVert) {
	cout<< endl;
	cout << "Vertice" << endl;
	cout << "Vertice Values: " << ourVert->v[0] << " " << ourVert->v[1] << " " << ourVert->v[2] << endl;
	cout << "Normal Values: " << ourVert->n[0] << " " <<  ourVert->n[1] << " " << ourVert->n[2] << endl;
	cout << endl;
}

void LoadedObject::printElem(struct elem *ourElem) {
	cout<< endl;
	cout << "Element" << endl;
	cout << "Element Values: " << ourElem->i[0] << " " << ourElem->i[1] << " " << ourElem->i[2] << endl;
	cout << endl; 
}


void LoadedObject::setVertCount(int num) {
	_vertCount = num;
	verts = new vert[num];
	cout << "Vertice Size: " << num << endl;
}

void LoadedObject::setElemCount(int num) {
	_elemCount = num;
	elems = new elem[num];
	cout << "Elements Size: " << num << endl;
}


bool LoadedObject::isSafe(int addCase) {
	//ADDVERT = 1
	//ADDELEM = 2
	switch(addCase) {
		case 1:
			if(_verticeIndexCount == _vertCount) {
				cout << "Refused ADD" << endl;
				cout << "We have hit our promise amount for Vertice: " << _verticeIndexCount << endl;
				return false;
			}
			return true;
		case 2:
			if(_elementIndexCount == _elemCount) { 
				cout << "Refused ADD" << endl;
				cout << "We have hit our promise amount for Elements: " << _elementIndexCount << endl;
				return false;
			}
			return true;
		default: 
			cout << "LoadedObject::isSafe() - We shouldn't be here" << endl;
			return false;
	}
}



