#include "Mesh.h"
#include "../TheOneEditor/Log.h"
#include "../TheOneEditor/SceneManager.h"
#include "GameObject.h"
#include "Transform.h"
//#include <GL/glew.h>
#include <span>
#include <vector>
#include <array>
#include <cstdio>
#include <cassert>
using namespace std;

Mesh::Mesh(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Mesh) {
    drawNormalsFaces = false;
    drawNormalsVerts = false;
    drawAABB = true;
    drawChecker = false;

    normalLineWidth = 1;
    normalLineLength = 0.1f;

    //GenerateAABB();
}

Mesh::~Mesh()
{
    /*if (_vertex_buffer_id) glDeleteBuffers(1, &_vertex_buffer_id);
    if (_indexs_buffer_id) glDeleteBuffers(1, &_indexs_buffer_id);*/
}

void Mesh::DrawComponent()
{
    if (auto gameObject = GetContainerGO())
    {
        glPushMatrix();
        glMultMatrixd(&gameObject.get()->GetComponent<Transform>()->getMatrix()[0].x);
    }
    
    glColor4ub(255, 255, 255, 255);

    // uncomment the line below for WIREFRAME MODE
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer_id);
    glEnableClientState(GL_VERTEX_ARRAY);

    if (drawAABB) DrawAABB();

    switch (mesh.format)
    {
    case Formats::F_V3:
        glVertexPointer(3, GL_FLOAT, 0, nullptr);
        break;

    case Formats::F_V3C4:
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(V3C4), nullptr);
        glColorPointer(4, GL_FLOAT, sizeof(V3C4), (void*)sizeof(V3));
        break;

    case Formats::F_V3T2:
        glEnable(GL_TEXTURE_2D);
        if (mesh.texture.get() && !drawChecker) mesh.texture->bind();
        //else mesh.checkboard.get()->bind();

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(V3T2), nullptr);
        glTexCoordPointer(2, GL_FLOAT, sizeof(V3T2), (void*)sizeof(V3));
        break;
    }

    if (mesh.indexs_buffer_id) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexs_buffer_id);
        glDrawElements(GL_TRIANGLES, mesh.numIndexs, GL_UNSIGNED_INT, nullptr);
    }
    else
        glDrawArrays(GL_TRIANGLES, 0, mesh.numVerts);
    
    if (drawNormalsVerts && !meshData.meshVerts.empty() && !meshData.meshNorms.empty())
        DrawVertexNormals();

    if (drawNormalsFaces && !meshData.meshFaceCenters.empty() && !meshData.meshFaceNorms.empty())
        DrawFaceNormals();

    

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);


    GLenum error = glGetError();

    glPopMatrix();
}

void Mesh::DrawVertexNormals() 
{
    glLineWidth(normalLineWidth);
    glBegin(GL_LINES);
    glColor3f(1.0f, 1.0f, 0.0f);

    for (int i = 0; i < meshData.meshVerts.size(); i++) {
        glVertex3f(meshData.meshVerts[i].x, meshData.meshVerts[i].y, meshData.meshVerts[i].z);
        glVertex3f(meshData.meshVerts[i].x + meshData.meshNorms[i].x * normalLineLength,
            meshData.meshVerts[i].y + meshData.meshNorms[i].y * normalLineLength,
            meshData.meshVerts[i].z + meshData.meshNorms[i].z * normalLineLength);
    }

    glColor3f(1.0f, 1.0f, 0.0f);
    glEnd();
}

void Mesh::DrawFaceNormals() 
{
    glLineWidth(normalLineWidth);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 1.0f);

    for (int i = 0; i < meshData.meshFaceCenters.size(); i++) {
        glm::vec3 endPoint = meshData.meshFaceCenters[i] + normalLineLength * meshData.meshFaceNorms[i];
        glVertex3f(meshData.meshFaceCenters[i].x, meshData.meshFaceCenters[i].y, meshData.meshFaceCenters[i].z);
        glVertex3f(endPoint.x, endPoint.y, endPoint.z);
    }

    glColor3f(0.0f, 1.0f, 1.0f);
    glEnd();
}

