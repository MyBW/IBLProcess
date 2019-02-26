#pragma once
#include <string>
#include "glew.h"
#include <vector>
#include "..\math\bwprimitive.h"
#include <map>
#include "RealTimeRenderable.h"
#include "..\Scene\Object.h"
class GLSLProgram
{
public:
	void LoadProgram(std::string VertexShader, std::string FragShader);
	virtual void InitParam(); 
	std::vector<std::string> ShaderPrecomplie;
	GLuint ProgramID;
	GLuint PositionLoc;
	GLuint UVLoc;
	GLuint NormalLoc;
	GLint ModelMatrixLoc;
	GLint ViewMatrixLoc;
	GLint ProjectMatrixLoc;
};

class BaseShowGLSL : public GLSLProgram
{
public:
	void InitParam();
	GLint TagentLoc;
	GLint TextureLoc;
	GLint LineColorLoc;
};

class DrawImageGLSL : public GLSLProgram
{
public:
	void InitParam();
	GLint ImageTextureLoc;
	GLint OrthographicLoc;
	GLuint ImageTex;
	GLuint DrawImageVAO;
	GLuint DrawImageTex;
	std::vector<BWVector3D> Square;
	std::map<std::string, float> DrawImageShaderParam;
};

class IrradianceCubemapGLSL : public GLSLProgram
{
public:
	void InitParam();
	GLuint SrcCubemap;
};


class PBRGLSL : public GLSLProgram
{
public:
	void SetIsHaveBaseColorTexture(bool IsHave);
	void SetIsHaveNormalmapTexture(bool IsHave);
	void SetIsHaveMaterialTexture(bool IsHave);
	void InitParam();
	template<typename Renderable, typename Renderer>
	void Draw(Renderable *Obj, Renderer *Render);
	BWVector3D ViewPos;
	BWVector3D LightPos;
	BWVector3D LightColor;
	float Exposure;
	int CubemapMaxMip;

	GLuint IBLTextureID;
	GLuint LUTTextureID;
	GLuint IBLIrradianceTextureID;

	GLuint IBL_Specular_Light_Loc;
	GLuint IBLIrradianceLoc;
	GLuint IBL_LUT_Loc;
	GLuint CubemapMaxMipLoc;
	GLuint ViewPosLoc;
	GLuint LightPosLoc;
	GLuint LightColorLoc;

	GLuint BaseColorTextureLoc;
	GLuint MaterialTextureLoc;
	GLuint NormalTextureLoc;

	GLuint BaseColorLoc;
	GLuint RoughnessLoc;
	GLuint SpecularLoc;
	GLuint MetalicLoc;
	GLuint ExposureLoc;

	float TestRougheness;
	void SetShaderPreComplie(bool IsHave, std::string &Data);
private:
	
};

template<typename Renderable, typename Renderer>
void PBRGLSL::Draw(Renderable *Obj, Renderer *Render)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(ProgramID);

	glUniformMatrix4fv(ProjectMatrixLoc, 1, GL_TRUE, Render->ProjectMatrix.M);
	glUniformMatrix4fv(ViewMatrixLoc, 1, GL_TRUE, Render->ViewMatrix.M);
	glUniformMatrix4fv(ModelMatrixLoc, 1, GL_TRUE, Obj->GetModleMatrix().M);
	

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, IBLTextureID);
	glUniform1i(IBL_Specular_Light_Loc, 0);
	glUniform1f(CubemapMaxMipLoc, CubemapMaxMip);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, LUTTextureID);
	glUniform1i(IBL_LUT_Loc, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, IBLIrradianceTextureID);
	glUniform1i(IBLIrradianceLoc, 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, Obj->BaseColorTextureID);
	glUniform1i(BaseColorTextureLoc, 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, Obj->NormalMapTextureID);
	glUniform1i(NormalTextureLoc, 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, Obj->MaterialTextureID);
	glUniform1i(MaterialTextureLoc, 5);
	
	glUniform1f(SpecularLoc, 0.5);
	glUniform1f(RoughnessLoc, TestRougheness);
	glUniform1f(MetalicLoc, 0.5);
	glUniform1f(ExposureLoc, Exposure);
	glUniform3f(ViewPosLoc, ViewPos.x, ViewPos.y, ViewPos.z);
	glUniform3f(LightPosLoc, LightPos.x, LightPos.y, LightPos.z);
	glUniform3f(LightColorLoc, LightColor.x, LightColor.y, LightColor.z);

	glBindVertexArray(Obj->VAO);
	glDrawArrays(GL_TRIANGLES, 0, Obj->DataSize());
}


class GenerateLUTGLSL : public GLSLProgram
{
public: 
	void InitParam();
	GLuint OrthographicLoc;
	GLuint DrawImageVAO;
	GLuint DrawImageTex;
	std::vector<BWVector3D> Square;
};

class UnrealGenerateSpecularIBL :public GLSLProgram
{
public: 
	void InitParam() override;
	RealTimeRenderable<Object> * ProcessCubemapCube;
	GLuint NumMipsLoc;
	GLuint MipIndexLoc;
	GLuint CubeMapLoc;
};