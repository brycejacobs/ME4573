#ifndef SHADERLOADER_H
#define SHADERLOADER_H

#include <vector>
#include "gl.h"


class ShaderLoader {
	private:
		GLuint _program;
		GLuint *_fragment;
		GLuint *_vertex;
		std::vector<GLuint> textures;
		std::vector<GLint> textureLocations;

	public:
		ShaderLoader();
		~ShaderLoader();
		ShaderLoader(const char *, const char *);
		void initialize(const char *, const char *);
		void initializeTextures(const char *);
		void createProgram(const char *, const char *);
		GLuint getProgram();
		bool checkCompilationStatus(GLuint);
		bool checkLinkingStatus(GLuint);
		char *load(const char *);
		void loadTexture(const char*);
		void bind();
		void unbind();
		GLuint getTexture(int);
		int getTexturesSize();
		void renderTextures(std::vector<const char *>);
		void storeTextures(std::vector<GLuint>);

};	



















#endif 