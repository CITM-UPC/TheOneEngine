#include "AssetMesh.h"
#include "Defs.h"

#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>

#include <vector>
#include <array>
#include <string>

using namespace std;

AssetMesh::AssetMesh(uint ID, string path, Formats format, const void* vertex_data, uint numVerts, const uint* index_data, uint numIndexs)
    : Asset(AssetType::MESH, ID, path), format(format), numVerts(numVerts), numIndexs(numIndexs)
{
    extension = ".fbx";
    this->path = ASSETS_PATH + std::to_string(ID) + extension;

    glGenBuffers(1, &vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);

    switch (format)
    {
        case Formats::F_V3:
            glBufferData(GL_ARRAY_BUFFER, sizeof(V3) * numVerts, vertex_data, GL_STATIC_DRAW);
            break;
        case Formats::F_V3C4:
            glBufferData(GL_ARRAY_BUFFER, sizeof(V3C4) * numVerts, vertex_data, GL_STATIC_DRAW);
            break;
        case Formats::F_V3T2:
            glBufferData(GL_ARRAY_BUFFER, sizeof(V3T2) * numVerts, vertex_data, GL_STATIC_DRAW);
            break;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (index_data)
    {
        glGenBuffers(1, &indexs_buffer_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexs_buffer_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numIndexs, index_data, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    else {
        indexs_buffer_id = 0;
    }
}

AssetMesh::AssetMesh(AssetMesh&& b) noexcept : Asset(AssetType::MESH, ID, path),
    format(b.format),
    vertex_buffer_id(b.vertex_buffer_id),
    numVerts(b.numVerts),
    indexs_buffer_id(b.indexs_buffer_id),
    numIndexs(b.numIndexs),
    texture(b.texture)
{
    b.vertex_buffer_id = 0;
    b.indexs_buffer_id = 0;
}

AssetMesh::~AssetMesh()
{
    if (vertex_buffer_id) glDeleteBuffers(1, &vertex_buffer_id);
    if (indexs_buffer_id) glDeleteBuffers(1, &indexs_buffer_id);
}

std::vector<AssetMesh::Ptr> AssetMesh::loadFromFile(const std::string& path)
{
    vector<AssetMesh::Ptr> mesh_ptrs;

    auto scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);

    for (size_t m = 0; m < scene->mNumMeshes; ++m) {
        auto mesh = scene->mMeshes[m];
        auto faces = mesh->mFaces;
        vec3f* verts = (vec3f*)mesh->mVertices;
        vec3f* texCoords = (vec3f*)mesh->mTextureCoords[0];

        vector<V3T2> vertex_data;
        vector<unsigned int> index_data;

        for (size_t i = 0; i < mesh->mNumVertices; ++i) {
            V3T2 v = { verts[i], vec2f(texCoords[i].x, texCoords[i].y) };
            vertex_data.push_back(v);
        }

        for (size_t f = 0; f < mesh->mNumFaces; ++f) {
            index_data.push_back(faces[f].mIndices[0]);
            index_data.push_back(faces[f].mIndices[1]);
            index_data.push_back(faces[f].mIndices[2]);
        }

        auto material = scene->mMaterials[mesh->mMaterialIndex];
        aiString aiPath;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath);

        string folderPath = "Assets\\";
        string texPath = folderPath + aiScene::GetShortFilename(aiPath.C_Str());

        auto mesh_ptr = make_shared<AssetMesh>(0, path, Formats::F_V3T2, vertex_data.data(), vertex_data.size(), index_data.data(), index_data.size());
        mesh_ptr->texture = make_shared<Texture>(texPath);

        mesh_ptrs.push_back(mesh_ptr);
    }

    aiReleaseImport(scene);

    return mesh_ptrs;
}

void AssetMesh::draw()
{
    glColor4ub(255, 255, 255, 255);

    // WIREFRAME MODE, comment the line below for fill mode
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);
    glEnableClientState(GL_VERTEX_ARRAY);

    switch (format)
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
        if (texture.get()) texture->bind();
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, sizeof(V3T2), nullptr);
        glTexCoordPointer(2, GL_FLOAT, sizeof(V3T2), (void*)sizeof(V3));
        break;
    }

    if (indexs_buffer_id) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexs_buffer_id);
        glDrawElements(GL_TRIANGLES, numIndexs, GL_UNSIGNED_INT, nullptr);
    }
    else {
        glDrawArrays(GL_TRIANGLES, 0, numVerts);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
}