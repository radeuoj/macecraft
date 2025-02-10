#pragma once

#include <glm/glm.hpp>

#include "BlockType.h"
#include "Renderer.h"
#include "TextureAtlas.h"

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

        bool isGenerated();
        void generate();
        void renderWhenPossible();
        void flush(Shader& shader, TextureAtlas* atlas);
        BlockType getBlock(glm::ivec3 pos);
        glm::i16vec2 getPosition();

        Renderer m_Renderer;
    private:
        World* m_World;
        glm::i16vec2 m_Position = glm::i16vec2(0);
        BlockType m_Blocks[SIZE][HEIGHT][SIZE] { BlockType::AIR };
        bool m_ShouldRender = true;
        bool m_IsGenerated = false;

        void render(TextureAtlas* atlas);
    };

}
