#pragma once

#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include "Shader.h"
#include <vector>

class Texture
{
public:
    GLuint ID;
    GLenum type;

    Texture() = default;
    void init(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);
    void initFromBytes(unsigned char* bytes, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);

    void texUnit(Shader& shader, const char* uniform, GLuint unit);
    void bind();
    void unbind();
    void Delete();
};