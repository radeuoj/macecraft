#pragma once

#include <glm/glm.hpp>

#include "BlockType.h"
#include "Renderer.h"

namespace Macecraft
{
    class World;

    class Chunk
    {
    public:
        static constexpr int SIZE = 16;
        static constexpr int HEIGHT = 256;

        Chunk(World& world, glm::i16vec2 position);
        void render();
        BlockType getBlock(glm::uvec3 pos);
        glm::i16vec2 getPosition();
        Renderer m_Renderer;
    private:
        World& m_World;
        glm::i16vec2 m_Position = glm::i16vec2(0);
        BlockType m_Blocks[16][256][16] { BlockType::AIR };
    };

}
