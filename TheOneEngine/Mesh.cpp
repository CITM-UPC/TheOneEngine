#include "Mesh.h"
#include "../TheOneEditor/Log.h"
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

    GenerateAABB();
}

Mesh::~Mesh()
{
    /*if (_vertex_buffer_id) glDeleteBuffers(1, &_vertex_buffer_id);
    if (_indexs_buffer_id) glDeleteBuffers(1, &_indexs_buffer_id);*/
}

void Mesh::DrawComponent()
{
    glColor4ub(255, 255, 255, 255);

    // uncomment the line below for WIREFRAME MODE
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer_id);
    glEnableClientState(GL_VERTEX_ARRAY);

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
    
    if (drawNormalsVerts && !mesh.meshVerts.empty() && !mesh.meshNorms.empty())
        DrawVertexNormals();

    if (drawNormalsFaces && !mesh.meshFaceCenters.empty() && !mesh.meshFaceNorms.empty())
        DrawFaceNormals();

    if (drawAABB) DrawAABB();

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);

    GLenum error = glGetError();

    //if (error != GL_NO_ERROR) {
    //    // Print the raw error code
    //    fprintf(stderr, "OpenGL error code: %d\n", error);

    //    // Print the corresponding error string
    //    const char* errorString = reinterpret_cast<const char*>(gluErrorString(error));
    //    fprintf(stderr, "OpenGL error: %s\n", errorString ? errorString : "Unknown");

    //    assert(false); // Trigger an assertion failure for debugging
    //}
}

void Mesh::DrawVertexNormals() 
{
    glLineWidth(normalLineWidth);
    glBegin(GL_LINES);
    glColor3f(1.0f, 1.0f, 0.0f);

    for (int i = 0; i < mesh.meshVerts.size(); i++) {
        glVertex3f(mesh.meshVerts[i].x, mesh.meshVerts[i].y, mesh.meshVerts[i].z);
        glVertex3f(mesh.meshVerts[i].x + mesh.meshNorms[i].x * normalLineLength,
            mesh.meshVerts[i].y + mesh.meshNorms[i].y * normalLineLength,
            mesh.meshVerts[i].z + mesh.meshNorms[i].z * normalLineLength);
    }

    glColor3f(1.0f, 1.0f, 0.0f);
    glEnd();
}

void Mesh::DrawFaceNormals() 
{
    glLineWidth(normalLineWidth);
    glBegin(GL_LINES);
    glColor3f(1.0f, 0.0f, 1.0f);

    for (int i = 0; i < mesh.meshFaceCenters.size(); i++) {
        glm::vec3 endPoint = mesh.meshFaceCenters[i] + normalLineLength * mesh.meshFaceNorms[i];
        glVertex3f(mesh.meshFaceCenters[i].x, mesh.meshFaceCenters[i].y, mesh.meshFaceCenters[i].z);
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


static inline void glVec3(const vec3& v) { glVertex3dv(&v.x); }

void Mesh::DrawAABB() 
{
    glLineWidth(2);
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
    glEnd();
}

void Mesh::DrawOBB() {

}