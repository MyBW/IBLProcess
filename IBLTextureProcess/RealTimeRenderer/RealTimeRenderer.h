#pragma once
#include "..\Math\BWPrimitive.h"
#include "glew.h"
#include "glut.h"
#include <vector>
#include <map>
#include <functional>
#include "GLSLProgram.h"

template<typename ObjectType> 
class RealTimeRenderable;

template<typename ObjectType>
class RealTimeRenderer
{
public:
	void Init(int Width , int Height);
	void BeginFrame();
	void EndFrame();
	void SetLineColor(float R, float G, float B);
	void SetLineWidth(int Width);
	void ShowCoord();
	void UseProgram();
	void SetModelMatrix(const BWMatrix4 &ModelMatrix);
	void SetViewMatrix(const BWMatrix4 &ViewMatrix);
	void SetProjectMatrix(const BWMatrix4 &ProjectMatrix);
	void SetDrawMode(GLenum Enum);
	void DrawLine(const std::vector<BWVector3D> &Lines);
	void DrawImage(int Width, int Height, void *Data, GLenum SrcPF = GL_RGB, GLenum DestPF = GL_RGB,  GLenum PixelEleType = GL_UNSIGNED_BYTE);
	void DrawImage(GLuint TextureID);
	void SetDrawImageShaderParam(std::string &ParamName, float Data);
	void GenerateCubeMipmap(GLuint TextureID , int MipNum);

	void CreateFrameBufferWithTexture(int Width, int Height, GLenum SrcPF , GLenum DestPF, GLenum PixelEleType);
	void BindFrameBufferWithTexture();
	void ReadDataFromFrameBuffer(int StartX, int StartY, int Height, int Width, GLenum OutputPF, GLenum OutputDataType, void *Data);
	void ClearFrameBufferWithTexture();

	//void ProcessCubeMap(GLuint CubeMap, int CubemapLength, GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, int MaxRoughtnessLevel, std::function<void(int, GLenum, GLenum, int , int )> FinalProcess);
	void ProcessCubeMap(GLuint CubeMap, int CubemapLength, GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, int MaxRoughtnessLevel,
		std::function<void(GLenum , GLenum , GLenum , float , int)> BeginProcess,
		std::function<void(GLenum, GLenum, GLenum, float, int)> EndProcess,
		std::function<void(GLenum , GLenum , GLenum , float , float , int )> ForBeginProcessMipmaplevel,
		std::function<void(GLenum, GLenum, GLenum, float, float, int)> ForEndProcessMipmaplevel,
		std::function<void(int, GLenum, GLenum, int, int)> ForBeginCubeMapFace,
		std::function<void(int, GLenum, GLenum, int, int)> ForEndCubeMapFace);
	void UnrealProcessCubeMap(GLuint CubeMap, int CubemapLength, GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, int MaxRoughtnessLevel,
		std::function<void(GLenum, GLenum, GLenum, float, int)> BeginProcess,
		std::function<void(GLenum, GLenum, GLenum, float, int)> EndProcess,
		std::function<void(GLenum, GLenum, GLenum, float, int)> ForBeginProcessMipmaplevel,
		std::function<void(GLenum, GLenum, GLenum, float, int)> ForEndProcessMipmaplevel,
		std::function<void(int, GLenum, GLenum, int, int)> ForBeginCubeMapFace,
		std::function<void(int, GLenum, GLenum, int, int)> ForEndCubeMapFace);
	void CreateIrradianceCubeMap(GLuint SrcCubemap,
		std::function<void(int)> BeginProcess,
		std::function<void(int)> ProcessFace,
		std::function<void(int , int )> EndProcessFace);
	void ConverEquirectangularToCubeMap(int Width, int Height, void *Data, int CubeMapSize, GLenum SrcPF = GL_RGB, GLenum DestPF = GL_RGB, GLenum PixelEleType = GL_UNSIGNED_BYTE);
	void Draw();
	void AddDrawable(const ObjectType* InObject);
	void AddDrawable(const ObjectType* InObject, GLSLProgram *Program);
	void ShowCubeMap(GLuint CubeMap, BWMatrix4& Project, BWMatrix4 &View);
	void GenerateLUT(GLuint &LUTTextureID, int Width);
	
	int Width;
	int Height;
	BWMatrix4 ViewMatrix;
	BWMatrix4 ProjectMatrix;

	GLuint BaseShaderProgram;
	GLint PosLoc;
	GLint NormalLoc;
	GLint UVLoc;
	GLint TagentLoc;
	GLint TextureLoc;
	GLint ModelMatrixLoc;
	GLint ViewMatrixLoc;
	GLint ProjectMatrixLoc;
	GLint LineColorLoc;

	GLuint DrawImageShader;
	GLint ImageTextureLoc;
	GLint DrawImagePosLoc;
	GLint DrawImageUVLoc;
	GLint OrthographicLoc;
	GLuint ImageTex;
	GLuint DrawImageVAO;
	GLuint DrawImageTex;
	std::vector<BWVector3D> Square;
	std::map<std::string, float> DrawImageShaderParam;

	GLuint EquirectangularToCubeMapProgram;
	GLuint CubeMapTexture;
	GLuint EquirectangularToCubePos;
	GLuint EquirectangularToCubeUV;
	GLuint EquirectangularToCubeNormal;
	GLuint EquirectangularToCubeViewMatrixLoc;
	GLuint EquirectangularToCubeProjectMatrixLoc;
	GLuint EquirectangularToCubeEquirectangularSamplerLoc;
	RealTimeRenderable<ObjectType> *Cube;
	BWMatrix4 ViewMatrixs[6];
	
	GLuint CubeMapShowProgram;
	GLuint CubeMapShowPos;
	GLuint CubeMapShowUV;
	GLuint CubeMapShowNormal;
	GLuint CubeMapShowViewMatrixLoc;
	GLuint CubeMapShowProjectMatrixLoc;
	GLuint CubeMapShowModelMatrixLoc;
	GLuint CubeMapShowSamplerLoc;
	GLuint CubeMapShowRoughnessLoc;
	float CubeMapShowRoughness;
	RealTimeRenderable<ObjectType> *CubeMapShow;

	GLuint ProcessCubemapProgram;
	GLuint ProcessCubemapPos;
	GLuint ProcessCubemapUV;
	GLuint ProcessCubemapNormal;
	GLuint ProcessCubemapRoughnessLoc;
	GLuint ProcessCubemapViewMatrixLoc;
	GLuint ProcessCubemapProjectMatrixLoc;
	GLuint ProcessCubemapCubemapLoc;
	RealTimeRenderable<ObjectType> *ProcessCubemapCube;

	IrradianceCubemapGLSL IrradianceCubemapProgram;
	GenerateLUTGLSL GenertateLUTProgram;
	UnrealGenerateSpecularIBL UnrealGenerateSpecularIBLProgram;

	GLuint FrameBuffer;
	GLuint FrameBufferTexture;
	std::vector<RealTimeRenderable<ObjectType>*> DrawList;
private:
	void InitShaderProgram();
	void InitPiplineState(int Width, int Height);

};



#include "RealTimeRenderer.inl"