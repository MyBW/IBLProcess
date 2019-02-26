#include <iostream>
#include "glew.h"
#include "glut.h"
#include "ImageLoader\ImageLoader.h"
#include "RealTimeRenderer\RealTimeRenderer.h"
#include "Scene\Object.h"
#include "Scene\Camera.h"
#include "RealTimeRenderer\GLSLProgram.h"
#include "RealTimeRenderer\ShaderRead.h"
#include "ObjFileLoader\ObjFileLoader.h"
ImageFile *Img;
ImageFile *SubImage;
ImageLoader IMGLoader;
bool IsReadFromBuffer;
RealTimeRenderer<Object> RTRenderer;
Object *Obj;
PBRGLSL *PBR;
Camera  CubeMapShowCamer;
GLuint TestCubeMap;
GLuint SpecularCubemapTexture;
GLuint LUTTexture;
GLuint IrradianceMap;

//Unreal
GLuint UnrealSpecularCubemapTexture;

bool IsContorllCamera = false;
int OldMouseX = 0;
int OldMouseY = 0;
void MoveCamera(unsigned char key, int x, int y)
{
	BWVector3D Direction;
	float Speed = 0.5;
	BWVector3D Dir(0, 1.0, 0);
	switch (key)
	{
	case 'w':
	case 'W':
		Direction = CubeMapShowCamer.GetDirection();
		Direction.normalize();
		Direction = Direction * Speed;
		break;
	case 'S':
	case 's':
		Direction = CubeMapShowCamer.GetDirection();
		Direction.normalize();
		Direction = Direction *(-1) * Speed;
		break;
	case 'A':
	case 'a':
		Direction = CubeMapShowCamer.GetRight();
		Direction.normalize();
		Direction = Direction *(-1) * Speed;
		break;
	case 'D':
	case 'd':
		Direction = CubeMapShowCamer.GetRight();
		Direction.normalize();
		Direction = Direction * Speed;
		break;
	default:
		break;
	}
	CubeMapShowCamer.Move(Direction);
}
void ProcessKeyboard(unsigned char key, int x, int y)
{
	Obj = const_cast<Object*>(RTRenderer.CubeMapShow->GetObject());
    static float Exposure = 1.0;
	switch (key)
	{
	case 'i':
	case 'I':
	{
		Exposure += 0.1;
		RTRenderer.SetDrawImageShaderParam(std::string("Exposure"), Exposure);
	}
		break;
	case 'o':
	case 'O':
	{
		Exposure -= 0.1;
		RTRenderer.SetDrawImageShaderParam(std::string("Exposure"), Exposure);
		break;
	}
	case 'p':
	case 'P':
	{
		IsReadFromBuffer = true;
		break;
	}
	case 'q':
	case 'Q':
	{
		Obj->SetRoataion(BWVector3D(1.0, 1.0, 1.0), Radian(0.1));
		break;
	}
	case 'u':
	case 'U':
	{
		PBR->TestRougheness += 0.01;
		break;
	}
	case 'y':
	case'Y':
	{
		PBR->TestRougheness -= 0.01;
		break;
	}
	case 'j':
	case 'J':
	{
		RTRenderer.CubeMapShowRoughness += 0.01;
		break;
	}
	case 'k':
	case 'K':
	{
		RTRenderer.CubeMapShowRoughness -= 0.01;
		break;
	}
	break;
	default:
		break;
	}
	PBR->Exposure = Exposure;
	MoveCamera(key, x, y);
	glutPostRedisplay();
}
void ShowImage()
{
	RTRenderer.BeginFrame();
	RTRenderer.DrawImage(LUTTexture);
	/*switch (Img->ImageFormat)
	{
	case  FIT_RGBA16:
		RTRenderer.DrawImage(Img->Width, Img->Height, Img->BitData, GL_RGBA16F, GL_RGB, GL_UNSIGNED_BYTE);
		break;
	case FIT_RGBF:
		RTRenderer.DrawImage(Img->Width, Img->Height, Img->BitData, GL_RGB32F, GL_RGB, GL_FLOAT);
		break;
	case FIT_BITMAP:
		RTRenderer.DrawImage(Img->Width, Img->Height, Img->BitData, GL_RGB, GL_BGR);
		break;
	default:
		break;
	}*/

	RTRenderer.EndFrame();
}
void PreProcessImage()
{
	RTRenderer.BeginFrame();

	if (IsReadFromBuffer)
	{
		switch (Img->ImageFormat)
		{
		case FIT_RGBF:
			RTRenderer.CreateFrameBufferWithTexture(Img->Width, Img->Height, GL_RGB32F, GL_RGB, GL_FLOAT);
			break;
		case FIT_BITMAP:
			RTRenderer.CreateFrameBufferWithTexture(Img->Width, Img->Height, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
			break;
		default:
			break;
		}
		RTRenderer.BindFrameBufferWithTexture();
	}

	
	switch (Img->ImageFormat)
	{
	case  FIT_RGBA16:
		RTRenderer.DrawImage(Img->Width, Img->Height, Img->BitData, GL_RGBA16F, GL_RGB, GL_UNSIGNED_BYTE);
		break;
	case FIT_RGBF:
		RTRenderer.DrawImage(Img->Width, Img->Height, Img->BitData, GL_RGB32F, GL_RGB, GL_FLOAT);
		break;
	case FIT_BITMAP:
		RTRenderer.DrawImage(Img->Width, Img->Height, Img->BitData, GL_RGB, GL_BGR);
		break;
	default:
		break;
	}
	if (IsReadFromBuffer)
	{
		SubImage = new ImageFile();
		Img->CopyWihoutData(SubImage);
		SubImage->SetSize(512, 512);
		switch (Img->ImageFormat)
		{
		case FIT_RGBF:
			RTRenderer.ReadDataFromFrameBuffer(0, 0, SubImage->Width, SubImage->Height, GL_RGB, GL_FLOAT, SubImage->BitData);
			break;
		case FIT_BITMAP:
			RTRenderer.ReadDataFromFrameBuffer(0, 0, SubImage->Width, SubImage->Height, GL_RGB, GL_UNSIGNED_BYTE, SubImage->BitData);
			break;
		default:
			break;
		}
		RTRenderer.ClearFrameBufferWithTexture();
		IsReadFromBuffer = false;
		IMGLoader.SaveToFile(std::string("LUT.hdr"), SubImage);
	}

	RTRenderer.EndFrame();
}
void DrawScene()
{
	RTRenderer.BeginFrame();

	RTRenderer.SetLineWidth(1);
	RTRenderer.UseProgram();
	RTRenderer.SetLineColor(1.0, 0.0, 0.0);
	RTRenderer.SetViewMatrix(CubeMapShowCamer.GetViewMatrix());
	RTRenderer.SetProjectMatrix(CubeMapShowCamer.GetProjectMatrix());
	
	//PBR->IBLTextureID = SpecularCubemapTexture;
	PBR->IBLTextureID = UnrealSpecularCubemapTexture;
	PBR->IBLIrradianceTextureID = IrradianceMap;
	PBR->LUTTextureID = LUTTexture;
	PBR->ViewPos = CubeMapShowCamer.GetPosition();
	PBR->LightColor = BWVector3D(100.0, 100.0, 100.0);
	PBR->LightPos = BWVector3D(30, 30, 0);

	RTRenderer.Draw();

	RTRenderer.EndFrame();
}
void ShowCubemap()
{
	RTRenderer.BeginFrame();
	//RTRenderer.ShowCubeMap(RTRenderer.CubeMapTexture, CubeMapShowCamer.GetProjectMatrix(), CubeMapShowCamer.GetViewMatrix());
	//RTRenderer.ShowCubeMap(SpecularCubemapTexture, CubeMapShowCamer.GetProjectMatrix(), CubeMapShowCamer.GetViewMatrix());
	//RTRenderer.ShowCubeMap(IrradianceMap, CubeMapShowCamer.GetProjectMatrix(), CubeMapShowCamer.GetViewMatrix());
	RTRenderer.ShowCubeMap(UnrealSpecularCubemapTexture, CubeMapShowCamer.GetProjectMatrix(), CubeMapShowCamer.GetViewMatrix());
	//RTRenderer.ShowCubeMap(TestCubeMap, CubeMapShowCamer.GetProjectMatrix(), CubeMapShowCamer.GetViewMatrix());
	RTRenderer.EndFrame();
}
void display()
{
	//ShowCubemap();
	DrawScene();
	//PreProcessImage();
	//ShowImage();
}
void mouse(int x, int y)
{
	if (IsContorllCamera && (OldMouseX != x || OldMouseY != y))
	{
		float DeltaX = x - OldMouseX;
		float DeltaY = y - OldMouseY;
		DeltaX = DeltaX / 800 * 3.14;
		DeltaY = DeltaY / 800 * 3.14;
		CubeMapShowCamer.SetFixedYawAxis(true);
		CubeMapShowCamer.Yaw(Radian(DeltaX * (-1.0)));
		CubeMapShowCamer.Pitch(Radian(DeltaY * (-1.0)));
		OldMouseY = y;
		OldMouseX = x;
	}
	glutPostRedisplay();
}
void moueMoveFunction(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		IsContorllCamera = true;
		OldMouseX = x;
		OldMouseY = y;
	}
	else
	{
		IsContorllCamera = false;
	}
}
int main(int argc, char **argv)
{

	auto CreateFileName = [](std::string &FileName, int Index1, int Index2)
	{
		size_t Pos = FileName.find_last_of('.');
		std::map<int, int> CoverToWebGL;
		CoverToWebGL[0] = 3;
		CoverToWebGL[1] = 2;
		CoverToWebGL[2] = 4;
		CoverToWebGL[3] = 5;
		CoverToWebGL[4] = 1;
		CoverToWebGL[5] = 0;
		FileName = FileName.substr(0, Pos) + std::string("_") + std::to_string(Index1) + std::string("_") + std::to_string(CoverToWebGL[Index2]) + FileName.substr(Pos, FileName.size() - Pos);
	};

	int Width, Height;
	Width = 1024;
	Height = 1024;
	IsReadFromBuffer = false;
	CubeMapShowCamer.Init(Width, Height);
	IMGLoader.SetIsFilpImage(true); // 反转一下
	Img = IMGLoader.GetImageFile(std::string("10-Shiodome_Stairs_3k.hdr"));

	ObjFileLoader ObjLoader;
	std::vector<ObjFileLoader::ObjInfo> ObjInfos = ObjLoader.OpenFile(std::string("TeaportObj.obj"));

	glutInit(&argc, argv);
	glutInitWindowSize(Width, Height);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("IBLTextureProcess");
	RTRenderer.Init(Width, Height);
	RTRenderer.BeginFrame();
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // 立方体贴图上有链接的地方有缝隙
	RTRenderer.ConverEquirectangularToCubeMap(Img->Width, Img->Height, Img->BitData, 128, GL_RGB16F, GL_RGB, GL_FLOAT);
	RTRenderer.GenerateCubeMipmap(RTRenderer.CubeMapTexture, 8);

	{
		//产生specular cube map 并保存
		GLuint DepthTexture;
		GLuint ColorTexture;
		GLuint NewFrameBuffer;
		ImageFile CubeImageInfo[6];
		for (int i = 0; i < 6; i++)
		{
			CubeImageInfo[i].BitData = nullptr;
			CubeImageInfo[i].BPP = 32;
			CubeImageInfo[i].SetSize(512, 512);
			CubeImageInfo[i].FileFormat = FIF_PNG;
			CubeImageInfo[i].FileName = "Test.png";
			CubeImageInfo[i].ImageColorType = FIC_RGBALPHA;
			CubeImageInfo[i].ImageFormat = FIT_BITMAP;
		}
		auto BegeinProcess = [](GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, float MaxMipLevel, int CubemapLength) {};
		auto EndProcess = [](GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, float MaxMipLevel, int CubemapLength) {};
		auto ForBeginProcessMipmaplevel = [&](GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, float MipLevel, float Roughtness, int CubemapLength)
		{
			glGenTextures(1, &DepthTexture);
			glBindTexture(GL_TEXTURE_2D, DepthTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, CubemapLength, CubemapLength, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


			glGenTextures(1, &ColorTexture);
			glBindTexture(GL_TEXTURE_2D, ColorTexture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexImage2D(GL_TEXTURE_2D, 0, SrcPF, CubemapLength, CubemapLength, 0, DestPF, PixelEleType, nullptr);
		};
		auto ForEndProcessMipmaplevel = [&](GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, float MipLevel, float Roughtness, int CubemapLength)
		{
			glDeleteTextures(1, &DepthTexture);
			glDeleteTextures(1, &ColorTexture);
		};
		auto ForBeginCubeMapFace = [&](int CubemapLength, GLenum DestPF, GLenum PixelEleType, int RoughtnessIndex, int CubemapIndex)
		{
			glGenFramebuffers(1, &NewFrameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, NewFrameBuffer);
			CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorTexture, 0));
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTexture, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		};
		auto ForEndCubeMapFace = [&](int CubemapLength, GLenum DestPF, GLenum PixelEleType, int RoughtnessIndex, int CubemapIndex)
		{
			CubeImageInfo[CubemapIndex].SetSize(CubemapLength, CubemapLength);
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glReadPixels(0, 0, CubemapLength, CubemapLength, DestPF, PixelEleType, CubeImageInfo[CubemapIndex].BitData);
			glPixelStorei(GL_PACK_ALIGNMENT, 4);
			std::string FileName = CubeImageInfo[CubemapIndex].FileName;
			CreateFileName(FileName, RoughtnessIndex, CubemapIndex);
			std::map<int, double> ToWebGL;
			ToWebGL[0] = 0.0;
			ToWebGL[1] = -180.0;
			ToWebGL[2] = -90.0;
			ToWebGL[3] = -90.0;
			ToWebGL[4] = -90.0;
			ToWebGL[5] = 90.0;
			IMGLoader.SaveToFile(FileName, &CubeImageInfo[CubemapIndex] , ToWebGL[CubemapIndex]);
			glDeleteFramebuffers(1, &NewFrameBuffer);
		};

		RTRenderer.ProcessCubeMap(RTRenderer.CubeMapTexture, 512, GL_RGBA32F, GL_BGRA, GL_UNSIGNED_BYTE, 4,
			BegeinProcess, EndProcess, ForBeginProcessMipmaplevel, ForEndProcessMipmaplevel, ForBeginCubeMapFace, ForEndCubeMapFace);
	}
	//return 0;

	
	{
		ImageFile CubeImageInfo[6];
		for (int i = 0; i < 6; i++)
		{
			CubeImageInfo[i].BitData = nullptr;
			CubeImageInfo[i].BPP = 32;
			CubeImageInfo[i].SetSize(512, 512);
			CubeImageInfo[i].FileFormat = FIF_PNG;
			CubeImageInfo[i].FileName = "TestIrradianc.png";
			CubeImageInfo[i].ImageColorType = FIC_RGBALPHA;
			CubeImageInfo[i].ImageFormat = FIT_BITMAP;
		}
		//生成diffuse cubemap data
		auto BeginProcess = [&](int CubeSize)
		{
			glGenTextures(1, &IrradianceMap);
			glBindTexture(GL_TEXTURE_CUBE_MAP, IrradianceMap);
			for (unsigned int i = 0; i < 6; ++i)
			{
				//glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, CubeSize, CubeSize, 0, GL_RGB, GL_FLOAT, nullptr);
				// 输出到文件
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA16F, CubeSize, CubeSize, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		};
		auto ProcessFace = [&](int FaceIndex)
		{
			CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + FaceIndex, IrradianceMap, 0));
		};
		auto EndProcessFace = [&](int CubemapLength, int CubemapIndex)
		{
			CubeImageInfo[CubemapIndex].SetSize(CubemapLength, CubemapLength);
			glPixelStorei(GL_PACK_ALIGNMENT, 1);
			glReadPixels(0, 0, CubemapLength, CubemapLength, GL_BGRA, GL_UNSIGNED_BYTE, CubeImageInfo[CubemapIndex].BitData);
			glPixelStorei(GL_PACK_ALIGNMENT, 4);
			std::string FileName = CubeImageInfo[CubemapIndex].FileName;
			CreateFileName(FileName, 0, CubemapIndex);
			std::map<int, float> ToWebGL;
			ToWebGL[0] = 0.0;
			ToWebGL[1] = -180.0;
			ToWebGL[2] = -90.0;
			ToWebGL[3] = -90.0;
			ToWebGL[4] = -90.0;
			ToWebGL[5] = 90.0;
			IMGLoader.SaveToFile(FileName, &CubeImageInfo[CubemapIndex], ToWebGL[CubemapIndex]);
		};

		RTRenderer.CreateIrradianceCubeMap(RTRenderer.CubeMapTexture, BeginProcess, ProcessFace, EndProcessFace);
		
	}
	return 0;
	{
		// 生成specular cubemap
		GLuint DepthTexture;
		GLuint NewFrameBuffer;
		auto BegeinProcess = [&](GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, float MaxMipLevel,int CubemapLength) 
		{
			glGenTextures(1, &SpecularCubemapTexture);
			glBindTexture(GL_TEXTURE_CUBE_MAP, SpecularCubemapTexture);
			// 这里的cubemap都是为HDR做准备的
			for (unsigned int i = 0; i < 6; i++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, SrcPF, CubemapLength, CubemapLength, 0, DestPF, PixelEleType, nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0));
			CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, MaxMipLevel - 1));//从0开始计数//开启这一项后会导致绑定到framebuffer上出现问题
			CHECK_GL_ERROR(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
		};
		auto EndProcess = [&](GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, float MaxMipLevel, int CubemapLength) 
		{

		};
		auto ForBeginProcessMipmaplevel = [&](GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, float MipLevel, float Roughtness, int CubemapLength)
		{
			glGenTextures(1, &DepthTexture);
			glBindTexture(GL_TEXTURE_2D, DepthTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, CubemapLength, CubemapLength, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		};
		auto ForEndProcessMipmaplevel = [&](GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, float MipLevel, float Roughtness, int CubemapLength)
		{
			glDeleteTextures(1, &DepthTexture);
		};
		auto ForBeginCubeMapFace = [&](int CubemapLength, GLenum DestPF, GLenum PixelEleType, int RoughtnessIndex, int CubemapIndex)
		{
			glGenFramebuffers(1, &NewFrameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, NewFrameBuffer);
			CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + CubemapIndex, SpecularCubemapTexture, RoughtnessIndex));
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTexture, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			GLenum statues = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		};
		auto ForEndCubeMapFace = [&](int CubemapLength, GLenum DestPF, GLenum PixelEleType, int RoughtnessIndex, int CubemapIndex)
		{
			glDeleteFramebuffers(1, &NewFrameBuffer);
		};

		RTRenderer.ProcessCubeMap(RTRenderer.CubeMapTexture, 128, GL_RGB16F, GL_RGB, GL_FLOAT, 5,
			BegeinProcess, EndProcess, ForBeginProcessMipmaplevel, ForEndProcessMipmaplevel, ForBeginCubeMapFace, ForEndCubeMapFace);
	}
	{
		//生成Unreal的Specular IBL
		GLuint DepthTexture;
		GLuint NewFrameBuffer;
		auto BegeinProcess = [&](GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, float MaxMipLevel, int CubemapLength)
		{
			glGenTextures(1, &UnrealSpecularCubemapTexture);
			glBindTexture(GL_TEXTURE_CUBE_MAP, UnrealSpecularCubemapTexture);
			// 这里的cubemap都是为HDR做准备的
			for (unsigned int i = 0; i < 6; i++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, SrcPF, CubemapLength, CubemapLength, 0, DestPF, PixelEleType, nullptr);
			}
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0));
			CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, MaxMipLevel));//从0开始计数//开启这一项后会导致绑定到framebuffer上出现问题
			CHECK_GL_ERROR(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
		};
		auto EndProcess = [&](GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, float MaxMipLevel, int CubemapLength)
		{

		};
		auto ForBeginProcessMipmaplevel = [&](GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, float MipLevel, int CubemapLength)
		{
			glGenTextures(1, &DepthTexture);
			glBindTexture(GL_TEXTURE_2D, DepthTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, CubemapLength, CubemapLength, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		};
		auto ForEndProcessMipmaplevel = [&](GLenum SrcPF, GLenum DestPF, GLenum PixelEleType, float MipLevel, int CubemapLength)
		{
			glDeleteTextures(1, &DepthTexture);
		};
		auto ForBeginCubeMapFace = [&](int CubemapLength, GLenum DestPF, GLenum PixelEleType, int MipLevel, int CubemapIndex)
		{
			glGenFramebuffers(1, &NewFrameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, NewFrameBuffer);
			CHECK_GL_ERROR(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + CubemapIndex, UnrealSpecularCubemapTexture, MipLevel));
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTexture, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			GLenum statues = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		};
		auto ForEndCubeMapFace = [&](int CubemapLength, GLenum DestPF, GLenum PixelEleType, int MipLevel, int CubemapIndex)
		{
			glDeleteFramebuffers(1, &NewFrameBuffer);
		};

		RTRenderer.UnrealProcessCubeMap(RTRenderer.CubeMapTexture, 128, GL_RGB16F, GL_RGB, GL_FLOAT, 7,
			BegeinProcess, EndProcess, ForBeginProcessMipmaplevel, ForEndProcessMipmaplevel, ForBeginCubeMapFace, ForEndCubeMapFace);

	}
	ImageFile *file = IMGLoader.GetImageFile(std::string("ibl_brdf_lut.png"));
	Img = file;

	RTRenderer.GenerateLUT(LUTTexture, 512);
	

	
	PBR = new PBRGLSL();
	PBR->SetShaderPreComplie(true, std::string("#version 430 core\r\n"));
	PBR->SetIsHaveBaseColorTexture(true);
	PBR->SetIsHaveNormalmapTexture(true);
	PBR->SetIsHaveMaterialTexture(true);
	//PBR->LoadProgram(std::string("Shader//PBR.vert"), std::string("Shader//PBR.frag"));
	PBR->LoadProgram(std::string("Shader//UnrealImageBaseLighting.vert"), std::string("Shader//UnrealImageBaseLighting.frag"));
	PBR->CubemapMaxMip = 5;
	for (int i = 0 ;i < ObjInfos.size() ; i++)
	{
		Obj = new Object();
		Obj->BaseColor = IMGLoader.GetImageFile(std::string("Teaport_DefaultMaterial_BaseColor.png"));
		Obj->NormalTexture = IMGLoader.GetImageFile(std::string("Teaport_DefaultMaterial_Normal.png"));
		Obj->MaterialTexture = IMGLoader.GetImageFile(std::string("Teaport_DefaultMaterial_OcclusionRoughnessMetallic.png"));
		//Obj->LoadObjModel("TeaportObj.obj", "us_barrel");
		Obj->LoadObjModel(ObjInfos[i], "us_barrel");
		RTRenderer.AddDrawable(Obj, PBR);
	}
	


	

	glutKeyboardFunc(&ProcessKeyboard);
	glutMotionFunc(mouse);
	glutMouseFunc(moueMoveFunction);
	glutDisplayFunc(display);
	glutMainLoop();
}