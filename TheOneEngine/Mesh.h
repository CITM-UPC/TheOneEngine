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
    
    std::vector<vec3f> meshVerts;
    std::vector<vec3f> meshNorms;
    std::vector<vec3f> meshFaceCenters;
    std::vector<vec3f> meshFaceNorms;
    
    bool drawActive;
    bool drawWireframe;
    bool drawAABB;
    bool drawOBB;

    bool drawNormalsVerts;
    bool drawNormalsFaces;
    int normalLineWidth = 1;
    float normalLineLength = 0.1f;
};

#endif // !MESH_H