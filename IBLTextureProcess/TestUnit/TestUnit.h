#pragma once
#include <string>
#include "json\writer.h"
#include "tiny_obj_loader.h"
class CoverShapesparkJsonToObjMtl
{
public:
	struct ShapeSparkMaterial
	{
		std::string MaterialName;
		float Metallic;
		float Roughtness;
		float Opacity;
		std::string BaseColorTextureName;
		std::string RoughtnessTextureName;
		std::string MetallicTextureName;
		std::string NormalMapTexture;
	};
	void Test();
	bool ProcessJsonInfo(Json::Value &JsonFile , std::vector<ShapeSparkMaterial> &ShapeSparkMaterials);
	void MergeShapesparkMaterialsInfoToObjMaterialInfo(std::vector<ShapeSparkMaterial> &ShapesparkMaterials, std::vector<tinyobj::material_t> &ObjMaterials);
	void SaveObjMaterialToFile(std::string &FileName, std::vector<tinyobj::material_t> &ObjMaterials);
};
