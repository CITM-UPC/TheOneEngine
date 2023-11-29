#include "Mesh.h"

Mesh::Mesh(std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Mesh) 
{
    drawNormalsFaces = false;
    drawNormalsVerts = false;
}

Mesh::~Mesh() {}

void Mesh::DrawComponent()
{
    glColor4ub(255, 255, 255, 255);

    // WIREFRAME MODE, comment the line below for fill mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    for (const auto mesh : meshes)
    {
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
            if (mesh.texture.get()) mesh.texture->bind();
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glVertexPointer(3, GL_FLOAT, sizeof(V3T2), nullptr);
            glTexCoordPointer(2, GL_FLOAT, sizeof(V3T2), (void*)sizeof(V3));
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

            for (int i = 0; i < _numVerts; i++) {
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

            for (int i = 0; i < _numFaces; i++) {
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
}