#pragma once

#include <unordered_map>
#include  <glad/glad.h>

#include "BlockData.h"
#include "BlockType.h"
#include "Texture.h"

namespace Macecraft {

class TextureArray: public Texture
{
public:
    static constexpr int NUMBER_OF_TEXTURES = 4;
    GLenum type = GL_TEXTURE_2D_ARRAY;
    
    TextureArray();
    
    void InitSpace();
    void UploadBytes(unsigned char* bytes, int offset);
    
    uint8_t GetTextureLocation(BlockType block, BlockDirection direction, uint8_t x, uint8_t y) const;

private:
    int m_SpriteWidth = 16, m_SpriteHeight = 16, m_ImageChannels = 4;
    
    std::unordered_map<BlockType, BlockData> m_TextureData;
};

}
