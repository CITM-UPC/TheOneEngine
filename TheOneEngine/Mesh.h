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

    void GenerateAABB();
    AABBox CalculateAABB(GameObject& gameObject);
    AABBox CalculateAABBWithChildren(GameObject& gameObject);

    void DrawComponent();

    json SaveComponent();
    void LoadComponent(const json& meshJSON);

private:

    void DrawAABB();

    void DrawVertexNormals();
    void DrawFaceNormals();

    //void DrawWireframe();
    void DrawOBB();

public:

    MeshBufferedData mesh;
    MeshData meshData;

    bool active;
    bool drawWireframe;
    bool drawAABB;
    bool drawOBB;
    bool drawChecker;

    bool drawNormalsVerts;
    bool drawNormalsFaces;
    int normalLineWidth;
    float normalLineLength;

    std::string path;

private:

    MeshLoader* meshLoader;
};

#endif // !__MESH_H__
