#include "ImageLoader.h"
#include "FreeImage.h"
#include "..\Util\BWStringOperation.h"
#include <fstream>


void FreeImageLoadErrorHandler(FREE_IMAGE_FORMAT fif, const char *message)
{
	// Callback method as required by FreeImage to report problems
	const char* typeName = FreeImage_GetFormatFromFIF(fif);
	if (typeName)
	{
		assert(0);
	}
	else
	{
		return;
	}
}

std::map<std::string, FREE_IMAGE_FORMAT> ImageLoader::SupportedImageFormat;
ImageLoader::ImageLoader()
{
	IsFlipImage = false;
	InitLoader();
}

ImageLoader::~ImageLoader()
{
	FreeImage_DeInitialise();
}

FIMEMORY* ImageLoader::GetImageFileData(const std::string &FileName) const
{
	std::ifstream filestream;
	filestream.open(FileName.c_str(), std::ios_base::in | std::ios_base::binary);
	if (!filestream.is_open())
	{
		return NULL;
	}
	filestream.seekg(0, std::ios_base::end);
	DWORD size = (DWORD)filestream.tellg();
	filestream.seekg(0, std::ios_base::beg);
	unsigned char *Data = new unsigned char[size]; //FIM auto realese
	filestream.read((char*)(Data), static_cast<std::streamsize>(size));
	FIMEMORY* fiMem = FreeImage_OpenMemory(Data, static_cast<DWORD>(size));
	return fiMem;
}

FIBITMAP* ImageLoader::EncodeImageData(const ImageFile *ImgData) const
{
	// Set error handler
	FreeImage_SetOutputMessage(FreeImageLoadErrorHandler);

	FIBITMAP* ret = 0;
	unsigned char* srcData = ImgData->BitData;
	ret = FreeImage_AllocateT( ImgData->ImageFormat, ImgData->Width, ImgData->Height, ImgData->BPP);
	if (!ret)
	{
		assert(0);
	}
	size_t dstPitch = FreeImage_GetPitch(ret);
	size_t srcPitch = ImgData->Width * ImgData->BPP / 8;
	unsigned char* pSrc;
	unsigned char* pDst = FreeImage_GetBits(ret);
	for (size_t y = 0; y < ImgData->Height; ++y)
	{
		pSrc = srcData + (ImgData->Height - y - 1) * srcPitch;
		memcpy(pDst, pSrc, srcPitch);
		pDst += dstPitch;
	}
	return ret;
}

