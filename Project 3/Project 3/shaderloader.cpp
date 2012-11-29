#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "shaderloader.h"
#include "image.h"

ShaderLoader::ShaderLoader(){}

ShaderLoader::~ShaderLoader() {
	glDetachShader(_program, *_fragment);
	glDetachShader(_program, *_vertex);
    
	glDeleteShader(*_fragment);
	glDeleteShader(*_vertex);
	glDeleteProgram(_program);
}

ShaderLoader::ShaderLoader(const char *vertexShaderName, const char *fragmentShaderName) {
	createProgram(vertexShaderName, fragmentShaderName);
}

void ShaderLoader::initialize(const char *vertexShaderName, const char *fragmentShaderName) {
	createProgram(vertexShaderName, fragmentShaderName);
}

void ShaderLoader::initializeTextures(const char *name) {

			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			//loadTexture(names[i]);
			std::cout << "Loading Texture " << name << std::endl;
    		int w, h, c, b;

		    void *p = image_read(name, &w, &h, &c, &b);
		    std::cout << "w: " << w << " h: " << h << " c: " << c << " b: " << b << std::endl;
		    int i = image_internal_form(c, b);
		    std::cout << "i: " << i << std::endl;
			int e = image_external_form(c);
			std::cout << "e: " << e << std::endl;
		    int t = image_external_type(b);
		    std::cout << "t: " << t << std::endl;

		    image_flip(w, h, c, b, p);
		    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		    glTexImage2D(GL_TEXTURE_2D, 0, i, w, h, 0, e, t, p);
		    std::cout << p << std::endl;
		    free(p);

			std::cout << "Done Loading Texture " << name << std::endl;
			textures.push_back(texture);
			std::cout << "Texture: " << name << " saved." << std::endl;
		
}

void ShaderLoader::createProgram(const char *vertexShaderName, const char *fragmentShaderName) {
std::cout << "Creating new program for shaders " << vertexShaderName << " and " << fragmentShaderName << std::endl;
	GLchar *vertexText;
	GLchar *fragmentText;
	_program = glCreateProgram();
	//Handle the Fragment Shader
	{
        _fragment = new GLuint;
        std::cout << "Fragment Shader Initialized: " << *_fragment << std::endl;
        *_fragment = glCreateShader(GL_FRAGMENT_SHADER);

		fragmentText = this->load(fragmentShaderName);

        glShaderSource(*_fragment, 1, (const GLchar **) &fragmentText, 0);

		std::cout << "Compiling Fragment Shader" << std::endl;
        glCompileShader(*_fragment);

		//Check to see if the shader compiled correctly.
        if(checkCompilationStatus(*_fragment)) {
            std::cout << "Fragment Shader Compiled Properly" << std::endl;
        } else {
            std::cout << "Fragment Shader had a problem compiling" << std::endl;
        }

        glAttachShader(_program, *_fragment);

        free(fragmentText);
	}

	//Handle the Vertex Shader
    {
        _vertex = new GLuint;
        *_vertex = glCreateShader(GL_VERTEX_SHADER);
        std::cout << "Vertex Shader Initialized: " << *_vertex << std::endl;
	    vertexText = this->load(vertexShaderName);
        glShaderSource(*_vertex, 1, (const GLchar **) &vertexText, 0);

		std::cout << "Compiling Vertex Shader" << std::endl;
        glCompileShader(*_vertex);

		//Check to see if the shader compiled correctly.
        if(checkCompilationStatus(*_vertex)) {
			std::cout << "Vertex Shader Compiled Properly" << std::endl;
        } else {
			std::cout << "Vertex Shader had a problem compiling" << std::endl;
        }

	    glAttachShader(_program, *_vertex);

	    free(vertexText);
    }

	std::cout << "Linking Program" << std::endl;
	glBindAttribLocation(_program, 6, "tangent");
    glLinkProgram(_program);

	//Check to make sure our program was linked properly.
    if(checkLinkingStatus(_program)) {
		std::cout  << "Program Linked Successfully" << std::endl;
    } else {
		std::cout << "Program had a problem Linking" << std::endl;
    }
}

