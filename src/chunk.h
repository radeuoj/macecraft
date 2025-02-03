#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "blocktype.h"
#include "renderer.h"

class Chunk
{
public:
    Chunk() = default;
    Chunk(glm::i16vec2 position);
    void Render(Renderer& renderer);
    BlockType GetBlock(glm::uvec3 pos);
private:
    glm::i16vec2 position = glm::i16vec2(0);
    BlockType blocks[16][256][16] { BlockType::AIR };
};
