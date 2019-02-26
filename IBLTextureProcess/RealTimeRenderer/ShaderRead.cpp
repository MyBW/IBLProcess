#include "ShaderRead.h"
#include "glew.h"
#include "glut.h"




int BWReadShaderSource(GLuint shader, const GLchar *filename, const std::vector<std::string>& PreComplies)
{

	FILE *file;
	const GLchar *source;
	GLsizei fileSize;
	int ret;
	file = fopen(filename, "rb");
	if (file == NULL)
	{
		cout << "open " << filename << " fial......" << endl;
		perror(filename);
		return -1;
	}
	fseek(file, 0L, SEEK_END);
	fileSize = ftell(file);
	fseek(file, 0L, SEEK_SET);
	source = new GLchar[fileSize];
	if (source == NULL) {
		cout << "there is no enough memery !" << endl;
		return  -1;
	}

	ret = fread((void*)source, sizeof(GLchar), fileSize, file) != fileSize;
	//source[fileSize] = '\0' ;

	fclose(file);
	if (ret)
	{
		cout << "can not read shader file !" << endl;
		return  -1;
	}
	int PreComplieSize = 0;
	for (auto PreComplie : PreComplies)
	{
		PreComplieSize += PreComplie.size();
		//PreComplieSize += 2;
	}
	GLchar *PreComplieData = new GLchar[PreComplieSize];
	int i = 0;
	for (auto PreComplie : PreComplies)
	{
		for (int j = 0 ; j < PreComplie.size(); j++)
		{
			PreComplieData[i] = PreComplie[j];
			i++;
		}
		//PreComplieData[i] = '\r'; i++;
		//PreComplieData[i] = '\n'; i++;
	}
	GLchar *FinalSource = new GLchar[PreComplieSize + fileSize];
	i = 0;
	for (;i <PreComplieSize; i++)
	{
		FinalSource[i] = PreComplieData[i];
	}
	for (; i < PreComplieSize + fileSize; i++)
	{
		FinalSource[i] = source[i - PreComplieSize];
	}
#ifdef SHOW_SHADER_CONTENT_
	cout << "--------------shader file name is :" << filename << "----------------------------" << endl;
	cout << FinalSource << endl;
	cout << "--------------------------------------------------------------------------" << endl << endl;
#endif
	int finalsize = fileSize + PreComplieSize;
	glShaderSource(shader, 1, (const GLchar**)(&FinalSource), &finalsize);
	

	/*
	const char *PSourceFile = SourceFile.c_str();
	const char *PSourceFile = SourceFile.c_str();
	glShaderSource(shader, 1, (const GLchar**)(&PSourceFile), &fileSize);*/
	GLenum tmpEnum = glGetError();

	delete[] source;
	return ret;
}
void PrintShaderInfoLog(GLuint shaderObj)
{
	GLsizei bufSize;
	GLchar *info;
	GLsizei length;
	glGetShaderiv(shaderObj, GL_INFO_LOG_LENGTH, &bufSize);
	if (bufSize > 0) {
		info = new GLchar[bufSize];
		if (info != NULL)
		{
			glGetShaderInfoLog(shaderObj, bufSize, &length, info);
			cout << info << endl;
			delete[] info;
		}
		else
		{
			cout << "there is not have many memory to allocate !!" << endl;
		}
	}
}
void PrintProgramInfoLog(GLuint glslProgram)
{
	GLsizei bufsize;
	GLchar* infoLog;
	GLsizei length;
	glGetProgramiv(glslProgram, GL_INFO_LOG_LENGTH, &bufsize);
	if (bufsize > 0) {
		infoLog = new GLchar[bufsize];
		if (infoLog != NULL)
		{
			glGetProgramInfoLog(glslProgram, bufsize, &length, infoLog);
			cout << infoLog << endl;
			delete[] infoLog;
		}
		else
		{
			cout << "there is not have many memory to allocate !!" << endl;
		}
	}
}
GLuint BWLoadShaders(SHADERINFO * shaderInfo, int shaderPrgmNum)
{
	if (!shaderInfo) {
		return  0;
	}
	GLuint shaderObj;
	GLuint glslProgram;
	glewInit();
	glslProgram = glCreateProgram();
	for (int i = 0; i < shaderPrgmNum; i++)
	{
		shaderObj = glCreateShader(shaderInfo[i].shaderType);
		std::vector<string> PreComplie;
		BWReadShaderSource(shaderObj, shaderInfo[i].name.c_str(), PreComplie);
		glCompileShader(shaderObj);
		PrintShaderInfoLog(shaderObj);
		glAttachShader(glslProgram, shaderObj);
	}
	glLinkProgram(glslProgram);
	PrintProgramInfoLog(glslProgram);
	return glslProgram;
}
GLuint BWLoadShadersDontLink(SHADERINFO * shaderInfo, int shaderPrgmNum)
{
	if (!shaderInfo) {
		return  0;
	}
	GLuint shaderObj;
	GLuint glslProgram;
	glewInit();
	glslProgram = glCreateProgram();
	for (int i = 0; i < shaderPrgmNum; i++)
	{
		shaderObj = glCreateShader(shaderInfo[i].shaderType);
		std::vector<string> PreComplie;
		BWReadShaderSource(shaderObj, shaderInfo[i].name.c_str(), PreComplie);
		glCompileShader(shaderObj);
		PrintShaderInfoLog(shaderObj);
		glAttachShader(glslProgram, shaderObj);
	}
	return glslProgram;
}
void DeleteProgram(GLuint program)
{
	glDeleteProgram(program);
}