GLuint ShaderLoader::getProgram() {
	return _program;
}

bool ShaderLoader::checkCompilationStatus(GLuint object) {
	GLchar *p;
	GLint s, n;

	std::cout << "Checking Compilation Status of Program" << std::endl;

	glGetShaderiv(object, GL_COMPILE_STATUS, &s);
	glGetShaderiv(object, GL_INFO_LOG_LENGTH, &n);

	if ((s == 0) && (p = (GLchar *) calloc(n + 1, 1))) {
		glGetShaderInfoLog(object, n, NULL, p);
		std::cout << "ShaderLoader.cpp: OpenGL Shader Error - Compilation Failed" << std::endl;
		std::cout << p << std::endl;
		free(p);
		return false;
	}

	return true;
}

bool ShaderLoader::checkLinkingStatus(GLuint object) {
	std::cout << "Checking Linking Status of Program" << std::endl;
	GLchar *p;
	GLint s, n;

	glGetProgramiv(object, GL_LINK_STATUS, &s);
	glGetProgramiv(object, GL_INFO_LOG_LENGTH, &n);

	if ((s == 0) && (p = (GLchar *) calloc(n + 1, 1))) {
		glGetProgramInfoLog(object, n, NULL, p);
		std::cout << "ShaderLoader.cpp: OpenGL Shader Error - Linking Failed" << std::endl;
		std::cout << p << std::endl;
		free(p);
		return false;
	}
	return true;
}
char *ShaderLoader::load(const char *name) {
	FILE *fp = 0;
	void *p = 0;
	size_t n = 0;
	if ((fp = fopen(name, "rb"))) {
		if (fseek(fp, 0, SEEK_END) == 0) // Seek the end.
		if ((n = (size_t) ftell(fp))) // Tell the length.
		if (fseek(fp, 0, SEEK_SET) == 0) // Seek the beginning.
		if ((p = calloc(n + 1, 1))) // Allocate a buffer.
		fread(p, 1, n, fp); // Read the data.
		fclose(fp); // Close the file.
	}
	
	char *pointer;

	std::cout << "Returning from load for " << name << std::endl;
	pointer = (char *) p;
	std::cout << pointer << std::endl;
	return pointer;


}

void ShaderLoader::loadTexture(const char *name) {
	std::cout << "Loading Texture " << name << std::endl;
    int w, h, c, b;

    void *p = image_read(name, &w, &h, &c, &b);
    std::cout << "w: " << w << " h: " << h << " c: " << c << " b: " << b << std::endl;
    int i = image_internal_form(c, b);
    std::cout << "i: " << i << std::endl;
	int e = image_external_form(c);
	std::cout << "e: " << e << std::endl;
    int t = image_external_type(b);
    std::cout << "t: " << t << std::endl;

    image_flip(w, h, c, b, p);

    glTexImage2D(GL_TEXTURE_2D, 0, i, w, h, 0, e, t, p);
    std::cout << p << std::endl;
    free(p);

	std::cout << "Done Loading Texture " << name << std::endl;
}

void ShaderLoader::bind() {
	glUseProgram(_program);
}

void ShaderLoader::unbind() {
	glUseProgram(0);
}

GLuint ShaderLoader::getTexture(int index) {
	return textures[index];
}

int ShaderLoader::getTexturesSize() {
	return textures.size();
}

void ShaderLoader::renderTextures(std::vector<const char *> glslNames) {

	//These should be in the order that the textures were loaded.

	
	for(int i = 0; i < glslNames.size(); i++) {
		GLint location;

		location = glGetUniformLocation(this->getProgram(), glslNames[i]);
		glUniform1i(location, i);

		glActiveTexture(GL_TEXTURE0 + i);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glActiveTexture(GL_TEXTURE0 + i);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	}

}
void ShaderLoader::storeTextures(std::vector<GLuint> texs) {
	textures = texs;
}

