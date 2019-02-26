#include "ObjFileLoader.h"
std::vector<ObjFileLoader::ObjInfo> ObjFileLoader::OpenFile(std::string FileName)
{
	std::vector<ObjFileLoader::ObjInfo> ObjInfos;
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn;
	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, FileName.c_str());
	int CurrentMaterialID = -2;
	ObjInfo CurrentInfo;
	for (size_t s = 0; s < shapes.size(); s++)
	{
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			int fv = shapes[s].mesh.num_face_vertices[f];

			if (CurrentMaterialID != shapes[s].mesh.material_ids[f])
			{
				if (CurrentInfo.PositionData.size() != 0)
				{
					ObjInfos.push_back(CurrentInfo);
					if (shapes[s].mesh.material_ids[f] != -1)
					{
						CurrentInfo.BaseColor = materials[shapes[s].mesh.material_ids[f]].diffuse_texname;
						//Info.NormalMap = materials[shapes[s].mesh.material_ids[0]].specular_texname;
						CurrentInfo.NormalMap = materials[shapes[s].mesh.material_ids[f]].normal_texname;
						CurrentInfo.RoughnessMap = materials[shapes[s].mesh.material_ids[f]].roughness_texname;
						//materials[shapes[s].mesh.material_ids[0]].metallic_texname;
					}
				}
				CurrentInfo.ClearAllData();
				CurrentMaterialID = shapes[s].mesh.material_ids[f];
			}
			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++)
			{
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				BWVector3D Vertex;
				for (int i = 0; i < 3; i++)
				{
					Vertex[i] = attrib.vertices[3 * idx.vertex_index + i];
				}
				CurrentInfo.PositionData.push_back(Vertex);
				/*tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];*/

				if (attrib.normals.size() != 0)
				{
					BWVector3D Normal;
					for (int i = 0; i < 3; i++)
					{
						Normal[i] = attrib.normals[3 * idx.normal_index + i];
					}
					CurrentInfo.NormalData.push_back(Normal);
					/*tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
					tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
					tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];*/
				}
				if (attrib.texcoords.size() != 0)
				{
					BWPoint2DD UV;
					UV.x = attrib.texcoords[2 * idx.texcoord_index + 0];
					UV.y = attrib.texcoords[2 * idx.texcoord_index + 1];
					CurrentInfo.UVData.push_back(UV);
					/*tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
					tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];*/
				}
			}
			index_offset += fv;
		}
	}
	if (CurrentInfo.PositionData.size() != 0)
	{
		ObjInfos.push_back(CurrentInfo);
	}
	return ObjInfos;
}
