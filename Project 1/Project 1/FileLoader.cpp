#include <iostream>
#include "FileLoader.h"


using std::fstream;
using std::string;
using std::ios;
using std::cout;
using std::endl;

FileLoader::FileLoader(LoadedObject *obj, char *argv[]) {
	fstream inFile;
	//Initialize our object
	this->object = obj;
	if(OpenFile(inFile, argv)) {
		LoadFile(inFile);
	}

}

bool FileLoader::OpenFile(fstream &inFile, char *argv[]) {
	inFile.open(argv[1], ios::in);

	if(inFile.fail()) {
		this->getBits(inFile);
		return false;
	} else {
		this->getBits(inFile);
		return true;
	}
}

void FileLoader::getBits(fstream &inFile) {
	cout << "eof bit: " << inFile.eof() << endl;
	cout << "fail bit: " << inFile.fail() << endl;
	cout << "good bit: " << inFile.good() << endl;
}

void FileLoader::LoadFile(fstream &inFile) {
	/* Find how many Vertice and Elements we will have */
	int vertCount, elemCount;

	inFile >> vertCount >> elemCount;

	/* Let our LoadedObject Class know what we have found */
	object->setVertCount(vertCount);
	object->setElemCount(elemCount);

	vert *v;
	elem *e;
	char *type;
	/* Find each Vertice and Element */
	while(!inFile.eof()) {
		type = new char;
		inFile >> type;

		if(*type == 'v') {
			v = new vert;
			inFile >> v->v[0] >> v->v[1] >> v->v[2];
			inFile >> v->n[0] >> v->n[1] >> v->n[2];

			object->addVert(v);
			this->getBits(inFile);
		} else {
			e = new elem;
			inFile >> e->i[0] >> e->i[1] >> e->i[2];

			object->addElem(e);
			this->getBits(inFile);
		}
	}
	inFile.close();
}