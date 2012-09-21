#ifndef FILELOADER_H
#define FILELOADER_H

#include <fstream>
#include <string>
#include "LoadedObject.h"

class FileLoader {
	private:
		std::ifstream inFile;
		LoadedObject *object;
		std::string *fileName;


	public:
		FileLoader(LoadedObject *, std::string *);
		boolean OpenFile(); //To perform error checking
		void LoadFile(); //Actual File
		






};







#endif