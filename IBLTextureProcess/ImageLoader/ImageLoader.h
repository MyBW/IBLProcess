#pragma once
#include <map>
#include <assert.h>
#include "FreeImage.h"

struct ImageFile 
{
	std::string FileName;
	unsigned int Height;
	unsigned int Width;
	unsigned int ByteSize;
	unsigned char BPP;  // bit/pre pixel
	unsigned char *BitData;
	FREE_IMAGE_FORMAT FileFormat;
	FREE_IMAGE_TYPE ImageFormat;
	FREE_IMAGE_COLOR_TYPE ImageColorType;
	ImageFile* GetSubImageFile(const std::string &NewImageName, int StartX, int StartY, int Width, int Height);
	unsigned char GetPixelSizeByByte();
	bool CheckCondition(int StartX, int StartY, int Width, int Height);
	void CopyWihoutData(ImageFile *Dest);
	void SetSize(int Width, int Height);
};
class ImageLoader
{
public:
	ImageLoader();
	~ImageLoader();
	ImageFile* GetImageFile(const std::string &FileName) const;
	bool SaveToFile(const std::string &FileName, const ImageFile *ImgData, double RotateAngle = 0);
	void SetIsFilpImage(bool InIsFlipImage) { IsFlipImage = InIsFlipImage; }
private:
	FIBITMAP* EncodeImageData(const ImageFile *ImgData) const;
	FIMEMORY* GetImageFileData(const std::string &FileName) const;
	static void InitLoader();
	static std::map<std::string, FREE_IMAGE_FORMAT> SupportedImageFormat;
	bool IsFlipImage; // FreeImage 默认左下角为原点
	
};