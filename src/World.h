#pragma once

#include <vector>
#include <queue>

#include "Chunk.h"

namespace Macecraft
{

    class World
    {
    public:
        std::vector<Chunk> chunks;

        World() = default;
        void init();
        void generateChunksIfNeeded();
        void renderChunks(Shader& shader, TextureAtlas* atlas);

        BlockType getBlock(glm::ivec3 pos);

    private:
        std::queue<int> m_ChunkGenerationQueue;

    };

}

