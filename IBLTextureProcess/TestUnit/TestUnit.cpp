#include "TestUnit.h"
#include "..\JsonLoader\JsonLoader.h"
#include <iostream>
#include "..\ObjFileLoader\ObjFileLoader.h"
#include <fstream>

void CoverShapesparkJsonToObjMtl::Test()
{
	JsonLoader JsonFileLoader;
	Json::Value Root;
	std::string FileName = "ShapesparkScene.json";
	bool LoadState = JsonFileLoader.LoadJsonFile(FileName, Root);
	if (!LoadState) 
	{
		std::cout << "load " << FileName << "  Fail!!" << std::endl;
		return;
	}
	std::vector<ShapeSparkMaterial> ShapesparkMaterials;
	if (!ProcessJsonInfo(Root, ShapesparkMaterials))
	{
		std::cout << "Process Json File Fail!!" << std::endl;
		return;
	}
	ObjFileLoader ObjMaterialLoder;
	std::vector<tinyobj::material_t> ObjMaterials;
	ObjMaterialLoder.LoadMaterialDataFromFile("SrcDemoscene.mtl", ObjMaterials);
	MergeShapesparkMaterialsInfoToObjMaterialInfo(ShapesparkMaterials, ObjMaterials);

	std::string OutFileName = "Demoscene.mtl";
	SaveObjMaterialToFile(OutFileName, ObjMaterials);
}

bool CoverShapesparkJsonToObjMtl::ProcessJsonInfo(Json::Value &JsonFile, std::vector<ShapeSparkMaterial> &ShapeSparkMaterials)
{
	int MaterialSize = JsonFile["materials"].size();
	for (int i = 0 ; i < MaterialSize; i++)
	{
		ShapeSparkMaterial MaterialInfo;
		Json::Value Material = JsonFile["materials"][i];
		MaterialInfo.MaterialName = Material["name"].asString();
		if (!Material["baseColorTexture"].isNull())
		{
			MaterialInfo.BaseColorTextureName = Material["baseColorTexture"]["name"].asString() + "." + Material["baseColorTexture"]["rawExt"].asString();
		}
		if (!Material["roughnessTexture"].isNull())
		{
			MaterialInfo.RoughtnessTextureName = Material["roughnessTexture"]["name"].asString() + "." + Material["roughnessTexture"]["rawExt"].asString();
		}
		if (!Material["metallicTexture"].isNull())
		{
			MaterialInfo.MetallicTextureName = Material["metallicTexture"]["name"].asString() + "." + Material["metallicTexture"]["rawExt"].asString();
		}
		if (!Material["bumpTexture"].isNull())
		{
			MaterialInfo.NormalMapTexture = Material["bumpTexture"]["name"].asString() + "." + Material["bumpTexture"]["rawExt"].asString();
		}
		if (!Material["metallic"].isNull())
		{
			MaterialInfo.Metallic = Material["metallic"].asFloat();
		}
		if (!Material["roughness"].isNull())
		{
			MaterialInfo.Roughtness = Material["roughness"].asFloat();
		}
		if (!Material["opacity"].isNull())
		{
			MaterialInfo.Opacity = Material["opacity"].asFloat();
		}
		ShapeSparkMaterials.push_back(MaterialInfo);
	}
	return true;
}

void CoverShapesparkJsonToObjMtl::MergeShapesparkMaterialsInfoToObjMaterialInfo(std::vector<ShapeSparkMaterial> &ShapesparkMaterials, std::vector<tinyobj::material_t> &ObjMaterials)
{
	for (ShapeSparkMaterial &ShapesparkMaterial : ShapesparkMaterials)
	{
		for (tinyobj::material_t &ObjMaterial : ObjMaterials)
		{
			if (ShapesparkMaterial.MaterialName == ObjMaterial.name)
			{
				ObjMaterial.diffuse_texname = ShapesparkMaterial.BaseColorTextureName;
				ObjMaterial.roughness_texname = ShapesparkMaterial.RoughtnessTextureName;
				ObjMaterial.metallic_texname = ShapesparkMaterial.MetallicTextureName;
				ObjMaterial.metallic = ShapesparkMaterial.Metallic;
				ObjMaterial.roughness = ShapesparkMaterial.Roughtness;
				ObjMaterial.dissolve = ShapesparkMaterial.Opacity;
			}
		}
	}
}

void CoverShapesparkJsonToObjMtl::SaveObjMaterialToFile(std::string &FileName, std::vector<tinyobj::material_t> &ObjMaterials)
{
	std::ofstream ObjMaterialFile(FileName.c_str());
	if (ObjMaterialFile)
	{
		for (tinyobj::material_t &ObjMaterial : ObjMaterials)
		{
			ObjMaterialFile << "newmtl " << ObjMaterial.name << std::endl;
			ObjMaterialFile << "	Ns " << ObjMaterial.shininess << std::endl;
			ObjMaterialFile << "	Ni " << ObjMaterial.ior << std::endl;
			ObjMaterialFile << "	d " << ObjMaterial.dissolve << std::endl;
			ObjMaterialFile << "	Tr " << 1.0 - ObjMaterial.transmittance[0] << std::endl;
			ObjMaterialFile << "	Tf " << ObjMaterial.transmittance[0] << " " << ObjMaterial.transmittance[1] << " " << ObjMaterial.transmittance[2] << std::endl;
			ObjMaterialFile << "	illum " << ObjMaterial.illum << std::endl;
			ObjMaterialFile << "	Ka " << ObjMaterial.ambient[0] << " "<< ObjMaterial.ambient[1] << " "<< ObjMaterial.ambient[2] << std::endl;
			ObjMaterialFile << "	Kd " << ObjMaterial.diffuse[0] << " " << ObjMaterial.diffuse[1] << " " << ObjMaterial.diffuse[2] << std::endl;
			ObjMaterialFile << "	Ks " << ObjMaterial.specular[0] << " " << ObjMaterial.specular[1] << " " << ObjMaterial.specular[2] << std::endl;
			ObjMaterialFile << "	Ke " << ObjMaterial.emission[0] << " " << ObjMaterial.emission[1] << " " << ObjMaterial.emission[2] << std::endl;
			ObjMaterialFile << "	Pr " << ObjMaterial.roughness << std::endl;
			ObjMaterialFile << "	Pm " << ObjMaterial.metallic << std::endl;
			if (ObjMaterial.diffuse_texname != "")
			{
				ObjMaterialFile << "	map_Kd " << "map\\"<<ObjMaterial.diffuse_texname << std::endl;
			}
			if (ObjMaterial.roughness_texname != "")
			{
				ObjMaterialFile << "	map_Pr " << "map\\" <<ObjMaterial.roughness_texname << std::endl;
			}
			if (ObjMaterial.metallic_texname != "")
			{
				ObjMaterialFile << "	map_Pm " << "map\\" <<ObjMaterial.metallic_texname << std::endl;
			}	
			ObjMaterialFile << std::endl;
		}
		ObjMaterialFile.close();
	}

}
