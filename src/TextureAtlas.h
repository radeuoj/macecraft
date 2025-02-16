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
    TextureAtlas(unsigned char *image, int imageLength, int spriteWidth, int spriteHeight);
    TextureAtlas(const char* path, int spriteWidth, int spriteHeight);

    void Bind();
    void Unbind();

    uint8_t GetTextureLocation(BlockType block, BlockDirection direction, uint8_t x, uint8_t y);
private:
    int m_ImageWidth = 0, m_ImageHeight = 0, m_ImageChannels = 0;
    int m_SpriteWidth = 0, m_SpriteHeight = 0;
    Texture m_Texture;
    std::unordered_map<BlockType, BlockData> m_TextureData;

    void InitFromBytes(unsigned char* bytes);

    inline uint8_t texcoords(uint8_t x, uint8_t y)
    {
        return x + 16 * y;
    }
};

} // Macecraft
