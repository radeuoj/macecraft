#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "BlockType.h"
#include "Renderer.h"

class Chunk
{
public:
    Chunk() = default;
    Chunk(glm::i16vec2 position);
    void render(Renderer& renderer);
    BlockType getBlock(glm::uvec3 pos);
private:
    glm::i16vec2 m_Position = glm::i16vec2(0);
    BlockType m_Blocks[16][256][16] { BlockType::AIR };
};
