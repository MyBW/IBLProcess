#include "ShaderRead.h"
#include "RealTimeRenderable.h"
#include "..\Math\RTMath.h"
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::Init(int Width, int Height)
{
	InitPiplineState(Width, Height);
	InitShaderProgram();
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::BeginFrame()
{
	glViewport(0, 0, Width, Height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::EndFrame()
{
	glutSwapBuffers();
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::SetLineColor(float R, float G, float B)
{
	glUniform3f(LineColorLoc, R, G, B);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::SetLineWidth(int Width)
{
	glLineWidth(Width);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::ShowCoord()
{
	SetLineWidth(3);

	std::vector<BWVector3D> Lines;
	Lines.push_back(BWVector3D(0, 0, 0));
	Lines.push_back(BWVector3D(100, 0, 0));
	SetLineColor(1.0, 0.0, 0.0);
	DrawLine(Lines);
	
	Lines.clear();
	Lines.push_back(BWVector3D(0, 0, 0));
	Lines.push_back(BWVector3D(0, 100, 0));
	SetLineColor(0.0, 1.0, 0.0);
	DrawLine(Lines);

	Lines.clear();
	Lines.push_back(BWVector3D(0, 0, 0));
	Lines.push_back(BWVector3D(0, 0, 100));
	SetLineColor(0.0, 0.0, 1.0);
	DrawLine(Lines);

	SetLineColor(1.0, 0.0, 0.0);
	SetLineWidth(1);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::UseProgram()
{
	glUseProgram(BaseShaderProgram);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::SetModelMatrix(const BWMatrix4 &ModelMatrix)
{
	glUniformMatrix4fv(ModelMatrixLoc, 1, GL_TRUE, ModelMatrix.M);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::SetViewMatrix(const BWMatrix4 &ViewMatrix)
{
	this->ViewMatrix = ViewMatrix;
	glUniformMatrix4fv(ViewMatrixLoc, 1, GL_TRUE, ViewMatrix.M);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::SetProjectMatrix(const BWMatrix4 &ProjectMatrix)
{
	this->ProjectMatrix = ProjectMatrix;
	glUniformMatrix4fv(ProjectMatrixLoc, 1, GL_TRUE, ProjectMatrix.M);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::SetDrawMode(GLenum Enum)
{
	glPolygonMode(GL_FRONT_AND_BACK, Enum);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::DrawLine(const std::vector<BWVector3D> &Lines)
{
	SetModelMatrix(BWMatrix4::IDENTITY);

	GLuint LinesBuff;
	glGenBuffers(1, &LinesBuff);
	glBindBuffer(GL_ARRAY_BUFFER, LinesBuff);
	glBufferData(GL_ARRAY_BUFFER, Lines.size() * sizeof(BWVector3D), Lines.data(), GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, LinesBuff);
	glEnableVertexAttribArray(PosLoc);
	glVertexAttribPointer(PosLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(VAO);
	glDrawArrays(GL_LINES, 0, Lines.size() * 3);

	glDeleteBuffers(1, &LinesBuff);
	glDeleteVertexArrays(1, &VAO);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::InitShaderProgram()
{
	std::vector<string> PreComplie;
	{
		GLuint VertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
		BaseShaderProgram = glCreateProgram();
		if (BWReadShaderSource(VertexShaderObj, "Shader\\BaseVertexShader.vert" ,PreComplie) == -1)
			return;
		glCompileShader(VertexShaderObj);
		PrintShaderInfoLog(VertexShaderObj);

		if (BWReadShaderSource(FragShaderObj, "Shader\\BaseFragmShader.frag", PreComplie) == -1)
			return;
		glCompileShader(FragShaderObj);
		PrintShaderInfoLog(FragShaderObj);

		glAttachShader(BaseShaderProgram, VertexShaderObj);
		glAttachShader(BaseShaderProgram, FragShaderObj);
		glLinkProgram(BaseShaderProgram);
		PrintProgramInfoLog(BaseShaderProgram);

		PosLoc = glGetAttribLocation(BaseShaderProgram, "Position");
		NormalLoc = glGetAttribLocation(BaseShaderProgram, "Normal");
		UVLoc = glGetAttribLocation(BaseShaderProgram, "inUV");
		TagentLoc = glGetAttribLocation(BaseShaderProgram, "Tagent");
		TextureLoc = glGetAttribLocation(BaseShaderProgram, "LightMap");
		ModelMatrixLoc = glGetUniformLocation(BaseShaderProgram, "ModelMatrix");
		ViewMatrixLoc = glGetUniformLocation(BaseShaderProgram, "ViewMatrix");
		ProjectMatrixLoc = glGetUniformLocation(BaseShaderProgram, "ProjectMatrix");
		LineColorLoc = glGetUniformLocation(BaseShaderProgram, "LineColor");
	}
	{
		GLuint VertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
		DrawImageShader = glCreateProgram();
		if (BWReadShaderSource(VertexShaderObj, "Shader\\DrawImageVS.vert", PreComplie) == -1)
			return;
		glCompileShader(VertexShaderObj);
		PrintShaderInfoLog(VertexShaderObj);

		if (BWReadShaderSource(FragShaderObj, "Shader\\DrawImagePS.frag", PreComplie) == -1)
			return;
		glCompileShader(FragShaderObj);
		PrintShaderInfoLog(FragShaderObj);

		glAttachShader(DrawImageShader, VertexShaderObj);
		glAttachShader(DrawImageShader, FragShaderObj);
		glLinkProgram(DrawImageShader);
		PrintProgramInfoLog(DrawImageShader);

		DrawImagePosLoc = glGetAttribLocation(DrawImageShader, "Position");
		DrawImageUVLoc = glGetAttribLocation(DrawImageShader, "inUV");
		ImageTextureLoc = glGetUniformLocation(DrawImageShader, "ImageTex");
		OrthographicLoc = glGetUniformLocation(DrawImageShader, "Orthographic");



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
		glEnableVertexAttribArray(DrawImagePosLoc);
		glVertexAttribPointer(DrawImagePosLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, UVBuff);
		glEnableVertexAttribArray(DrawImageUVLoc);
		glVertexAttribPointer(DrawImageUVLoc, 2, GL_FLOAT, GL_FALSE, 0, NULL);

		 glBindVertexArray(0);
	}

	{
		GLuint VertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
		EquirectangularToCubeMapProgram = glCreateProgram();
		if (BWReadShaderSource(VertexShaderObj, "Shader\\ConvertEquirectangularToCubeMap.vert", PreComplie) == -1)
			return;
		glCompileShader(VertexShaderObj);
		PrintShaderInfoLog(VertexShaderObj);

		if (BWReadShaderSource(FragShaderObj, "Shader\\ConvertEquirectangularToCubeMap.frag", PreComplie) == -1)
			return;
		glCompileShader(FragShaderObj);
		PrintShaderInfoLog(FragShaderObj);

		glAttachShader(EquirectangularToCubeMapProgram, VertexShaderObj);
		glAttachShader(EquirectangularToCubeMapProgram, FragShaderObj);
		glLinkProgram(EquirectangularToCubeMapProgram);
		PrintProgramInfoLog(EquirectangularToCubeMapProgram);

		EquirectangularToCubePos = glGetAttribLocation(EquirectangularToCubeMapProgram, "Position");
		EquirectangularToCubeNormal = glGetAttribLocation(EquirectangularToCubeMapProgram, "Normal");
		EquirectangularToCubeUV = glGetAttribLocation(EquirectangularToCubeMapProgram, "inUV");
		EquirectangularToCubeEquirectangularSamplerLoc = glGetUniformLocation(EquirectangularToCubeMapProgram, "EquirectangularMap");
		EquirectangularToCubeViewMatrixLoc = glGetUniformLocation(EquirectangularToCubeMapProgram, "ViewMatrix");
		EquirectangularToCubeProjectMatrixLoc = glGetUniformLocation(EquirectangularToCubeMapProgram, "ProjectMatrix");

		Object *Obj = new Object();
		Obj->LoadObjModel("cube.obj", "cube");
		Cube = new RealTimeRenderable<Object>();
		Cube->InitRealTimeRenderable(Obj, EquirectangularToCubePos, EquirectangularToCubeNormal, EquirectangularToCubeUV);

		BWQuaternion Quaterniton;
		//以下的方向都是相对世界坐标系来说的 左手坐标系 这里要注意旋转角度的正负对方向的影响
		Quaterniton.fromAngleAxis(Radian(PI / 2), BWVector3D(0.0, 1.0, 0.0));
		ViewMatrixs[0] = BWMatrix4::makeViewMatrix(BWVector3D(0.0, 0.0, 0.0), Quaterniton, nullptr); // +X
		Quaterniton.fromAngleAxis(Radian(-PI / 2), BWVector3D(0.0, 1.0, 0.0));
		ViewMatrixs[1] = BWMatrix4::makeViewMatrix(BWVector3D(0.0, 0.0, 0.0), Quaterniton, nullptr); //-X
		Quaterniton.fromAngleAxis(Radian(PI / 2.0), BWVector3D(1.0, 0.0, 0.0));
		ViewMatrixs[2] = BWMatrix4::makeViewMatrix(BWVector3D(0.0, 0.0, 0.0), Quaterniton, nullptr); // +Y
		Quaterniton.fromAngleAxis(Radian(-PI / 2.0), BWVector3D(1.0, 0.0, 0.0));
		ViewMatrixs[3] = BWMatrix4::makeViewMatrix(BWVector3D(0.0, 0.0, 0.0), Quaterniton, nullptr); // -Y
		Quaterniton.fromAngleAxis(Radian(0), BWVector3D(0.0, 1.0, 0.0));
		ViewMatrixs[4] = BWMatrix4::makeViewMatrix(BWVector3D(0.0, 0.0, 0.0), Quaterniton, nullptr); // +Z
		Quaterniton.fromAngleAxis(Radian(PI), BWVector3D(0.0, 1.0, 0.0));
		ViewMatrixs[5] = BWMatrix4::makeViewMatrix(BWVector3D(0.0, 0.0, 0.0), Quaterniton, nullptr); //-Z
		
	}

	{
		GLuint VertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
		CubeMapShowProgram = glCreateProgram();
		if (BWReadShaderSource(VertexShaderObj, "Shader\\DrawCubeMap.vert", PreComplie) == -1)
			return;
		glCompileShader(VertexShaderObj);
		PrintShaderInfoLog(VertexShaderObj);

		if (BWReadShaderSource(FragShaderObj, "Shader\\DrawCubeMap.frag", PreComplie) == -1)
			return;
		glCompileShader(FragShaderObj);
		PrintShaderInfoLog(FragShaderObj);

		glAttachShader(CubeMapShowProgram, VertexShaderObj);
		glAttachShader(CubeMapShowProgram, FragShaderObj);
		glLinkProgram(CubeMapShowProgram);
		PrintProgramInfoLog(CubeMapShowProgram);

		CubeMapShowPos = glGetAttribLocation(CubeMapShowProgram, "Position");
		CubeMapShowNormal = glGetAttribLocation(CubeMapShowProgram, "Normal");
		CubeMapShowUV = glGetAttribLocation(CubeMapShowProgram, "inUV");
		CubeMapShowModelMatrixLoc = glGetUniformLocation(CubeMapShowProgram, "ModelMatrix");
		CubeMapShowViewMatrixLoc = glGetUniformLocation(CubeMapShowProgram, "ViewMatrix");
		CubeMapShowProjectMatrixLoc = glGetUniformLocation(CubeMapShowProgram, "ProjectMatrix");
		CubeMapShowSamplerLoc = glGetUniformLocation(CubeMapShowProgram, "CubeMap");
		CubeMapShowRoughnessLoc = glGetUniformLocation(CubeMapShowProgram, "Roughness");
		CubeMapShowRoughness = 0.0;
		
		Object *Obj = new Object();
		Obj->LoadObjModel("cube.obj", std::string("test"));
		Obj->SetPosition(0.0, 0.0, -4);
		CubeMapShow = new RealTimeRenderable<Object>();
		CubeMapShow->InitRealTimeRenderable(Obj, CubeMapShowPos, CubeMapShowNormal, CubeMapShowUV);
	}
	{
		GLuint VertexShaderObj = glCreateShader(GL_VERTEX_SHADER);
		GLuint FragShaderObj = glCreateShader(GL_FRAGMENT_SHADER);
		ProcessCubemapProgram = glCreateProgram();
		if (BWReadShaderSource(VertexShaderObj, "Shader\\ProcessCubemap.vert", PreComplie) == -1)
			return;
		glCompileShader(VertexShaderObj);
		PrintShaderInfoLog(VertexShaderObj);

		if (BWReadShaderSource(FragShaderObj, "Shader\\SSProcessCubemap.frag", PreComplie) == -1)
			return;
		glCompileShader(FragShaderObj);
		PrintShaderInfoLog(FragShaderObj);

		glAttachShader(ProcessCubemapProgram, VertexShaderObj);
		glAttachShader(ProcessCubemapProgram, FragShaderObj);
		glLinkProgram(ProcessCubemapProgram);
		PrintProgramInfoLog(ProcessCubemapProgram);

		ProcessCubemapPos = glGetAttribLocation(ProcessCubemapProgram, "Position");
		ProcessCubemapNormal = glGetAttribLocation(ProcessCubemapProgram, "Normal");
		ProcessCubemapUV = glGetAttribLocation(ProcessCubemapProgram, "inUV");
		ProcessCubemapViewMatrixLoc = glGetUniformLocation(ProcessCubemapProgram, "ViewMatrix");
		ProcessCubemapProjectMatrixLoc = glGetUniformLocation(ProcessCubemapProgram, "ProjectMatrix");
		ProcessCubemapCubemapLoc = glGetUniformLocation(ProcessCubemapProgram, "CubeMap");
		ProcessCubemapRoughnessLoc = glGetUniformLocation(ProcessCubemapProgram, "Roughness");
		

		Object *Obj = new Object();
		Obj->LoadObjModel("cube.obj", std::string("test"));
		ProcessCubemapCube = new RealTimeRenderable<Object>();
		ProcessCubemapCube->InitRealTimeRenderable(Obj, ProcessCubemapPos, ProcessCubemapNormal, ProcessCubemapUV);
	}

	IrradianceCubemapProgram.LoadProgram("Shader\\IrradianceCubemap.vert", "Shader\\IrradianceCubemap.frag");
	GenertateLUTProgram.LoadProgram("Shader\\GenerateLUT.vert", "Shader\\GenerateLUT.frag");
	UnrealGenerateSpecularIBLProgram.LoadProgram("Shader\\UnrealProcessCubeMap.vert", "Shader\\UnrealProcessCuebMap.frag");
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::InitPiplineState(int Width, int Height)
{
	glewInit();
	glViewport(0, 0, Width, Height);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	this->Width = Width;
	this->Height = Height;

}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::Draw()
{
	for (auto Obj : DrawList)
	{
		Obj->Draw(this);
	}
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::AddDrawable(const ObjectType* InObject, GLSLProgram *Program)
{
	RealTimeRenderable<ObjectType> *NewRenderable = new RealTimeRenderable<ObjectType>();
	NewRenderable->InitRealTimeRenderable(InObject, Program);
	DrawList.push_back(NewRenderable);
}

template<typename ObjectType>
void RealTimeRenderer<ObjectType>::AddDrawable(const ObjectType* InObject)
{
	RealTimeRenderable<ObjectType> *NewRenderable = new RealTimeRenderable<ObjectType>();
	NewRenderable->InitRealTimeRenderable(InObject, this);
	DrawList.push_back(NewRenderable);
}


template<typename ObjectType>
void RealTimeRenderer<ObjectType>::DrawImage(int Width, int Height, void *Data, GLenum SrcPF, GLenum DestPF,GLenum PixelEleType)
{	
	
	glGenTextures(1, &DrawImageTex);
	glBindTexture(GL_TEXTURE_2D, DrawImageTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, SrcPF, Width, Height, 0, DestPF, PixelEleType, Data);

	DrawImage(DrawImageTex);

	glDeleteTextures(1, &DrawImageTex);
}


template<typename ObjectType>
void RealTimeRenderer<ObjectType>::CreateFrameBufferWithTexture(int Width, int Height, GLenum SrcPF, GLenum DestPF, GLenum PixelEleType)
{
	glGenFramebuffers(1, &FrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
	
	glGenTextures(1, &FrameBufferTexture);
	glBindTexture(GL_TEXTURE_2D, FrameBufferTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, SrcPF, Width, Height, 0, DestPF, PixelEleType, nullptr);

	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FrameBufferTexture, 0);
}


template<typename ObjectType>
void RealTimeRenderer<ObjectType>::BindFrameBufferWithTexture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FrameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

template<typename ObjectType>
void RealTimeRenderer<ObjectType>::ReadDataFromFrameBuffer(int StartX, int StartY, int Width, int Height, GLenum OutputPF, GLenum OutputDataType, void *Data)
{
	CHECK_GL_ERROR();
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(StartX, StartY, StartX + Width, StartY + Height, OutputPF, OutputDataType, Data);
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::ClearFrameBufferWithTexture()
{
	glDeleteTextures(1, &FrameBufferTexture);
	glDeleteFramebuffers(1, &FrameBuffer);
	glBindFramebuffer(1, 0);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::SetDrawImageShaderParam(std::string &ParamName, float Data)
{
	DrawImageShaderParam[ParamName] = Data;
}

template<typename ObjectType>
void RealTimeRenderer<ObjectType>::GenerateCubeMipmap(GLuint TextureID, int MipNum)
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0));
	CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, MipNum));//从0开始计数//开启这一项后会导致绑定到framebuffer上出现问题
	CHECK_GL_ERROR(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
}



template<typename ObjectType>
void RealTimeRenderer<ObjectType>::CreateIrradianceCubeMap( GLuint SrcCubemap,
	std::function<void(int)> BeginProcess,
	std::function<void(int)> ProcessFace,
	std::function<void(int, int)> EndProcessFace
	)
{
	
	int CubemapSize = 32;
	BeginProcess(CubemapSize);
	/*glGenTextures(1, &DestCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, DestCubemap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, CubemapSize, CubemapSize, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);*/


	GLuint DepthTexture;
	glGenTextures(1, &DepthTexture);
	glBindTexture(GL_TEXTURE_2D, DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, CubemapSize, CubemapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(IrradianceCubemapProgram.ProgramID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, SrcCubemap);
	CHECK_GL_ERROR(glUniform1i(IrradianceCubemapProgram.SrcCubemap, 0));

	BWMatrix4 Orthographic;
	Orthographic.SetZero();
	float CubeWidth = 1.0;
	float N = -0.25;
	float F = -0.55;
	Orthographic.M00 = 2.0 / CubeWidth;
	Orthographic.M11 = 2.0 / CubeWidth;
	Orthographic.M22 = -2.0 / (F - N);
	Orthographic.M23 = -(F + N) / (F - N);
	Orthographic.M33 = 1.0;
	CHECK_GL_ERROR(glUniformMatrix4fv(IrradianceCubemapProgram.ProjectMatrixLoc, 1, GL_TRUE, Orthographic.M));

	for (int i = 0; i < 6; i++)
	{
		CHECK_GL_ERROR(glUniformMatrix4fv(IrradianceCubemapProgram.ViewMatrixLoc, 1, GL_TRUE, ViewMatrixs[i].M));
		
		GLuint NewFrameBuffer;
		glGenFramebuffers(1, &NewFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, NewFrameBuffer);
		ProcessFace(i);
		//CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, DestCubemap, 0));

		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTexture, 0);
		glViewport(0, 0, CubemapSize, CubemapSize);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(Cube->GetVAO());
		glDrawArrays(GL_TRIANGLES, 0, Cube->DataSize());

		EndProcessFace(CubemapSize,i);

		glDeleteFramebuffers(1, &NewFrameBuffer);
	}
	glDeleteTextures(1, &DepthTexture);
}



template<typename ObjectType>
void RealTimeRenderer<ObjectType>::ConverEquirectangularToCubeMap(int Width, int Height, void *Data, int CubeMapSize, GLenum SrcPF /*= GL_RGB*/, GLenum DestPF /*= GL_RGB*/, GLenum PixelEleType /*= GL_UNSIGNED_BYTE*/)
{
	CHECK_GL_ERROR(glGenTextures(1, &CubeMapTexture));
	CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapTexture));
	for (unsigned int i = 0; i < 6; i++)
	{
		CHECK_GL_ERROR(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, SrcPF, CubeMapSize, CubeMapSize, 0, DestPF, PixelEleType, nullptr));
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLuint DepthTexture;
	glGenTextures(1, &DepthTexture);
	glBindTexture(GL_TEXTURE_2D, DepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, CubeMapSize, CubeMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	GLuint EquirectangularTexture;
	glGenTextures(1, &EquirectangularTexture);
	glBindTexture(GL_TEXTURE_2D, EquirectangularTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, SrcPF, Width, Height, 0, DestPF, PixelEleType, Data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, EquirectangularTexture);
	CHECK_GL_ERROR(glUniform1i(EquirectangularToCubeEquirectangularSamplerLoc, 0));

	CHECK_GL_ERROR(glUseProgram(EquirectangularToCubeMapProgram));
	BWMatrix4 Orthographic;
	Orthographic.SetZero();
	float CubeWidth = 1.0;
	float N = -0.25;
	float F = -0.55;
	Orthographic.M00 = 2.0 / CubeWidth;
	Orthographic.M11 = 2.0 / CubeWidth;
	Orthographic.M22 = -2.0 / (F - N);
	Orthographic.M23 = -(F + N) / (F - N);
	Orthographic.M33 = 1.0;
	CHECK_GL_ERROR( glUniformMatrix4fv(EquirectangularToCubeProjectMatrixLoc, 1, GL_TRUE, Orthographic.M));

	
	
	for (int i = 0 ;i < 6 ; i++)
	{
		GLuint NewFrameBuffer;
		CHECK_GL_ERROR( glUniformMatrix4fv(EquirectangularToCubeViewMatrixLoc, 1, GL_TRUE, ViewMatrixs[i].M));
		glGenFramebuffers(1, &NewFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, NewFrameBuffer);
		CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, CubeMapTexture, 0));
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTexture, 0);
		glViewport(0, 0, CubeMapSize, CubeMapSize);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindVertexArray(Cube->GetVAO());
		glDrawArrays(GL_TRIANGLES, 0, Cube->DataSize());

		glDeleteFramebuffers(1, &NewFrameBuffer);
	}
	glDeleteTextures(1, &EquirectangularTexture);
	glDeleteTextures(1, &DepthTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapTexture);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}


template<typename ObjectType>
void RealTimeRenderer<ObjectType>::ShowCubeMap(GLuint CubeMap , BWMatrix4& Project, BWMatrix4 &View)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(CubeMapShowProgram);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap);
	glUniform1i(CubeMapShowSamplerLoc, 0);

	float LevelFrom1X1 = 1 - 1.2 * log2(CubeMapShowRoughness);
	float tmpRought = 9.0 - 1 - LevelFrom1X1;

	glUniform1f(CubeMapShowRoughnessLoc, CubeMapShowRoughness);

	CHECK_GL_ERROR(glUniformMatrix4fv(CubeMapShowModelMatrixLoc, 1, GL_TRUE, CubeMapShow->GetModleMatrix().M));
	glUniformMatrix4fv(CubeMapShowProjectMatrixLoc, 1, GL_TRUE, Project.M);
	glUniformMatrix4fv(CubeMapShowViewMatrixLoc, 1, GL_TRUE, View.M);
	glBindVertexArray(CubeMapShow->GetVAO());
	glDrawArrays(GL_TRIANGLES, 0, CubeMapShow->DataSize());

}

template<typename ObjectType>
void RealTimeRenderer<ObjectType>::UnrealProcessCubeMap(GLuint CubeMap, int CubemapLength, GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, int MaxRoughtnessLevel, 
	std::function<void(GLenum, GLenum, GLenum, float, int)> BeginProcess, 
	std::function<void(GLenum, GLenum, GLenum, float, int)> EndProcess, 
	std::function<void(GLenum, GLenum, GLenum, float, int)> ForBeginProcessMipmaplevel, 
	std::function<void(GLenum, GLenum, GLenum, float, int)> ForEndProcessMipmaplevel, 
	std::function<void(int, GLenum, GLenum, int, int)> ForBeginCubeMapFace, 
	std::function<void(int, GLenum, GLenum, int, int)> ForEndCubeMapFace)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(UnrealGenerateSpecularIBLProgram.ProgramID);


	BWMatrix4 Orthographic;
	Orthographic.SetZero();
	float CubeWidth = 1.0;
	float N = -0.25;
	float F = -0.55;
	Orthographic.M00 = 2.0 / CubeWidth;
	Orthographic.M11 = 2.0 / CubeWidth;
	Orthographic.M22 = -2.0 / (F - N);
	Orthographic.M23 = -(F + N) / (F - N);
	Orthographic.M33 = 1.0;
	CHECK_GL_ERROR(glUniformMatrix4fv(UnrealGenerateSpecularIBLProgram.ProjectMatrixLoc, 1, GL_TRUE, Orthographic.M));
	int MipmapNum = log(CubemapLength) / log(2);


	MipmapNum++;
	BeginProcess(SrcPF, DestPF, PixelEleType, MipmapNum, CubemapLength);
	glUniform1i(UnrealGenerateSpecularIBLProgram.NumMipsLoc, MipmapNum);
	for (int i = 0; i < MipmapNum ; i++)
	{
		int MipSize = 1 << (MipmapNum - i - 1);
		glViewport(0, 0, MipSize, MipSize);
		glUniform1i(UnrealGenerateSpecularIBLProgram.MipIndexLoc, i);

		float LevelFrom1x1 = MipmapNum - 1 - i;
		float RougnessOfShader = exp2((1.0 - LevelFrom1x1) / 1.2);

		ForBeginProcessMipmaplevel(SrcPF, DestPF, PixelEleType, i, MipSize);
		for (int j = 0; j < 6; j++)
		{
			CHECK_GL_ERROR(glUniformMatrix4fv(UnrealGenerateSpecularIBLProgram.ViewMatrixLoc, 1, GL_TRUE, ViewMatrixs[j].M));
			ForBeginCubeMapFace(MipSize, DestPF, PixelEleType, i, j);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap);
			glUniform1i(UnrealGenerateSpecularIBLProgram.CubeMapLoc, 0);

			glBindVertexArray(UnrealGenerateSpecularIBLProgram.ProcessCubemapCube->GetVAO());
			glDrawArrays(GL_TRIANGLES, 0, UnrealGenerateSpecularIBLProgram.ProcessCubemapCube->DataSize());

			ForEndCubeMapFace(MipSize, DestPF, PixelEleType, i, j);
		}
		ForEndProcessMipmaplevel(SrcPF, DestPF, PixelEleType, i, MipSize);
	}
	EndProcess(SrcPF, DestPF, PixelEleType, MipmapNum, CubemapLength);


	//MipmapNum = TMin(MipmapNum, MaxRoughtnessLevel);
	/*BeginProcess(SrcPF, DestPF, PixelEleType, MaxRoughtnessLevel, CubemapLength);
	glUniform1i(UnrealGenerateSpecularIBLProgram.NumMipsLoc, MipmapNum);

	for (int i = 0; i < MaxRoughtnessLevel + 1; i++)
	{
		float InvMipLevel = pow(0.5, i);
		glViewport(0, 0, CubemapLength*InvMipLevel, CubemapLength*InvMipLevel);
		glUniform1i(UnrealGenerateSpecularIBLProgram.MipIndexLoc, i);

		float LevelFrom1x1 = MipmapNum - 1 - i;
		float RougnessOfShader = exp2((1.0 - LevelFrom1x1) / 1.2);

		ForBeginProcessMipmaplevel(SrcPF, DestPF, PixelEleType, i, CubemapLength * InvMipLevel);
		for (int j = 0; j < 6; j++)
		{
			CHECK_GL_ERROR(glUniformMatrix4fv(UnrealGenerateSpecularIBLProgram.ViewMatrixLoc, 1, GL_TRUE, ViewMatrixs[j].M));
			ForBeginCubeMapFace(CubemapLength*InvMipLevel, DestPF, PixelEleType, i, j);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap);
			glUniform1i(UnrealGenerateSpecularIBLProgram.CubeMapLoc, 0);

			glBindVertexArray(UnrealGenerateSpecularIBLProgram.ProcessCubemapCube->GetVAO());
			glDrawArrays(GL_TRIANGLES, 0, UnrealGenerateSpecularIBLProgram.ProcessCubemapCube->DataSize());

			ForEndCubeMapFace(CubemapLength*InvMipLevel, DestPF, PixelEleType, i, j);
		}
		ForEndProcessMipmaplevel(SrcPF, DestPF, PixelEleType, i, CubemapLength * InvMipLevel);
	}
	EndProcess(SrcPF, DestPF, PixelEleType, MipmapNum, CubemapLength);*/
}

template<typename ObjectType>
void RealTimeRenderer<ObjectType>::ProcessCubeMap(GLuint CubeMap, int CubemapLength, GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, int MaxRoughtnessLevel, 
	std::function<void(GLenum, GLenum, GLenum, float, int)> BeginProcess,
	std::function<void(GLenum, GLenum, GLenum, float, int)> EndProcess,
	std::function<void(GLenum, GLenum, GLenum, float, float, int)> ForBeginProcessMipmaplevel,
	std::function<void(GLenum, GLenum, GLenum, float, float, int)> ForEndProcessMipmaplevel,
	std::function<void(int, GLenum, GLenum, int, int)> ForBeginCubeMapFace,
	std::function<void(int, GLenum, GLenum, int, int)> ForEndCubeMapFace)
{

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(ProcessCubemapProgram);

	

	BWMatrix4 Orthographic;
	Orthographic.SetZero();
	float CubeWidth = 1.0;
	float N = -0.25;
	float F = -0.55;
	Orthographic.M00 = 2.0 / CubeWidth;
	Orthographic.M11 = 2.0 / CubeWidth;
	Orthographic.M22 = -2.0 / (F - N);
	Orthographic.M23 = -(F + N) / (F - N);
	Orthographic.M33 = 1.0;
	CHECK_GL_ERROR(glUniformMatrix4fv(ProcessCubemapProjectMatrixLoc, 1, GL_TRUE, Orthographic.M));
	int RoughtnessLevel = log(CubemapLength) / log(2);
	//RoughtnessLevel = TMin(RoughtnessLevel, MaxRoughtnessLevel);
	BeginProcess(SrcPF, DestPF, PixelEleType, RoughtnessLevel, CubemapLength);

	
	for (int i = 0; i < RoughtnessLevel; i++)
	{
		float MipLevel = pow(0.5, i);
		float Roughtness = float(i)/(RoughtnessLevel - 1);
		{
			//SS Roughtness change
			std::map<int, float> SSRoughtnessMap;
			SSRoughtnessMap[0] = 1.0;
			SSRoughtnessMap[1] = 0.95;
			SSRoughtnessMap[2] = 0.9;
			SSRoughtnessMap[3] = 0.85;
			SSRoughtnessMap[4] = 0.8;
			SSRoughtnessMap[5] = 0.7;
			SSRoughtnessMap[6] = 0.55;
			SSRoughtnessMap[7] = 0.4;
			SSRoughtnessMap[8] = 0.25;
			Roughtness = SSRoughtnessMap[i];
		}
		glViewport(0, 0, CubemapLength*MipLevel, CubemapLength*MipLevel);
		glUniform1f(ProcessCubemapRoughnessLoc, Roughtness);

		ForBeginProcessMipmaplevel(SrcPF, DestPF, PixelEleType, MipLevel, Roughtness, CubemapLength * MipLevel);
		for (int j = 0; j < 6; j++)
		{
			CHECK_GL_ERROR(glUniformMatrix4fv(ProcessCubemapViewMatrixLoc, 1, GL_TRUE, ViewMatrixs[j].M));
			ForBeginCubeMapFace(CubemapLength*MipLevel, DestPF, PixelEleType, i, j);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMap);
			glUniform1i(ProcessCubemapCubemapLoc, 0);

			glBindVertexArray(ProcessCubemapCube->GetVAO());
			glDrawArrays(GL_TRIANGLES, 0, ProcessCubemapCube->DataSize());

			ForEndCubeMapFace(CubemapLength*MipLevel, DestPF, PixelEleType, i, j);
		}
		ForEndProcessMipmaplevel(SrcPF, DestPF, PixelEleType, MipLevel, Roughtness, CubemapLength * MipLevel);
	}
	EndProcess(SrcPF, DestPF, PixelEleType, RoughtnessLevel, CubemapLength);
}


template<typename ObjectType>
void RealTimeRenderer<ObjectType>::GenerateLUT(GLuint &LUTTextureID, int Width)
{
	glViewport(0, 0, Width, Width);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glUseProgram(GenertateLUTProgram.ProgramID);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glGenTextures(1, &LUTTextureID);
	glBindTexture(GL_TEXTURE_2D, LUTTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, Width, Width, 0, GL_RG, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	BWMatrix4 Orthographic;
	Orthographic.SetZero();
	Orthographic.M00 = 2.0 / 20;
	Orthographic.M11 = 2.0 / 20;
	Orthographic.M22 = -2.0 / 20;
	Orthographic.M33 = 1;
	glUniformMatrix4fv(GenertateLUTProgram.OrthographicLoc, 1, GL_TRUE, Orthographic.M);

	GLuint NewFrameBuffer;
	glGenFramebuffers(1, &NewFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, NewFrameBuffer);
	CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, LUTTextureID, 0));
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	glBindVertexArray(GenertateLUTProgram.DrawImageVAO);
	glDrawArrays(GL_TRIANGLES, 0, GenertateLUTProgram.Square.size() * 3);
	EndFrame();

	glDeleteFramebuffers(1, &NewFrameBuffer);
}
template<typename ObjectType>
void RealTimeRenderer<ObjectType>::DrawImage(GLuint TextureID)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glUseProgram(DrawImageShader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TextureID);
	glUniform1i(ImageTextureLoc, 0);

	BWMatrix4 Orthographic;
	Orthographic.SetZero();
	Orthographic.M00 = 2.0 / 20;
	Orthographic.M11 = 2.0 / 20;
	Orthographic.M22 = -2.0 / 20;
	Orthographic.M33 = 1;
	glUniformMatrix4fv(OrthographicLoc, 1, GL_TRUE, Orthographic.M);

	for (auto &Para : DrawImageShaderParam)
	{
		GLuint DataLoc = glGetUniformLocation(DrawImageShader, Para.first.data());
		glUniform1f(DataLoc, Para.second);
	}


	glBindVertexArray(DrawImageVAO);
	glDrawArrays(GL_TRIANGLES, 0, Square.size() * 3);
}
