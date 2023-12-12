#include "Mesh.h"

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
    
    
    // JULS: must change how Meshes work, as they should be also Graphic
    // JULS: Commmented all the code relating to the AABB

    switch (mesh.format)
    {
    case Formats::F_V3:
        glVertexPointer(3, GL_FLOAT, 0, nullptr);
        
        /*glBufferData(GL_ARRAY_BUFFER, sizeof(V3) * numVerts, vertex_data, GL_STATIC_DRAW);
        for (const auto& v : span((V3*)vertex_data, numVerts)) {
            aabb.min = glm::min(aabb.min, vec3(v.v));
            aabb.max = glm::max(aabb.max, vec3(v.v));
        }*/

        break;

    case Formats::F_V3C4:
        glEnableClientState(GL_COLOR_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(V3C4), nullptr);
        glColorPointer(4, GL_FLOAT, sizeof(V3C4), (void*)sizeof(V3));

        /* glBufferData(GL_ARRAY_BUFFER, sizeof(V3C4) * numVerts, vertex_data, GL_STATIC_DRAW);
        for (const auto& v : span((V3C4*)vertex_data, numVerts)) {
            aabb.min = glm::min(aabb.min, vec3(v.v));
            aabb.max = glm::max(aabb.max, vec3(v.v));
        }*/

        break;

    case Formats::F_V3T2:
        glEnable(GL_TEXTURE_2D);
        if (mesh.texture.get() && !drawChecker) mesh.texture->bind();
        //else mesh.checkboard.get()->bind();

        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(V3T2), nullptr);
        glTexCoordPointer(2, GL_FLOAT, sizeof(V3T2), (void*)sizeof(V3));

        /*glBufferData(GL_ARRAY_BUFFER, sizeof(V3T2) * numVerts, vertex_data, GL_STATIC_DRAW);
        for (const auto& v : span((V3T2*)vertex_data, numVerts)) {
            aabb.min = glm::min(aabb.min, vec3(v.v));
            aabb.max = glm::max(aabb.max, vec3(v.v));
        }*/

        break;
    }

    if (mesh.indexs_buffer_id) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.indexs_buffer_id);
        glDrawElements(GL_TRIANGLES, mesh.numIndexs, GL_UNSIGNED_INT, nullptr);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, mesh.numVerts);
    }

    if (drawNormalsVerts && !meshVerts.empty() && !meshNorms.empty()) {
        glLineWidth(normalLineWidth);
        glBegin(GL_LINES);
        glColor3f(1.0f, 1.0f, 0.0f);

        for (int i = 0; i < mesh.numVerts; i++) {
            glVertex3f(meshVerts[i].x, meshVerts[i].y, meshVerts[i].z);
            glVertex3f(meshVerts[i].x + meshNorms[i].x * normalLineLength,
                meshVerts[i].y + meshNorms[i].y * normalLineLength,
                meshVerts[i].z + meshNorms[i].z * normalLineLength);
        }

        glColor3f(1.0f, 1.0f, 0.0f);
        glEnd();
    }

    if (drawNormalsFaces && !meshFaceCenters.empty() && !meshFaceNorms.empty()) {
        glLineWidth(normalLineWidth);
        glBegin(GL_LINES);
        glColor3f(1.0f, 0.0f, 1.0f);

        for (int i = 0; i < mesh.numFaces; i++) {
            glm::vec3 endPoint = meshFaceCenters[i] + normalLineLength * meshFaceNorms[i];
            glVertex3f(meshFaceCenters[i].x, meshFaceCenters[i].y, meshFaceCenters[i].z);
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