#pragma once
#include <string>
#include <vector>
#include "..\Math\BWPrimitive.h"
#include "tiny_obj_loader.h"
class ObjFileLoader
{
public:
	struct ObjInfo 
	{
		std::vector<BWVector3D> PositionData;
		std::vector<BWPoint2DD> UVData;
		std::vector<BWVector3D> NormalData;
		std::string BaseColor;
		std::string NormalMap;
		std::string RoughnessMap;
		void ClearAllData()
		{
			PositionData.clear();
			UVData.clear();
			NormalData.clear();
			BaseColor.clear();
			NormalMap.clear();
			RoughnessMap.clear();
		}
	};
	std::vector<ObjInfo> OpenFile(std::string FileName);
};