void Mesh::GenerateAABB() {
    glGenBuffers(1, &mesh.vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer_id);
    
    switch (mesh.format) {
    case Formats::F_V3:
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3) * mesh.numVerts, meshData.vertex_data.data(), GL_STATIC_DRAW);
        for (const auto& v : span((V3*)meshData.vertex_data.data(), meshData.vertex_data.size())) {
            aabb.min = glm::min(aabb.min, vec3(v.v));
            aabb.max = glm::max(aabb.max, vec3(v.v));
        }
        break;

    case Formats::F_V3C4:
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3C4) * mesh.numVerts, meshData.vertex_data.data(), GL_STATIC_DRAW);
        for (const auto& v : span((V3C4*)meshData.vertex_data.data(), meshData.vertex_data.size())) {
            aabb.min = glm::min(aabb.min, vec3(v.v));
            aabb.max = glm::max(aabb.max, vec3(v.v));
        }
        break;

    case Formats::F_V3T2:
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3T2) * mesh.numVerts, meshData.vertex_data.data(), GL_STATIC_DRAW);
        for (const auto& v : span((V3T2*)meshData.vertex_data.data(), meshData.vertex_data.size())) {
            aabb.min = glm::min(aabb.min, vec3(v.v));
            aabb.max = glm::max(aabb.max, vec3(v.v));
        }
        break;
    }

    //GLenum error = glGetError();
    //if (error != GL_NO_ERROR) {
    //    // Print the raw error code
    //    fprintf(stderr, "OpenGL error code: %d\n", error);
    //
    //    // Print the corresponding error string
    //    const char* errorString = reinterpret_cast<const char*>(gluErrorString(error));
    //    fprintf(stderr, "OpenGL error: %s\n", errorString ? errorString : "Unknown");
    //
    //    assert(false); // Trigger an assertion failure for debugging
    //}

}

json Mesh::SaveComponent()
{
    json meshJSON;

    meshJSON["Name"] = name;
    meshJSON["Type"] = type;
    if (auto pGO = containerGO.lock())
    {
        meshJSON["ParentUID"] = pGO.get()->GetUID();
    }
    meshJSON["UID"] = UID;
    meshJSON["Active"] = active;
    meshJSON["DrawWireframe"] = drawWireframe;
    meshJSON["DrawAABB"] = drawAABB;
    meshJSON["DrawOBB"] = drawOBB;
    meshJSON["DrawChecker"] = drawChecker;
    meshJSON["DrawNormalsVerts"] = drawNormalsVerts;
    meshJSON["DrawNormalsFaces"] = drawNormalsFaces;
    meshJSON["Path"] = path;

    //MeshData && MeshBufferedData are already serialized in .mesh files

    return meshJSON;
}

void Mesh::LoadComponent(const json& meshJSON)
{
    // Load basic properties
    if (meshJSON.contains("UID"))
    {
        UID = meshJSON["UID"];
    }

    if (meshJSON.contains("Name"))
    {
        name = meshJSON["Name"];
    }

    // Load parent UID and set parent
    if (meshJSON.contains("ParentUID"))
    {
        uint32_t parentUID = meshJSON["ParentUID"];
        if (auto parentGameObject = SceneManager::GetInstance().FindGOByUID(parentUID))
        {
            containerGO = parentGameObject;
        }
    }

    // Load mesh-specific properties
    if (meshJSON.contains("Active"))
    {
        active = meshJSON["Active"];
    }

    if (meshJSON.contains("DrawWireframe"))
    {
        drawWireframe = meshJSON["DrawWireframe"];
    }

    if (meshJSON.contains("DrawAABB"))
    {
        drawAABB = meshJSON["DrawAABB"];
    }

    if (meshJSON.contains("DrawOBB"))
    {
        drawOBB = meshJSON["DrawOBB"];
    }

    if (meshJSON.contains("DrawChecker"))
    {
        drawChecker = meshJSON["DrawChecker"];
    }

    if (meshJSON.contains("DrawNormalsVerts"))
    {
        drawNormalsVerts = meshJSON["DrawNormalsVerts"];
    }

    if (meshJSON.contains("DrawNormalsFaces"))
    {
        drawNormalsFaces = meshJSON["DrawNormalsFaces"];
    }

    if (meshJSON.contains("Path"))
    {
        path = meshJSON["Path"];
    }

    // Implement additional logic to handle other mesh-specific properties as needed
    // ...

    // Optional: Reinitialize or update the mesh based on the loaded data
    // ...
}


static inline void glVec3(const vec3& v) { glVertex3dv(&v.x); }

