#pragma once
template<typename ObjectType>
class RealTimeRenderer;
class GLSLProgram;
template<typename ObjectType>
class RealTimeRenderable 
{
public:
	virtual ~RealTimeRenderable();
	void InitRealTimeRenderable(const ObjectType *InObject, RealTimeRenderer<ObjectType> *RTRenderer);
	void InitRealTimeRenderable(const ObjectType *InObject, GLuint PosLoc, GLuint NormalLoc, GLuint UVLoc);
	void InitRealTimeRenderable(const ObjectType *InObject, GLSLProgram *Program);
	void GenerateBaseColor();
	void GenerateNormalMap();
	void GenerateMaterialTexture();
	void Draw(RealTimeRenderer<ObjectType> *RTRenderer);
	GLuint GetVAO() { return VAO; }
	int DataSize() { return Object->GetPosData().size() * 3; }

	const BWMatrix4& GetModleMatrix() { return Object->GetModelMatrix(); }
	const ObjectType* GetObject() { return Object; }

	const ObjectType *Object;
	GLSLProgram *Program;
	GLuint PosBuffer;
	GLuint UVBuffer;
	GLuint NormalBuffer;
	GLuint LightMapBuffer;
	GLuint IndexBuffer;
	GLuint VAO;
	GLuint TextureName;
	GLuint BaseColorTextureID;
	GLuint NormalMapTextureID;
	GLuint MaterialTextureID;
};

template<typename ObjectType>
void RealTimeRenderable<ObjectType>::GenerateMaterialTexture()
{
	if (Object->MaterialTexture == nullptr) return;

	glGenTextures(1, &MaterialTextureID);
	glBindTexture(GL_TEXTURE_2D, MaterialTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Object->MaterialTexture->Width, Object->MaterialTexture->Height, 0, GL_BGR, GL_UNSIGNED_BYTE, Object->MaterialTexture->BitData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
}

template<typename ObjectType>
void RealTimeRenderable<ObjectType>::GenerateNormalMap()
{
	if (Object->NormalTexture == nullptr) return;

	glGenTextures(1, &NormalMapTextureID);
	glBindTexture(GL_TEXTURE_2D, NormalMapTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Object->NormalTexture->Width, Object->NormalTexture->Height, 0, GL_BGR, GL_UNSIGNED_BYTE, Object->NormalTexture->BitData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
}

template<typename ObjectType>
void RealTimeRenderable<ObjectType>::GenerateBaseColor()
{
	if (Object->BaseColor == nullptr) return;
	glGenTextures(1, &BaseColorTextureID);
	glBindTexture(GL_TEXTURE_2D, BaseColorTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Object->BaseColor->Width, Object->BaseColor->Height, 0, GL_BGR, GL_UNSIGNED_BYTE, Object->BaseColor->BitData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

#include "RealTimeRenderable.inl"