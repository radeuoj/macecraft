#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

#include "Chunk.h"

namespace Macecraft
{
    class World
    {
    public:
        std::unordered_map<glm::ivec2, Chunk> chunks;

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
        std::queue<glm::ivec2> m_ChunkGenerationQueue;

    };

}

