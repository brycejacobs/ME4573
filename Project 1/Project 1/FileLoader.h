#ifndef FILELOADER_H
#define FILELOADER_H

#include <fstream>
#include <string>
#include "LoadedObject.h"

class FileLoader {
	private:

		LoadedObject *object;
		std::string fileName;


	public:
		FileLoader(LoadedObject *, char *argv[]);
		bool OpenFile(std::fstream &, char *argv[]); //To perform error checking
		void getBits(std::fstream &);
		void LoadFile(std::fstream &); //Actual File
};







#endif