#include "Texture.h"
#include <GL/glew.h>
#include <IL/il.h>

#include <algorithm>

using namespace std;

Texture::Texture(const std::string& path)
{
    //load image data using devil
    auto img = ilGenImage();
    ilBindImage(img);
    ilLoadImage((wchar_t*)path.c_str());
    auto width = ilGetInteger(IL_IMAGE_WIDTH);
    auto height = ilGetInteger(IL_IMAGE_HEIGHT);
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

Texture::Texture(Texture&& tex) noexcept : _id(tex._id) {
    tex._id = 0;
}

Texture::~Texture() {
    if (_id) glDeleteTextures(1, &_id);
}

void Texture::bind() const {
    glBindTexture(GL_TEXTURE_2D, _id);
}