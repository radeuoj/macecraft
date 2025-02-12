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
        static constexpr int CHUNK_GENERATION_LIMIT_PER_FRAME = 2;
        static constexpr int CHUNK_RENDER_DISTANCE = 20;

        std::unordered_map<glm::ivec2, Chunk> chunks;

        World() = default;
        void init();
        void generateChunksIfNeeded(glm::vec3 playerPosition);
        bool addChunkIfDoesntExist(glm::ivec2 pos);
        bool areNeighboursGenerated(glm::ivec2 pos);
        void renderChunks(Shader& shader, TextureAtlas* atlas);
        static float squareDistance(glm::vec2 a, glm::vec2 b);
        static glm::vec3 chunkPosToWorldPos(glm::ivec2 chunkPos, glm::vec3 pos);
        static std::pair<glm::ivec2, glm::vec3> worldPosToChunkPos(glm::vec3 pos);
        void deleteChunkIfExists(glm::ivec2 pos);
        void safeGenerateChunk(glm::ivec2 pos);

        BlockType getBlock(glm::ivec3 pos);

    private:
        /**
         * index of chunk in chunks vector
         */
        std::queue<glm::ivec2> m_ChunkGenerationQueue;
        int m_ChunkGenerationCounter = 0;

    };

}