ImageFile* ImageLoader::GetImageFile(const std::string &FileName) const
{
	
	FIMEMORY* FileMem = GetImageFileData(FileName);
	if (!FileMem) return nullptr;

	ImageFile *RtFile = new ImageFile();
	RtFile->FileName = FileName;
	RtFile->FileFormat = FreeImage_GetFileType(FileName.data());
	FIBITMAP* FIBitMap = FreeImage_LoadFromMemory(RtFile->FileFormat, FileMem);
	if (IsFlipImage)
	{
		FreeImage_FlipVertical(FIBitMap);
	}
	RtFile->ImageFormat = FreeImage_GetImageType(FIBitMap);
	RtFile->Height = FreeImage_GetHeight(FIBitMap);
	RtFile->Width = FreeImage_GetWidth(FIBitMap);
	RtFile->ImageColorType = FreeImage_GetColorType(FIBitMap);
	RtFile->BPP = FreeImage_GetBPP(FIBitMap);

	if (RtFile->ImageColorType == FIC_MINISWHITE || RtFile->ImageColorType == FIC_MINISBLACK)
	{
		FIBITMAP* newBitmap = FreeImage_ConvertToGreyscale(FIBitMap);
		// free old bitmap and replace
		FreeImage_Unload(FIBitMap);
		FIBitMap = newBitmap;
		// get new formats
		RtFile->BPP = FreeImage_GetBPP(FIBitMap);
		RtFile->ImageColorType = FreeImage_GetColorType(FIBitMap);
	}
	// Perform any colour conversions for RGB
	else if (RtFile->BPP < 8 || RtFile->ImageColorType == FIC_PALETTE || RtFile->ImageColorType == FIC_CMYK)
	{
		FIBITMAP* newBitmap = FreeImage_ConvertTo24Bits(FIBitMap);
		// free old bitmap and replace
		FreeImage_Unload(FIBitMap);
		FIBitMap = newBitmap;
		RtFile->BPP = FreeImage_GetBPP(FIBitMap);
		RtFile->ImageColorType = FreeImage_GetColorType(FIBitMap);
	}

//	switch (RtFile->FileFormat)
//	{
//	case FIT_BITMAP:
//		if (RtFile->ImageColorType == FIC_MINISWHITE || RtFile->ImageColorType == FIC_MINISBLACK)
//		{
//			FIBITMAP* newBitmap = FreeImage_ConvertToGreyscale(FIBitMap);
//			// free old bitmap and replace
//			FreeImage_Unload(FIBitMap);
//			FIBitMap = newBitmap;
//			// get new formats
//			RtFile->BPP = FreeImage_GetBPP(FIBitMap);
//			RtFile->ImageColorType = FreeImage_GetColorType(FIBitMap);
//		}
//		// Perform any colour conversions for RGB
//		else if (RtFile->BPP < 8 || RtFile->ImageColorType == FIC_PALETTE || RtFile->ImageColorType == FIC_CMYK)
//		{
//			FIBITMAP* newBitmap = FreeImage_ConvertTo24Bits(FIBitMap);
//			// free old bitmap and replace
//			FreeImage_Unload(FIBitMap);
//			FIBitMap = newBitmap;
//			RtFile->BPP = FreeImage_GetBPP(FIBitMap);
//			RtFile->ImageColorType = FreeImage_GetColorType(FIBitMap);
//		}
//
//		// by this stage, 8-bit is greyscale, 16/24/32 bit are RGB[A]
//		switch (RtFile->BPP)
//		{
//		case 8:
//			imgData->format = PF_L8;
//			break;
//		case 16:
//			// Determine 555 or 565 from green mask
//			// cannot be 16-bit greyscale since that's FIT_UINT16
//			if (FreeImage_GetGreenMask(fiBitmap) == FI16_565_GREEN_MASK)
//			{
//				imgData->format = PF_R5G6B5;
//			}
//			else
//			{
//				// FreeImage doesn't support 4444 format so must be 1555
//				imgData->format = PF_A1R5G5B5;
//			}
//			break;
//		case 24:
//			// FreeImage differs per platform
//			//     PF_BYTE_BGR[A] for little endian (== PF_ARGB native)
//			//     PF_BYTE_RGB[A] for big endian (== PF_RGBA native)
//#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
//			imgData->format = PF_BYTE_RGB;
//#else
//			imgData->format = PF_BYTE_BGR;
//#endif
//			break;
//		case 32:
//#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
//			imgData->format = PF_BYTE_RGBA;
//#else
//			imgData->format = PF_BYTE_BGRA;
//#endif
//			break;
//
//
//		};
//		break;
//	case FIT_UINT16:
//	case FIT_INT16:
//		// 16-bit greyscale
//		imgData->format = PF_L16;
//		break;
//	case FIT_FLOAT:
//		// Single-component floating point data
//		imgData->format = PF_FLOAT32_R;
//		break;
//	case FIT_RGB16:
//		imgData->format = PF_SHORT_RGB;
//		break;
//	case FIT_RGBA16:
//		imgData->format = PF_SHORT_RGBA;
//		break;
//	case FIT_RGBF:
//		imgData->format = PF_FLOAT32_RGB;
//		break;
//	case FIT_RGBAF:
//		imgData->format = PF_FLOAT32_RGBA;
//		break;
//	default:
//		assert(0);
//		break;
//
//	};
    

	unsigned char* SrcData = FreeImage_GetBits(FIBitMap);
	unsigned SrcPitch = FreeImage_GetPitch(FIBitMap);

	// Final data - invert image and trim pitch at the same time
	size_t DstPitch = RtFile->Width * RtFile->BPP/8;
	RtFile->ByteSize = DstPitch * RtFile->Height;
	RtFile->BitData = new unsigned char[RtFile->ByteSize];

	unsigned char* pSrc;
	unsigned char* pDst = RtFile->BitData;
	for (size_t y = 0; y < RtFile->Height; ++y)
	{
		pSrc = SrcData + (RtFile->Height - y - 1) * SrcPitch;
		memcpy(pDst, pSrc, DstPitch);
		pDst += DstPitch;
	}


	FreeImage_Unload(FIBitMap);
	FreeImage_CloseMemory(FileMem);

	return RtFile;
}


