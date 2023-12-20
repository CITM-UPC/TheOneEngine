#ifndef __MESH_H__
#define __MESH_H__
#pragma once

#include "Defs.h"
#include "MeshLoader.h"
#include "Component.h"
#include "Texture.h"
#include "Graphic.h"

#include <vector>
#include <memory>
#include <string>

class GameObject;
struct MeshBufferData;

class Mesh : public Component, public Graphic
{
public:

    Mesh(std::shared_ptr<GameObject> containerGO);
    virtual ~Mesh();

    void DrawComponent();

    void GenerateAABB();

    json SaveComponent();

public:

    MeshBufferedData mesh;
    MeshData meshData;

    //now inside MeshBufferedData
    //std::vector<std::shared_ptr<Texture>> texture;

public:
    bool active;
    bool drawWireframe;
    bool drawAABB;
    bool drawOBB;
    bool drawChecker;

    bool drawNormalsVerts;
    bool drawNormalsFaces;
    int normalLineWidth;
    float normalLineLength;

private:
    
    void DrawAABB();

    void DrawVertexNormals();
    void DrawFaceNormals();

    //void DrawWireframe();
    void DrawOBB();
};

#endif // !__MESH_H__
