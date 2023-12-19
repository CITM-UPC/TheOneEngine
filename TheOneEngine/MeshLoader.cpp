#include "MeshLoader.h"
#include "Texture.h"

#include <assimp/postprocess.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>

#include <GL/glew.h>

#include <span>
#include <vector>
#include <array>
#include <filesystem>
#include <fstream>


namespace fs = std::filesystem;
using namespace std;

struct aiMeshExt : aiMesh
{
    auto verts() const { return span((vec3f*)mVertices, mNumVertices); }
    auto texCoords() const { return span((vec3f*)mTextureCoords[0], mNumVertices); }
    auto faces() const { return span(mFaces, mNumFaces); }
};

struct aiSceneExt : aiScene
{
    auto materials() const { return span(mMaterials, mNumMaterials); }
    auto meshes() const { return span((aiMeshExt**)mMeshes, mNumMeshes); }
};



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
    meshBuffData.numVerts = meshData.vertex_data.size();
    meshBuffData.numIndexs = meshData.index_data.size();
    glGenBuffers(1, &meshBuffData.vertex_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, meshBuffData.vertex_buffer_id);

    switch (meshData.format)
    {
    case Formats::F_V3:
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3) * meshData.vertex_data.size(), meshData.vertex_data.data(), GL_STATIC_DRAW);
        meshBuffData.format = Formats::F_V3;
        break;
    case Formats::F_V3C4:
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3C4) * meshData.vertex_data.size(), meshData.vertex_data.data(), GL_STATIC_DRAW);
        meshBuffData.format = Formats::F_V3C4;
        break;
    case Formats::F_V3T2:
        glBufferData(GL_ARRAY_BUFFER, sizeof(V3T2) * meshData.vertex_data.size(), meshData.vertex_data.data(), GL_STATIC_DRAW);
        meshBuffData.format = Formats::F_V3T2;
        break;
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (meshData.index_data.data())
    {
        glGenBuffers(1, &meshBuffData.indexs_buffer_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBuffData.indexs_buffer_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * meshData.index_data.size(), meshData.index_data.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    else {
        meshBuffData.indexs_buffer_id = 0;
    }
}

std::vector<MeshBufferedData> MeshLoader::LoadMesh(const std::string& path)
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

        if (texCoords != nullptr)
        {
            for (size_t i = 0; i < mesh->mNumVertices; ++i)
            {
                V3T2 v = { verts[i], vec2f(texCoords[i].x, texCoords[i].y) };
                vertex_data.push_back(v);
            }
        }
        else
        {
            for (size_t i = 0; i < mesh->mNumVertices; ++i)
            {
                V3T2 v = { verts[i], vec2f(0, 0) };
                vertex_data.push_back(v);
            }
        }

        for (size_t f = 0; f < mesh->mNumFaces; ++f)
        {
            index_data.push_back(faces[f].mIndices[0]);
            index_data.push_back(faces[f].mIndices[1]);
            index_data.push_back(faces[f].mIndices[2]);

            /*vec3f faceNormal = glm::cross(&index_data[1] - &index_data[0], &index_data[2] - &index_data[0]);
            faceNormal = glm::normalize(faceNormal);
            mesh_sptr->meshFaceNorms.push_back(faceNormal);

            vec3f faceCenter = (index_data[0] + index_data[1] + index_data[2]) / 3.0f;
            mesh_sptr->meshFaceCenters.push_back(faceCenter);*/
        }

        meshData =
        {
            Formats::F_V3T2,
            vertex_data,
            index_data
        };

        BufferData(meshData);
        meshBuffData.meshName = mesh->mName.C_Str();
        meshBuffData.materialIndex = mesh->mMaterialIndex;

        mehsesData.push_back(meshBuffData);
    }

    aiReleaseImport(scene);

    return mehsesData;
}

std::vector<std::shared_ptr<Texture>> MeshLoader::LoadTexture(const std::string& path, std::shared_ptr<GameObject> containerGO)
{
    const auto scene_ptr = aiImportFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
    const aiSceneExt& scene = *(aiSceneExt*)scene_ptr;

    std::vector<std::shared_ptr<Texture>> texture_ptrs;

    for (auto& material : scene.materials())
    {
        aiString aiPath;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &aiPath);
        fs::path texPath = fs::path(path).parent_path() / fs::path(aiPath.C_Str()).filename();
        auto texture_ptr = make_shared<Texture>(texPath.string());
        texture_ptrs.push_back(texture_ptr);
    }

    aiReleaseImport(scene_ptr);

    return texture_ptrs;
}

void MeshLoader::serializeMeshBufferedData(const MeshBufferedData& data, const std::string& filename)
{
    std::ofstream outFile(filename, std::ios::binary);
    // Serialize string members
    size_t meshNameLength = data.meshName.size();
    outFile.write(reinterpret_cast<const char*>(&meshNameLength), sizeof(size_t));
    outFile.write(data.meshName.c_str(), meshNameLength);

    // Serialize enum member
    outFile.write(reinterpret_cast<const char*>(&data.format), sizeof(Formats));

    // Serialize other members
    outFile.write(reinterpret_cast<const char*>(&data.vertex_buffer_id), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&data.numVerts), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&data.indexs_buffer_id), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&data.numIndexs), sizeof(uint));
    outFile.write(reinterpret_cast<const char*>(&data.numFaces), sizeof(uint));

    size_t texturePathLength = data.texturePath.size();
    outFile.write(reinterpret_cast<const char*>(&texturePathLength), sizeof(size_t));
    outFile.write(data.texturePath.c_str(), texturePathLength);

    outFile.write(reinterpret_cast<const char*>(data.texture.get()), sizeof(Texture)); // Assuming Texture has a serialize function

    outFile.write(reinterpret_cast<const char*>(&data.materialIndex), sizeof(uint));

    // Close the file
    outFile.close();

}