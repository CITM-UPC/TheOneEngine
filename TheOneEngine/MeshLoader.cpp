#include "MeshLoader.h"
#include "Defs.h"

#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>

#include <vector>
#include <array>
#include <string>


MeshLoader::MeshLoader() {}

//MeshLoader::MeshLoader(MeshLoader&& b) noexcept :
//    Component(containerGO.lock(), ComponentType::Mesh),
//    format(b.format),
//    vertex_buffer_id(b.vertex_buffer_id),
//    numVerts(b.numVerts),
//    indexs_buffer_id(b.indexs_buffer_id),
//    numIndexs(b.numIndexs),
//    texture(b.texture)
//{
//    b.vertex_buffer_id = 0;
//    b.indexs_buffer_id = 0;
//}

MeshLoader::~MeshLoader()
{
    if (meshBuffData.vertex_buffer_id) glDeleteBuffers(1, &meshBuffData.vertex_buffer_id);
    if (meshBuffData.indexs_buffer_id) glDeleteBuffers(1, &meshBuffData.indexs_buffer_id);
}

void MeshLoader::BufferData(MeshData meshData)
{
    //extension = ".fbx";
    //this->path = ASSETS_PATH + std::to_string(ID) + extension;

    glGenBuffers(1, &meshBuffData.vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, meshBuffData.vertex_buffer_id);

    switch (meshData.format)
    {
    case Formats::F_V3:
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3) * meshBuffData.numVerts, meshData.vertex_data, GL_STATIC_DRAW);
        break;
    case Formats::F_V3C4:
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3C4) * meshBuffData.numVerts, meshData.vertex_data, GL_STATIC_DRAW);
        break;
    case Formats::F_V3T2:
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3T2) * meshBuffData.numVerts, meshData.vertex_data, GL_STATIC_DRAW);
        break;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (meshData.index_data)
    {
        glGenBuffers(1, &meshBuffData.indexs_buffer_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffData.indexs_buffer_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * meshBuffData.numIndexs, meshData.index_data, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    else {
        meshBuffData.indexs_buffer_id = 0;
    }
}

std::vector<MeshLoader::MeshBufferedData> MeshLoader::loadFromFile(std::shared_ptr<GameObject> containerGO, const std::string& path)
{
    std::vector<MeshBufferedData> mehsesData;

    auto scene = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_ForceGenNormals);

    for (size_t m = 0; m < scene->mNumMeshes; ++m)
    {
        auto mesh = scene->mMeshes[m];
        auto faces = mesh->mFaces;
        vec3f* verts = (vec3f*)mesh->mVertices;
        vec3f* texCoords = (vec3f*)mesh->mTextureCoords[0];

        std::vector<V3T2> vertex_data;
        std::vector<unsigned int> index_data;

        for (size_t i = 0; i < mesh->mNumVertices; ++i)
        {
            V3T2 v = { verts[i], vec2f(texCoords[i].x, texCoords[i].y) };
            vertex_data.push_back(v);
        }

        for (size_t f = 0; f < mesh->mNumFaces; ++f)
        {
            index_data.push_back(faces[f].mIndices[0]);
            index_data.push_back(faces[f].mIndices[1]);
            index_data.push_back(faces[f].mIndices[2]);
        }

        auto material = scene->mMaterials[mesh->mMaterialIndex];
        aiString aiPath;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath);

        std::string folderPath = "Assets\\";
        std::string texPath = folderPath + aiScene::GetShortFilename(aiPath.C_Str());

        meshData =
        {
            Formats::F_V3T2,
            vertex_data.data(),
            (uint)vertex_data.size(),
            index_data.data(),
            (uint)index_data.size(),
        };

        BufferData(meshData);
        meshBuffData.texturePath = texPath;

        mehsesData.push_back(meshBuffData);
    }

    aiReleaseImport(scene);

    return mehsesData;
}