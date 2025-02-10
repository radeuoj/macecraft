#include "TextureAtlas.h"

namespace Macecraft
{
    TextureAtlas::TextureAtlas(unsigned char *bytes,int imageWidth, int imageHeight, int channels, int spriteWidth, int spriteHeight):
    m_ImageWidth(imageWidth),
    m_ImageHeight(imageHeight),
    m_SpriteWidth(spriteWidth),
    m_SpriteHeight(spriteHeight),
    m_Texture()
    {
        m_Texture.initFromBytes(bytes, imageWidth, imageHeight, channels, GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA4, GL_UNSIGNED_BYTE);

        using enum BlockType;

        m_TextureData[DIRT] = BlockData(texcoords(0, 0));
    }

    uint8_t TextureAtlas::getTextureLocation(BlockType block, BlockDirection direction, bool x, bool y)
    {
        return m_TextureData[block][direction] + x + (y * m_SpriteHeight);
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