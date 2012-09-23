#ifndef LOADEDOBJECT_H
#define LOADEDOBJECT_H

#include "GLObjects.h"

class LoadedObject {
	private:
		int _vertCount, _elemCount;
		int _verticeIndexCount, _elementIndexCount;
		int _safetyCount; //To Make sure that we received all the Vertice and Triangles promised;



	public:
		static vert *verts;
		static elem *elems;
		static model *obj;
		LoadedObject();
		void addVert(struct vert *);
		void addElem(struct elem *);
		model *getModel();
		static elem *getElems();
		int getElemCount();
		static vert *getVerts();
		int getVertCount();
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