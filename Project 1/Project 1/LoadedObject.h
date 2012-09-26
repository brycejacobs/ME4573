#ifndef LOADEDOBJECT_H
#define LOADEDOBJECT_H

#include <vector>
#include "GLObjects.h"


class LoadedObject {
	private:
		std::vector<vert> verts;
		std::vector<elem> elems;

	public:


		LoadedObject();
		void addVert(vert *);
		void addElem(elem *);
		model *createModel();
		const std::vector<elem> &getElems() const;
		const std::vector<vert> &getVerts() const;
		void initializeVBO(model *); 
		void printVert(vert *);
		void printElem(elem *);
		void renderObject(model *obj);



		
};








#endif