#include "FileLoader.h"


using namespace std;

FileLoader::FileLoader(LoadedObject *obj, string *fileName) {
	this->fileName = fileName;
	//Initialize our object
	this->object = obj;
	if(OpenFile()) {
		LoadFile();
	}

}

boolean FileLoader::OpenFile() {
	inFile.open(*fileName);
	if(inFile.fail()) {
		return false;
	} else {
		return true;
	}
}

void FileLoader::LoadFile() {
	/* Find how many Vertice and Elements we will have */
	int vertCount, elemCount;

	inFile >> vertCount >> elemCount;

	/* Let our LoadedObject Class know what we have found */
	object->setVertCount(vertCount);
	object->setElemCount(elemCount);

	/* Find each Vertice and Element */
	while(inFile.eof()) {
		char type;
		inFile >> type;
		if(type == 'v') {
			vert *v = new vert;
			GLfloat v1, v2, v3, n1, n2, n3;
			inFile >> v1 >> v2 >> v3 >> n1 >> n2 >> n3;
			v->v[0] = v1;
			v->v[1] = v2;
			v->v[2] = v3;
			v->v[0] = n1;
			v->v[1] = n2;
			v->v[2] = n3;
			object->addVert(v);
		} else {
			elem *e = new elem;
			GLuint i1, i2, i3;
			inFile >> i1 >> i2 >> i3;
			e->i[0] = i1;
			e->i[1] = i2;
			e->i[2] = i3;
			object->addElem(e);
		}
		inFile.close();
	}



}