#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

#include "blocktype.h"
#include "renderer.h"

class Chunk
{
public:
    Chunk() = default;
    Chunk(glm::uvec2 position);
    void Render(Renderer& renderer);
    BlockType GetBlock(glm::uvec3 pos);
private:
    glm::uvec2 position = glm::uvec2(0);
    BlockType blocks[16][256][16] { BlockType::AIR };
};
