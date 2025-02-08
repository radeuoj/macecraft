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

        Chunk(World* world, glm::i16vec2 position);
        Chunk(const Chunk& other) = delete;
        Chunk(Chunk&& other) noexcept;
        void render();
        BlockType getBlock(glm::uvec3 pos);
        glm::i16vec2 getPosition();
        Renderer m_Renderer;
    private:
        World* m_World;
        glm::i16vec2 m_Position = glm::i16vec2(0);
        BlockType m_Blocks[SIZE][HEIGHT][SIZE] { BlockType::AIR };
        // std::array<std::array<std::array<BlockType, SIZE>, HEIGHT>, SIZE> m_Blocks;
    };

}
