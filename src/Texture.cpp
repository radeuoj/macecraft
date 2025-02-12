#include "Texture.h"

#include <iostream>

void Texture::initFromFile(const char *image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
    type = texType;

    int widthImg, heightImg, numColCh;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &numColCh, 0);

    std::cout << bytes << std::endl << std::endl << std::endl;

    glGenTextures(1, &ID);
    glActiveTexture(slot);
    glBindTexture(texType, ID);

    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);

    if (numColCh == 4)
        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            widthImg,
            heightImg,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            bytes
        );
    else if (numColCh == 3)
        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            widthImg,
            heightImg,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            bytes
        );
    else if (numColCh == 1)
        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            widthImg,
            heightImg,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            bytes
        );
    else
        throw std::invalid_argument("Automatic Texture type recognition failed");

    glGenerateMipmap(texType);

    stbi_image_free(bytes);
    glBindTexture(texType, 0);
}

void Texture::initFromBytes(unsigned char* bytes, int width, int height, int channels, GLenum texType, GLenum slot, GLenum format, GLenum pixelType)
{
    type = texType;

    glGenTextures(1, &ID);
    glActiveTexture(slot);
    glBindTexture(texType, ID);

    glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // glTexImage2D(texType, 0, GL_RGBA, widthImg, heightImg, 0, format, pixelType, bytes);

    if (channels == 4)
        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            bytes
        );
    else if (channels == 3)
        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
            0,
            GL_RGB,
            GL_UNSIGNED_BYTE,
            bytes
        );
    else if (channels == 1)
        glTexImage2D
        (
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            width,
            height,
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

void Texture::texUnit(Shader& shader, const char *uniform, GLuint unit)
{
    GLuint tex0Uni = glGetUniformLocation(shader.ID, uniform);
    shader.activate();
    glUniform1i(tex0Uni, unit);
}

void Texture::bind()
{
    glBindTexture(type, ID);
}

void Texture::unbind()
{
    glBindTexture(type, 0);
}

void Texture::Delete()
{
    glDeleteTextures(1, &ID);
}