void Mesh::DrawAABB() 
{
    glLineWidth(2);
    //glColor3f(1.0f, 0.0f, 1.0f);
    glBegin(GL_LINE_STRIP);
    
    glVec3(aabb.a());
    glVec3(aabb.b());
    glVec3(aabb.c());
    glVec3(aabb.d());
    glVec3(aabb.a());
    
    glVec3(aabb.e());
    glVec3(aabb.f());
    glVec3(aabb.g());
    glVec3(aabb.h());
    glVec3(aabb.e());
    glEnd();
    
    glBegin(GL_LINES);
    glVec3(aabb.h());
    glVec3(aabb.d());
    glVec3(aabb.f());
    glVec3(aabb.b());
    glVec3(aabb.g());
    glVec3(aabb.c());

    glLineWidth(1);
    //glColor3f(1.0f, 0.0f, 1.0f);
    glEnd();
}

void Mesh::DrawOBB() {
    glColor3f(1, 0, 1);
    glLineWidth(2);
    vec3f* obb_points = nullptr;
    //obb.GetCornerPoints(obb_points);
    //
    //glBegin(GL_LINES);
    //
    //glVertex3f(obb.CornerPoint(0).x, obb.CornerPoint(0).y, obb.CornerPoint(0).z);
    //glVertex3f(obb.CornerPoint(1).x, obb.CornerPoint(1).y, obb.CornerPoint(1).z);
    //
    //glVertex3f(obb.CornerPoint(0).x, obb.CornerPoint(0).y, obb.CornerPoint(0).z);
    //glVertex3f(obb.CornerPoint(4).x, obb.CornerPoint(4).y, obb.CornerPoint(4).z);
    //
    //glVertex3f(obb.CornerPoint(0).x, obb.CornerPoint(0).y, obb.CornerPoint(0).z);
    //glVertex3f(obb.CornerPoint(2).x, obb.CornerPoint(2).y, obb.CornerPoint(2).z);
    //
    //glVertex3f(obb.CornerPoint(2).x, obb.CornerPoint(2).y, obb.CornerPoint(2).z);
    //glVertex3f(obb.CornerPoint(3).x, obb.CornerPoint(3).y, obb.CornerPoint(3).z);
    //
    //glVertex3f(obb.CornerPoint(1).x, obb.CornerPoint(1).y, obb.CornerPoint(1).z);
    //glVertex3f(obb.CornerPoint(5).x, obb.CornerPoint(5).y, obb.CornerPoint(5).z);
    //
    //glVertex3f(obb.CornerPoint(1).x, obb.CornerPoint(1).y, obb.CornerPoint(1).z);
    //glVertex3f(obb.CornerPoint(3).x, obb.CornerPoint(3).y, obb.CornerPoint(3).z);
    //
    //glVertex3f(obb.CornerPoint(4).x, obb.CornerPoint(4).y, obb.CornerPoint(4).z);
    //glVertex3f(obb.CornerPoint(5).x, obb.CornerPoint(5).y, obb.CornerPoint(5).z);
    //
    //glVertex3f(obb.CornerPoint(4).x, obb.CornerPoint(4).y, obb.CornerPoint(4).z);
    //glVertex3f(obb.CornerPoint(6).x, obb.CornerPoint(6).y, obb.CornerPoint(6).z);
    //
    //glVertex3f(obb.CornerPoint(2).x, obb.CornerPoint(2).y, obb.CornerPoint(2).z);
    //glVertex3f(obb.CornerPoint(6).x, obb.CornerPoint(6).y, obb.CornerPoint(6).z);
    //
    //glVertex3f(obb.CornerPoint(5).x, obb.CornerPoint(5).y, obb.CornerPoint(5).z);
    //glVertex3f(obb.CornerPoint(7).x, obb.CornerPoint(7).y, obb.CornerPoint(7).z);
    //
    //glVertex3f(obb.CornerPoint(6).x, obb.CornerPoint(6).y, obb.CornerPoint(6).z);
    //glVertex3f(obb.CornerPoint(7).x, obb.CornerPoint(7).y, obb.CornerPoint(7).z);
    //
    //glVertex3f(obb.CornerPoint(3).x, obb.CornerPoint(3).y, obb.CornerPoint(3).z);
    //glVertex3f(obb.CornerPoint(7).x, obb.CornerPoint(7).y, obb.CornerPoint(7).z);
    //
    //glLineWidth(1);
    //glEnd();
}