#pragma once

#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include "Shader.h"
#include <vector>

namespace Macecraft
{
    
class Texture
{
public:
    GLuint ID;
    GLenum type;

    Texture() = default;
    ~Texture();

    void InitFromBytes(unsigned char* bytes, int imageWidth, int imageHeight, int imageChannels, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

    void Bind();
    void Unbind();
};
    
}

