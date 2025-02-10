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
        void addChunkIfDoesntExist(glm::ivec2 pos);
        bool areNeighboursGenerated(glm::ivec2 pos);
        void renderChunks(Shader& shader, TextureAtlas* atlas);

        BlockType getBlock(glm::ivec3 pos);

    private:
        /**
         * index of chunk in chunks vector
         */
        std::queue<int> m_ChunkGenerationQueue;

    };

}

