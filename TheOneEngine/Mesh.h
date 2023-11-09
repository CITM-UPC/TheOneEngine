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

    //now inside MeshBufferedData
    //std::vector<std::shared_ptr<Texture>> texture;

};

#endif // !MESH_H