bool ImageLoader::SaveToFile(const std::string &FileName, const ImageFile *ImgData , double RotateAngle )
{
	FIBITMAP *FIBitMap = EncodeImageData(ImgData);
	if (FIBitMap)
	{
		if (RotateAngle != 0)
		{
			FIBitMap = FreeImage_Rotate(FIBitMap, RotateAngle);
		}
		FreeImage_Save(ImgData->FileFormat, FIBitMap, FileName.c_str(), PNG_Z_NO_COMPRESSION);
		FreeImage_Unload(FIBitMap);
		return true;
	}
	return false;
}

void ImageLoader::InitLoader()
{
	if (SupportedImageFormat.size() != 0) return;
	FreeImage_Initialise(false);
	// Register codecs
	for (int i = 0; i < FreeImage_GetFIFCount(); ++i)
	{

		std::string exts(FreeImage_GetFIFExtensionList((FREE_IMAGE_FORMAT)i));
		StringVector extsVector = StringUtil::Split(exts, ",");
		for (StringVector::iterator v = extsVector.begin(); v != extsVector.end(); ++v)
		{
			// FreeImage 3.13 lists many formats twice: once under their own codec and
			// once under the "RAW" codec, which is listed last. Avoid letting the RAW override
			// the dedicated codec!
			if (SupportedImageFormat.find(*v) == SupportedImageFormat.end())
			{
				SupportedImageFormat[*v] = (FREE_IMAGE_FORMAT)i;
			}
		}
	}
	// Set error handler
	FreeImage_SetOutputMessage(FreeImageLoadErrorHandler);
}


ImageFile* ImageFile::GetSubImageFile(const std::string &NewImageName, int StartX, int StartY, int Width, int Height)
{
	if (!CheckCondition(StartX, StartY, Width, Height))
	{
		return nullptr;
	}
	ImageFile *NewImg = new ImageFile();
	CopyWihoutData(NewImg);
	NewImg->FileName = NewImageName;
	NewImg->Width = Width;
	NewImg->Height = Height;
	NewImg->ByteSize = Width * Height * NewImg->GetPixelSizeByByte();
	NewImg->BitData = new unsigned char[NewImg->ByteSize];
	unsigned char *pSrc = this->BitData + (StartY * this->Width  + StartX)* NewImg->BPP / 8;
	unsigned char *pDest = NewImg->BitData;
	for (int i = 0 ;i < Height ; i++)
	{
		unsigned char *Src = pSrc + i * this->Width * GetPixelSizeByByte();
		unsigned char *Des = pDest + i * NewImg->Width * NewImg->GetPixelSizeByByte();
		memcpy(Des, Src, Width * NewImg->GetPixelSizeByByte());
	}
	return NewImg;

}

unsigned char ImageFile::GetPixelSizeByByte()
{
	return BPP / 8;
}

bool ImageFile::CheckCondition(int StartX, int StartY, int Width, int Height)
{
	auto Check = [](int CheckData , int Condition)->bool
	{
		return (CheckData > -1 && CheckData < Condition);
	};
	if (Check(StartX, this->Width) 
		&& Check(StartY, this->Height)
		&& Check(StartX + Width - 1, this->Width)
		&& Check(StartY + Height - 1, this->Height))
	{
		return true;
	}
	return false;
}

void ImageFile::CopyWihoutData(ImageFile *Dest)
{
#define COPY(DEST, DATA)\
     DEST->DATA = DATA

	Dest->BitData = nullptr;
	COPY(Dest, FileName);
	COPY(Dest, FileFormat);
	COPY(Dest, ImageFormat);
	COPY(Dest, ImageColorType);
	COPY(Dest, Height);
	COPY(Dest, Width);
	COPY(Dest, ByteSize);
	COPY(Dest, BPP);
#undef  COPY
}

void ImageFile::SetSize(int Width, int Height)
{
	this->Width = Width;
	this->Height = Height;
	ByteSize = Width * Height * BPP / 8;
	if (BitData)
	{
		delete BitData;
		BitData = nullptr;
	}
	BitData = new unsigned char[ByteSize];
}
