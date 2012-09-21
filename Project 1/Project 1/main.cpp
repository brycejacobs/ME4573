#include <iostream>
#include <string>
#include "LoadedObject.h"
#include "FileLoader.h"

using namespace std;

int main(int argc, char *argv[]) {
	cout << argc << endl;
	cout << argv[1] << endl;
	
	string *fileName = new string[strlen(argv[1])];
	*fileName = argv[1];
	
	
	
	LoadedObject *object = new LoadedObject();
	FileLoader *loader = new FileLoader(object, fileName);



	return 0;
}
