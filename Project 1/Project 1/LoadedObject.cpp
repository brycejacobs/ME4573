#include "LoadedObject.h"

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
	if(isSafe()) {
		verts[_verticeIndexCount] = *ourVert;
		_verticeIndexCount++;
		_safetyCount++;
	}
}

void LoadedObject::addElem(struct elem *ourElem) {
	if(isSafe()) {
		elems[_elementIndexCount]  = *ourElem;
		_elementIndexCount++;
		_safetyCount++;
	}
}

void LoadedObject::setVertCount(int num) {
	_vertCount = num;
	verts = new vert[num];
}

void LoadedObject::setElemCount(int num) {
	_elemCount = num;
	elems = new elem[num];
}

bool LoadedObject::isSafe() {
	int sum =  _vertCount + _elemCount;
	if(sum == _safetyCount || _safetyCount < sum) {
		return true;
	} else {
		return false;
	}

}