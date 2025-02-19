#include "Texture.h"

#include <iostream>

namespace Macecraft
{

void Texture::InitFromBytes(unsigned char* bytes, int imageWidth, int imageHeight, int imageChannels, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
    type = texType;

    glGenTextures(1, &ID);
    glActiveTexture(slot);
    glBindTexture(texType, ID);

    // glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(texType, GL_TEXTURE_MAX_LOD, 4.0f);
    // glTexParameteri(texType, GL_TEXTURE_MAX_LEVEL, 2);

    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(texType, GL_TEXTURE_WRAP_R, GL_REPEAT);

    if (imageChannels == 4)
        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            imageWidth,
            imageHeight,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            bytes
        );
    else if (imageChannels == 3)
        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            imageWidth,
            imageHeight,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            bytes
        );
    else if (imageChannels == 1)
        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            imageWidth,
            imageHeight,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            bytes
        );
    else
        throw std::invalid_argument("Automatic Texture type recognition failed");

    glGenerateMipmap(texType);

    glBindTexture(texType, 0);
}

void Texture::Bind()
{
    glBindTexture(type, ID);
}

void Texture::Unbind()
{
    glBindTexture(type, 0);
}

Texture::~Texture()
{
    glDeleteTextures(1, &ID);
}
    
}



