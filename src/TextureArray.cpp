#include "TextureArray.h"

#include <iostream>

namespace Macecraft {

TextureArray::TextureArray()
{
    InitSpace();
    stbi_set_flip_vertically_on_load(true);

    int _;
    
    unsigned char* black = stbi_load("res/textures/black.png", &_, &_, &_, 0);
    UploadBytes(black, 0);
    stbi_image_free(black);
    
    unsigned char* dirt = stbi_load("res/textures/dirt.png", &_, &_, &_, 0);
    UploadBytes(dirt, 1);
    stbi_image_free(dirt);
    
    unsigned char* grass_sides = stbi_load("res/textures/grass_sides.png", &_, &_, &_, 0);
    UploadBytes(grass_sides, 2);
    stbi_image_free(grass_sides);
    
    unsigned char* grass_top = stbi_load("res/textures/grass_top.png", &_, &_, &_, 0);
    UploadBytes(grass_top, 3);
    stbi_image_free(grass_top);

    using enum BlockType;

    m_TextureData[DIRT] = BlockData(1);
    
    m_TextureData[GRASS] = BlockData(2, 2, 2, 2, 3, 1);

    glGenerateMipmap(type);
}


void TextureArray::InitSpace()
{
    type = GL_TEXTURE_2D_ARRAY;
    constexpr GLenum slot = GL_TEXTURE0;

    glGenTextures(1, &ID);
    glActiveTexture(slot);
    glBindTexture(type, ID);

    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    // glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // glTexParameteri(type, GL_TEXTURE_MAX_LOD, 4.0f);
    // glTexParameteri(type, GL_TEXTURE_MAX_LEVEL, 2);

    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(type, GL_TEXTURE_WRAP_R, GL_REPEAT);

    glTexStorage3D(type, 3, GL_RGBA8, m_SpriteWidth, m_SpriteHeight, NUMBER_OF_TEXTURES);
    
    // glTexSubImage3D(type, 0, 0, 0, 0, m_SpriteWidth, m_SpriteHeight, NUMBER_OF_TEXTURES, format, GL_UNSIGNED_BYTE, bytes);

    // glTexImage3D(type, 0, GL_RGBA, spriteWidth, spriteHeight, length, 0, format, GL_UNSIGNED_BYTE, bytes);

    // glGenerateMipmap(type);

    // glBindTexture(type, 0);
}

void TextureArray::UploadBytes(unsigned char* bytes, int offset)
{
    GLenum format = GL_RGBA;
    if (m_ImageChannels == 4)
        format = GL_RGBA;
    else if (m_ImageChannels == 3)
        format = GL_RGB;
    else if (m_ImageChannels == 1)
        format = GL_RED;
    else
        throw std::invalid_argument("Automatic Texture type recognition failed");
    
    glTexSubImage3D(type, 0, 0, 0, offset, m_SpriteWidth, m_SpriteHeight, 1, format, GL_UNSIGNED_BYTE, bytes);
}

uint8_t TextureArray::GetTextureLocation(BlockType block, BlockDirection direction, uint8_t x, uint8_t y) const
{
    // return 2 * y + x;
    return (m_TextureData.at(block)[direction] << 2) + 2 * y + x;
}


}
