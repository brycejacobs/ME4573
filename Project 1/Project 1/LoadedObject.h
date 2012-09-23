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
		void addVert(struct vert *);
		void addElem(struct elem *);
		model *getModel();
		elem *getElems();
		vert *getVerts();
		void printVert(struct vert *);
		void printElem(struct elem *);
		void setVertCount(int);
		void setElemCount(int);

		/* @param int
		 * ADDVERT = 1;
		 * ADDELEM = 2;
		 */
		bool isSafe(int);

		
};








#endif