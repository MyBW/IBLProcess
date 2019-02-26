#include "GLSLProgram.h"
#include "ShaderRead.h"

void GLSLProgram::LoadProgram(std::string VertexShader, std::string FragShader)
{
	GLuint VertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
	ProgramID = glCreateProgram();
	if (BWReadShaderSource(VertexShaderObj, VertexShader.c_str(), ShaderPrecomplie) == -1)
		return;
	glCompileShader(VertexShaderObj);
	PrintShaderInfoLog(VertexShaderObj);

	if (BWReadShaderSource(FragShaderObj, FragShader.c_str(), ShaderPrecomplie) == -1)
		return;
	glCompileShader(FragShaderObj);
	PrintShaderInfoLog(FragShaderObj);

	glAttachShader(ProgramID, VertexShaderObj);
	glAttachShader(ProgramID, FragShaderObj);
	glLinkProgram(ProgramID);
	PrintProgramInfoLog(ProgramID);
	InitParam();
}

void GLSLProgram::InitParam()
{
	PositionLoc = glGetAttribLocation(ProgramID, "inPosition");
	NormalLoc = glGetAttribLocation(ProgramID, "inNormal");
	UVLoc = glGetAttribLocation(ProgramID, "inUV");
	ModelMatrixLoc = glGetUniformLocation(ProgramID, "ModelMatrix");
	ViewMatrixLoc = glGetUniformLocation(ProgramID, "ViewMatrix");
	ProjectMatrixLoc = glGetUniformLocation(ProgramID, "ProjectMatrix");
}

void BaseShowGLSL::InitParam()
{
	GLSLProgram::InitParam();
	TagentLoc = glGetAttribLocation(ProgramID, "Tagent");
	TextureLoc = glGetAttribLocation(ProgramID, "LightMap");
	LineColorLoc = glGetUniformLocation(ProgramID, "LineColor");
}

void DrawImageGLSL::InitParam()
{
	GLSLProgram::InitParam();
	ImageTextureLoc = glGetUniformLocation(ProgramID, "ImageTex");
	OrthographicLoc = glGetUniformLocation(ProgramID, "Orthographic");

	Square.clear();
	std::vector<BWPoint2DD> UVs;
	Square.push_back(BWVector3D(-10, 10, -10));
	UVs.push_back(BWPoint2DD(0, 1));
	Square.push_back(BWVector3D(-10, -10, -10));
	UVs.push_back(BWPoint2DD(0, 0));
	Square.push_back(BWVector3D(10, -10, -10));
	UVs.push_back(BWPoint2DD(1, 0));

	Square.push_back(BWVector3D(-10, 10, -10));
	UVs.push_back(BWPoint2DD(0, 1));
	Square.push_back(BWVector3D(10, -10, -10));
	UVs.push_back(BWPoint2DD(1, 0));
	Square.push_back(BWVector3D(10, 10, -10));
	UVs.push_back(BWPoint2DD(1, 1));

	GLuint LinesBuff;
	glGenBuffers(1, &LinesBuff);
	glBindBuffer(GL_ARRAY_BUFFER, LinesBuff);
	glBufferData(GL_ARRAY_BUFFER, Square.size() * sizeof(BWVector3D), Square.data(), GL_STATIC_DRAW);

	GLuint UVBuff;
	glGenBuffers(1, &UVBuff);
	glBindBuffer(GL_ARRAY_BUFFER, UVBuff);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(BWPoint2DD), UVs.data(), GL_STATIC_DRAW);


	glGenVertexArrays(1, &DrawImageVAO);
	glBindVertexArray(DrawImageVAO);

	glBindBuffer(GL_ARRAY_BUFFER, LinesBuff);
	glEnableVertexAttribArray(PositionLoc);
	glVertexAttribPointer(PositionLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, UVBuff);
	glEnableVertexAttribArray(UVLoc);
	glVertexAttribPointer(UVLoc, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(0);
}

void PBRGLSL::SetIsHaveBaseColorTexture(bool IsHave)
{
	std::string PreComp = std::string("#define BaseColor_Texture 1\r\n");
	SetShaderPreComplie(IsHave, PreComp);
}

void PBRGLSL::SetIsHaveNormalmapTexture(bool IsHave)
{
	std::string PreComp = std::string("#define Normal_Texture 1\r\n");
	SetShaderPreComplie(IsHave, PreComp);
}

void PBRGLSL::SetIsHaveMaterialTexture(bool IsHave)
{
	std::string PreComp = std::string("#define Matrial_Texture 1\r\n");
	SetShaderPreComplie(IsHave, PreComp);
}

