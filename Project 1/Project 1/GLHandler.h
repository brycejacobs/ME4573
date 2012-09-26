#ifndef GLHANDLER_H
#define GLHANDLER_H

//#include "LoadedObject.h"


//#ifdef __cplusplus
extern "C" //{
	void displayCallback();

//#endif



class GLHandler {

	private:
		LoadedObject *object;
		
	public:
		GLHandler(LoadedObject *);
		//static void initializeVBO(struct model *);
		void renderObject(struct model *);
		void displayFunction(void);
		model *createModel(void);
		void setupDisplayCallback();


};

//GLHandler *currentInstance;

// #ifdef __cplusplus
// }

// #endif

#endif 