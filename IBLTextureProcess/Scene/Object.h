#pragma once
#include <vector>
#include <string>
#include "..\Math\BWPrimitive.h"
#include "..\ImageLoader\ImageLoader.h"
#include "..\ObjFileLoader\ObjFileLoader.h"
struct TriangleInfo
{
	std::vector<BWVector3D> P;
	std::vector<BWVector3D> N;
	std::vector<BWPoint2DD> UV;
	void ClearData()
	{
		P.clear();
		N.clear();
		UV.clear();
	}
};
class Object
{
public:
	Object();
	bool LoadObjModel(ObjFileLoader::ObjInfo &info, const std::string &Name);
	bool LoadObjModel(const char* FileName , const std::string &Name);
	
	std::string& GetName() { return Name; }
	void SetPosition(float X, float Y, float Z);
	void SetRotation(float w, float x, float y, float z);
	void SetRoataion(const BWVector3D& axis, const Radian &angle);
	void SetScale(float x, float y, float z);
	int GetTriangleNum();
	bool GetTriangleInfoByIndex(int Index , TriangleInfo &Reslut);
	bool GetTriangleVertexIndex(int Index, int &P1, int &P2, int &P3);
	const BWMatrix4& GetModelMatrix() const;
	const std::vector<BWVector3D>& GetPosData() const;
	const std::vector<BWPoint2DD>& GetUVData() const;
	const std::vector<BWVector3D>& GetNormalData() const;
	const std::vector<unsigned int>& GetIndexData() const;
	const std::vector<BWPoint2DD>& GetLightmapUVData() const;
	void GetAABB(BWVector3D &Min, BWVector3D &Max) const;
	ImageFile *BaseColor;
	ImageFile *NormalTexture;
	ImageFile *MaterialTexture;
private:
	void UpdateModelMatrix();
	void ComputeBoundingBox();
	void UpdateAABB();
	BWVector3D BoundingBoxMin;
	BWVector3D BoundingBoxMax;
	BWVector3D AABBMin;
	BWVector3D AABBMax;
	std::string Name;
	BWMatrix4 ModelMatrix;
	BWVector3D Position;
	BWVector3D Scale;
	BWQuaternion Orientation;

	std::vector<BWVector3D> PosData;
	std::vector<BWPoint2DD> UVData;
	std::vector<BWVector3D> NormalData;
	std::vector<unsigned int> IndexData;
	std::vector<BWPoint2DD> LightmapUV;
	
};
