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

	/* Create our Vertice and Element pointers to hold new Objects */
	vert *v;
	elem *e;

	char type;
	int elemcount = 0; // For Refusing the last element before EOF is reached.

	/* See how many Vertice and Triangles we are promised */
	int vs, es;
	inFile >> vs >> es;

	/* Find each Vertice and Element */
	while(!inFile.eof()) {
		inFile >> type;

		if(type == 'v') {
			v = new vert;

			/* Read in the full Vertice */
			inFile >> v->v[0] >> v->v[1] >> v->v[2] >> v->n[0] >> v->n[1] >> v->n[2];
			
			object->addVert(v);

		} else if( type == 't' ) {
			if(elemcount < es) {
				e = new elem;
				
				/* Read in the full Element */
				inFile >> e->i[0] >> e->i[1] >> e->i[2];

				object->addElem(e);
				elemcount ++;
			}

		}

	}
	std::cout << "VERTCOUNT: " << vs << std::endl;
	std::cout << "ELEMCOUNT: " << es << std::endl;
	inFile.close();

}