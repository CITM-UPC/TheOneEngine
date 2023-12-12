#ifndef __MESH_H__
#define __MESH_H__
#pragma once

#include "Defs.h"
#include "MeshLoader.h"
#include "Component.h"
#include "Texture.h"
#include "Graphic.h"

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

    MeshBufferedData mesh;

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
    int normalLineWidth = 1;
    float normalLineLength = 0.1f;
    std::vector<vec3f> meshVerts;
    std::vector<vec3f> meshNorms;
    std::vector<vec3f> meshFaceCenters;
    std::vector<vec3f> meshFaceNorms;

private:
    void DrawVertexNormals();
    void DrawFaceNormals();
    void DrawWireframe();
    void DrawAABB();
    void DrawOBB();
};

#endif // !__MESH_H__
