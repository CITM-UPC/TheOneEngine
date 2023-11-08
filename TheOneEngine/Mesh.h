#ifndef __MESH_H__
#define __MESH_H__
#pragma once

#include "Defs.h"
#include "Component.h"
#include "MeshLoader.h"

#include <string>
#include <vector>
#include <memory>

class GameObject;
struct MeshBufferData;

class Mesh : public Component
{
public:

	Mesh(std::shared_ptr<GameObject> containerGO);
	virtual ~Mesh();

	void DrawMesh(MeshLoader::MeshBufferedData mesh);

public:

	std::vector<MeshLoader::MeshBufferedData> meshes;
	Texture::Ptr texture;

};

#endif // !__MESH_H__