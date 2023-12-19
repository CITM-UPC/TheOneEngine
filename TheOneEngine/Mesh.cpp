#include <span>

#include "Mesh.h"
#include "../TheOneEditor/Log.h"


Mesh::Mesh(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Mesh) {
    drawNormalsFaces = false;
    drawNormalsVerts = false;
    drawAABB = true;
    drawChecker = false;
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

        break;
    }

    if (mesh.indexs_buffer_id) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexs_buffer_id);
        glDrawElements(GL_TRIANGLES, mesh.numIndexs, GL_UNSIGNED_INT, nullptr);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, mesh.numVerts);
    }
    
    if (drawNormalsVerts && !mesh.meshVerts.empty() && !mesh.meshNorms.empty()) {
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
    LOG(LogType::LOG_INFO, " %d", mesh.meshFaceCenters);
    if (drawNormalsFaces && !mesh.meshFaceCenters.empty() && !mesh.meshFaceNorms.empty()) {
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

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
}

void Mesh::DrawVertexNormals() 
{

}

void Mesh::DrawFaceNormals() 
{

}

void Mesh::DrawAABB() 
{

}

void Mesh::DrawOBB() {

}