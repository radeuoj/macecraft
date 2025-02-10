#pragma once
#include "Texture.h"
#include "BlockData.h"
#include <unordered_map>

#include "BlockType.h"

namespace Macecraft
{

    class TextureAtlas
    {
    public:
        TextureAtlas(unsigned char *bytes,int imageWidth, int imageHeight, int channels, int spriteWidth, int spriteHeight);

        void bind();
        void unbind();

        uint8_t getTextureLocation(BlockType block, BlockDirection direction, uint8_t x, uint8_t y);
    private:
        int m_ImageWidth = 0, m_ImageHeight = 0;
        int m_SpriteWidth = 0, m_SpriteHeight = 0;
        Texture m_Texture;
        std::unordered_map<BlockType, BlockData> m_TextureData;

        inline uint8_t texcoords(uint8_t x, uint8_t y)
        {
            return x + 16 * y;
        }
    };

    // inline GLubyte texcoords(GLubyte x, GLubyte y)
    // {
    //     // return (x * 16) + y;
    //     return x + 2 * y;
    // }

} // Macecraft
