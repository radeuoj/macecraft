#include "TextureAtlas.h"

namespace Macecraft
{
    TextureAtlas::TextureAtlas(unsigned char *image, int imageLength, int spriteWidth, int spriteHeight):
    m_SpriteWidth(spriteWidth),
    m_SpriteHeight(spriteHeight),
    m_Texture()
    {
        m_Texture.initFromImage(image, imageLength, GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA4, GL_UNSIGNED_BYTE);

        // TODO: magic numbers
        m_ImageWidth = 256;
        m_ImageHeight = 256;

        using enum BlockType;

        m_TextureData[DIRT] = BlockData(texcoords(0, 0));
        m_TextureData[GRASS] = BlockData(texcoords(1, 0), texcoords(1, 0), texcoords(1, 0), texcoords(1, 0), texcoords(2, 0), texcoords(0, 0));
    }

    uint8_t TextureAtlas::getTextureLocation(BlockType block, BlockDirection direction, uint8_t x, uint8_t y)
    {
        return m_TextureData[block][direction] + x + y * 16;
    }

    void TextureAtlas::bind()
    {
        m_Texture.bind();
    }

    void TextureAtlas::unbind()
    {
        m_Texture.unbind();
    }



} // Macecraft