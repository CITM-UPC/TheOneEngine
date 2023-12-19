#ifndef __MESH_LOADER_H__
#define __MESH_LOADER_H__
#pragma once

#include "Defs.h"

#include <vector>
#include <array>
#include <memory>
#include <string>

class GameObject;
class Texture;

enum Formats { F_V3, F_V3C4, F_V3T2 };
struct V3 { vec3f v; };
struct V3C4 { vec3f v; vec4f c; };
struct V3T2 { vec3f v; vec2f t; };

struct MeshData
{
	Formats format;
	std::vector<V3T2> vertex_data;
	std::vector<unsigned int> index_data;
};

struct MeshBufferedData
{
	std::shared_ptr<GameObject> parent;

	std::string meshName;
	Formats format;

	uint vertex_buffer_id;
	uint numVerts;

	uint indexs_buffer_id;
	uint numIndexs;

	uint numFaces;

	std::string texturePath;
	std::shared_ptr<Texture> texture;
	uint materialIndex;
	//Texture::Ptr checkboard = std::shared_ptr<Texture>(new Texture); // JULS: for the checkers texture
};

class MeshLoader
{
public:

	MeshLoader();
	//MeshLoader(MeshLoader&& b) noexcept;
	virtual ~MeshLoader();

	std::vector<MeshBufferedData> LoadMesh(const std::string& path);
	std::vector<std::shared_ptr<Texture>> LoadTexture(const std::string& path, std::shared_ptr<GameObject> containerGO = nullptr);

	void BufferData(MeshData meshData);

	void serializeMeshBufferedData(const MeshBufferedData& data, const std::string& filename);
	MeshBufferedData deserializeMeshBufferedData(const std::string& filename);

private:

	/*MeshLoader(const MeshLoader& cpy);
	MeshLoader& operator=(const MeshLoader&);*/

private:

	MeshData meshData;
	MeshBufferedData meshBuffData;

};

#endif // !__MESH_LOADER_H__