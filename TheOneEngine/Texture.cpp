#include "Texture.h"
#include <GL/glew.h>
#include <IL/il.h>

#include <algorithm>

#define CHECKERS_HEIGHT 32
#define CHECKERS_WIDTH 32


using namespace std;

Texture::Texture(const std::string& newPath, std::shared_ptr<GameObject> containerGO) : Component(containerGO, ComponentType::Texture)
{
    //load image data using devil
    auto img = ilGenImage();
    ilBindImage(img);
    ilLoadImage((const wchar_t*)newPath.c_str());
    path = newPath;
    auto width = ilGetInteger(IL_IMAGE_WIDTH);
    this->width = static_cast<uint>(width);
    auto height = ilGetInteger(IL_IMAGE_HEIGHT);
    this->height = static_cast<uint>(height);
    ILint channels = ilGetInteger(IL_IMAGE_CHANNELS);
    auto data = ilGetData();

    // Ajusta la alineación de pixel si es necesario (importante para imágenes RGB)
    if (channels == 3) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }

    
    GLenum format;
    switch (channels)
    {
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        format = GL_RGB;
        break;
    }

    //load image as a texture in VRAM
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    //now we can delete image from RAM
    ilDeleteImage(img);
}

Texture::Texture(std::shared_ptr<GameObject> containerGO, Texture* ref) : Component(containerGO, ComponentType::Texture)
{
    //load image data using devil
    auto img = ilGenImage();
    ilBindImage(img);
    ilLoadImage((const wchar_t*)ref->path.c_str());

    width = ref->width;
    height = ref->height;

    auto channels = ilGetInteger(IL_IMAGE_CHANNELS);
    auto data = ilGetData();

    //load image as a texture in VRAM
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);

    //now we can delete image from RAM
    ilDeleteImage(img);
}

//Texture::Texture(Texture&& tex) noexcept : _id(tex._id) {
//    tex._id = 0;
//}

Texture::~Texture() {
    if (_id) glDeleteTextures(1, &_id);
}

json Texture::SaveComponent()
{
    json textureJSON;

    /*textureJSON["Name"] = name;
    textureJSON["Type"] = type;
    if (auto pGO = containerGO.lock())
    {
        textureJSON["ParentUID"] = pGO.get()->GetUID();
    }
    textureJSON["UID"] = UID;
    textureJSON["Active"] = active;
    textureJSON["Path"] = path;
    textureJSON["Width"] = width;
    textureJSON["Height"] = height;*/



    return textureJSON;
}

void Texture::LoadComponent(const json& meshJSON)
{

}

void Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, _id);
}

void NoTexture()
{
    GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];

    for (int i = 0; i < CHECKERS_HEIGHT; i++) {
        for (int j = 0; j < CHECKERS_WIDTH; j++)
        {
            int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
            checkerImage[i][j][0] = (GLubyte)c;
            checkerImage[i][j][1] = (GLubyte)c;
            checkerImage[i][j][2] = (GLubyte)c;
            checkerImage[i][j][3] = (GLubyte)255;
        }
    }
}