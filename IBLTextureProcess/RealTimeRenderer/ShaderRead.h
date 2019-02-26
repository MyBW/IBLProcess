
#ifndef SHADERREAD_H_
#define SHADERREAD_H_
#include "glew.h"
#include "glut.h"

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#define SHOW_SHADER_CONTENT_
struct SHADERINFO
{
	GLuint shaderType;
	string name;
};


#define CHECK_GL_ERROR(glfunc) \
{\
	glfunc; \
	int e = glGetError(); \
if (e)\
{\
	const char *errorStr = ""; \
	switch (e) \
{\
	case GL_INVALID_ENUM:     errorStr = "GL_INVALID_ENUM"; break; \
	case GL_INVALID_VALUE:    errorStr = "GL_INVALID_VALUE"; break; \
	case GL_INVALID_OPERATION: errorStr = "GL_INVALID_OPERATION"; break; \
	case GL_INVALID_FRAMEBUFFER_OPERATION: errorStr = "GL_INVALID_FRAMEBUFFER_OPERATION"; break; \
	case GL_OUT_OF_MEMORY: errorStr = "GL_OUT_OF_MEMEORY"; break; \
	default: break; \
}\
	std::string msgBuf; \
	msgBuf += "OpenGL  Error:	"; \
	msgBuf += errorStr;\
    msgBuf +=  "\n File:"; \
	msgBuf += __FILE__; \
	msgBuf += "\n Line:"; \
	msgBuf += __LINE__; \
	msgBuf += "\n";\
	std::cout << msgBuf.c_str() << std::endl; \
}\
}



int BWReadShaderSource(GLuint shader, const GLchar *filename , const std::vector<std::string>& PreComplies);
void PrintShaderInfoLog(GLuint shaderObj);
void PrintProgramInfoLog(GLuint glslProgram);
GLuint BWLoadShaders(SHADERINFO * shaderInfo, int shaderPrgmNum);
void DeleteProgram(GLuint program);
#endif
