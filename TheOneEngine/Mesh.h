#ifndef MESH_H
#define MESH_H
#pragma once

#include "Defs.h"
#include "MeshLoader.h"
#include "Component.h"
#include "Texture.h"

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

    void DrawComponent();

public:

    std::vector<MeshBufferedData> meshes;
    Texture::Ptr texture;

};

#endif // !MESH_H