#ifndef LOADEDOBJECT_H
#define LOADEDOBJECT_H

#include "GLObjects.h"

class LoadedObject {
	private:
		model *obj;
		vert *verts;
		elem *elems;
		int _vertCount, _elemCount;
		int _verticeIndexCount, _elementIndexCount;
		int _safetyCount; //To Make sure that we received all the Vertice and Triangles promised;



	public:
		LoadedObject();
		void addVert(struct vert *ourVert);
		void addElem(struct elem *ourElem);
		void setVertCount(int);
		void setElemCount(int);
		bool isSafe();
};








#endif