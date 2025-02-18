#include "TextureAtlas.h"

namespace Macecraft
{
TextureAtlas::TextureAtlas(unsigned char *image, int imageLength, int spriteWidth, int spriteHeight):
m_SpriteWidth(spriteWidth),
m_SpriteHeight(spriteHeight)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bytes = stbi_load_from_memory(image, imageLength, &m_ImageWidth, &m_ImageHeight, &m_ImageChannels, 0);

    InitFromBytes(bytes);

    stbi_image_free(bytes);
}

TextureAtlas::TextureAtlas(const char* path, int spriteWidth, int spriteHeight):
m_SpriteWidth(spriteWidth),
m_SpriteHeight(spriteHeight)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned char* bytes = stbi_load(path, &m_ImageWidth, &m_ImageHeight, &m_ImageChannels, 0);

    InitFromBytes(bytes);

    stbi_image_free(bytes);
}

void TextureAtlas::InitFromBytes(unsigned char* bytes)
{
    m_Texture.InitFromBytes(bytes, m_ImageWidth, m_ImageHeight, m_ImageChannels, GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA4, GL_UNSIGNED_BYTE);

    using enum BlockType;

    m_TextureData[DIRT] = BlockData(texcoords(0, 0));
    m_TextureData[GRASS] = BlockData(texcoords(1, 0), texcoords(1, 0), texcoords(1, 0), texcoords(1, 0), texcoords(2, 0), texcoords(0, 0));
}


uint8_t TextureAtlas::GetTextureLocation(BlockType block, BlockDirection direction, uint8_t x, uint8_t y) const
{
    return m_TextureData.at(block)[direction] + x + y * 16;
}

void TextureAtlas::Bind()
{
    m_Texture.Bind();
}

void TextureAtlas::Unbind()
{
    m_Texture.Unbind();
}



} // Macecraft