void PBRGLSL::InitParam()
{
	GLSLProgram::InitParam();
	IBL_Specular_Light_Loc = glGetUniformLocation(ProgramID, "IBL_Specular_Light");
	IBL_LUT_Loc = glGetUniformLocation(ProgramID, "IBL_LUT");
	CubemapMaxMipLoc = glGetUniformLocation(ProgramID, "CubemapMaxMip");
	ViewPosLoc = glGetUniformLocation(ProgramID, "ViewPos");
	LightPosLoc = glGetUniformLocation(ProgramID, "LightPos");
	LightColorLoc = glGetUniformLocation(ProgramID, "LightColor");

	BaseColorTextureLoc = glGetUniformLocation(ProgramID, "BaseColorTexture");
	MaterialTextureLoc = glGetUniformLocation(ProgramID, "MaterialTexture");
	NormalTextureLoc = glGetUniformLocation(ProgramID, "NormalTexture");

	BaseColorLoc = glGetUniformLocation(ProgramID, "PsBaseColor");
	RoughnessLoc = glGetUniformLocation(ProgramID, "Roughness");
	SpecularLoc = glGetUniformLocation(ProgramID, "Specular");
	MetalicLoc = glGetUniformLocation(ProgramID, "Metalic");

	ExposureLoc = glGetUniformLocation(ProgramID, "Exposure");
	IBLIrradianceLoc = glGetUniformLocation(ProgramID, "Irradiance_Light");
	TestRougheness = 1.0;
}

void PBRGLSL::SetShaderPreComplie(bool IsHave, std::string &Data)
{
	if (IsHave)
	{
		ShaderPrecomplie.push_back(Data);
	}
	else
	{
		std::vector<std::string>::iterator Itor = ShaderPrecomplie.begin();
		for (; Itor != ShaderPrecomplie.end(); Itor++)
		{
			if (*Itor == Data)
			{
				ShaderPrecomplie.erase(Itor);
				break;
			}
		}
	}
}

void IrradianceCubemapGLSL::InitParam()
{
	GLSLProgram::InitParam();
	SrcCubemap = glGetUniformLocation(ProgramID, "SrcCubemap");
}

void GenerateLUTGLSL::InitParam()
{
	GLSLProgram::InitParam();
	OrthographicLoc = glGetUniformLocation(ProgramID, "Orthographic");

	Square.clear();
	std::vector<BWPoint2DD> UVs;
	Square.push_back(BWVector3D(-10, 10, -10));
	UVs.push_back(BWPoint2DD(0, 1));
	Square.push_back(BWVector3D(-10, -10, -10));
	UVs.push_back(BWPoint2DD(0, 0));
	Square.push_back(BWVector3D(10, -10, -10));
	UVs.push_back(BWPoint2DD(1, 0));

	Square.push_back(BWVector3D(-10, 10, -10));
	UVs.push_back(BWPoint2DD(0, 1));
	Square.push_back(BWVector3D(10, -10, -10));
	UVs.push_back(BWPoint2DD(1, 0));
	Square.push_back(BWVector3D(10, 10, -10));
	UVs.push_back(BWPoint2DD(1, 1));

	GLuint LinesBuff;
	glGenBuffers(1, &LinesBuff);
	glBindBuffer(GL_ARRAY_BUFFER, LinesBuff);
	glBufferData(GL_ARRAY_BUFFER, Square.size() * sizeof(BWVector3D), Square.data(), GL_STATIC_DRAW);

	GLuint UVBuff;
	glGenBuffers(1, &UVBuff);
	glBindBuffer(GL_ARRAY_BUFFER, UVBuff);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(BWPoint2DD), UVs.data(), GL_STATIC_DRAW);


	glGenVertexArrays(1, &DrawImageVAO);
	glBindVertexArray(DrawImageVAO);

	glBindBuffer(GL_ARRAY_BUFFER, LinesBuff);
	glEnableVertexAttribArray(PositionLoc);
	glVertexAttribPointer(PositionLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, UVBuff);
	glEnableVertexAttribArray(UVLoc);
	glVertexAttribPointer(UVLoc, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(0);
}

void UnrealGenerateSpecularIBL::InitParam()
{
	GLSLProgram::InitParam();
	NumMipsLoc = glGetUniformLocation(ProgramID, "NumMips");
	MipIndexLoc = glGetUniformLocation(ProgramID, "MipIndex");
	CubeMapLoc = glGetUniformLocation(ProgramID, "CubeMap");

	Object *Obj = new Object();
	Obj->LoadObjModel("cube.obj", std::string("test"));
	ProcessCubemapCube = new RealTimeRenderable<Object>();
	ProcessCubemapCube->InitRealTimeRenderable(Obj, PositionLoc, NormalLoc, UVLoc);
}
