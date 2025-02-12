#include "World.h"

namespace Macecraft
{

    void World::init()
    {
        // constexpr int S = 16;
        // for (int i = 0; i < S; i++)
        // {
        //     for (int j = 0; j < S; j++)
        //     {
        //         // chunks.push_back(Chunk(*this, {i, j}));
        //         chunks.emplace_back(this, glm::i16vec2(i, j));
        //     }
        // }
    }

    void World::renderChunks(Shader& shader, TextureAtlas* atlas, glm::vec3 playerPosition)
    {
        for (auto& [pos, chunk] : chunks)
        {
            chunk.flush(shader, atlas);
        }
    }

    void World::generateChunksIfNeeded(glm::vec3 playerPosition)
    {
        glm::ivec2 playerChunk = worldPosToChunkPos(playerPosition).first;
        m_ChunkGenerationCounter = 0;

        safeGenerateChunk(playerChunk);

        for (int dist = 1; dist <= CHUNK_RENDER_DISTANCE; dist++)
        {
            // 1 0
            safeGenerateChunk(playerChunk + glm::ivec2(dist, 0));

            for (int i = 1; i < dist; i++)
            {
                safeGenerateChunk(playerChunk + glm::ivec2(dist, i));
                safeGenerateChunk(playerChunk + glm::ivec2(dist, -i));
            }

            // -1 0
            safeGenerateChunk(playerChunk + glm::ivec2(-dist, 0));

            for (int i = 1; i < dist; i++)
            {
                safeGenerateChunk(playerChunk + glm::ivec2(-dist, i));
                safeGenerateChunk(playerChunk + glm::ivec2(-dist, -i));
            }

            // 0 1
            safeGenerateChunk(playerChunk + glm::ivec2(0, dist));

            for (int i = 1; i < dist; i++)
            {
                safeGenerateChunk(playerChunk + glm::ivec2(i, dist));
                safeGenerateChunk(playerChunk + glm::ivec2(-i, dist));
            }

            // 0 -1
            safeGenerateChunk(playerChunk + glm::ivec2(0, -dist));

            for (int i = 1; i < dist; i++)
            {
                safeGenerateChunk(playerChunk + glm::ivec2(i, -dist));
                safeGenerateChunk(playerChunk + glm::ivec2(-i, -dist));
            }

            safeGenerateChunk(playerChunk + glm::ivec2(dist, dist));
            safeGenerateChunk(playerChunk + glm::ivec2(-dist, dist));
            safeGenerateChunk(playerChunk + glm::ivec2(dist, -dist));
            safeGenerateChunk(playerChunk + glm::ivec2(-dist, -dist));
        }
    }

    void World::safeGenerateChunk(glm::ivec2 pos)
    {
        if (m_ChunkGenerationCounter < CHUNK_GENERATION_LIMIT_PER_FRAME && !addChunkIfDoesntExist(pos))
        {
            chunks.at(pos).generate();
            m_ChunkGenerationCounter++;
        }
    }

    void World::cleanupChunks(glm::vec3 playerPosition)
    {
        glm::ivec2 playerChunkPos = worldPosToChunkPos(playerPosition).first;
        std::erase_if(chunks, [&playerChunkPos](auto& kv) -> bool
        {
            return squareDistance(kv.first, playerChunkPos) > CHUNK_RENDER_DISTANCE + CHUNK_RENDER_DISTANCE_ERROR;
        });
    }


    float World::squareDistance(const glm::vec2 a, const glm::vec2 b)
    {
        return max(abs(a.x - b.x), abs(a.y - b.y));
    }

    int World::squareDistance(const glm::ivec2 a, const glm::ivec2 b)
    {
        return max(abs(a.x - b.x), abs(a.y - b.y));
    }

    glm::vec3 World::chunkPosToWorldPos(const glm::ivec2 chunkPos, const glm::vec3 pos)
    {
        return glm::vec3(chunkPos.x * Chunk::SIZE + pos.x, pos.y, chunkPos.y * Chunk::SIZE + pos.y);
    }

    std::pair<glm::ivec2, glm::vec3> World::worldPosToChunkPos(glm::vec3 pos)
    {
        glm::ivec2 chunkPos = glm::ivec2(0);
        glm::vec3 localPos = glm::vec3(0);

        chunkPos.x = int(pos.x) / Chunk::SIZE;
        chunkPos.y = int(pos.z) / Chunk::SIZE;

        if (pos.x < 0)
            chunkPos.x -= 1;

        if (pos.z < 0)
            chunkPos.y -= 1;

        localPos.x = pos.x - chunkPos.x * Chunk::SIZE;
        localPos.z = pos.z - chunkPos.y * Chunk::SIZE;
        localPos.y = pos.y;

        return std::make_pair(chunkPos, localPos);
    }

    void World::deleteChunkIfExists(glm::ivec2 pos)
    {
        chunks.erase(pos);
    }

    /**
     * @return if it already existed
     */
    bool World::addChunkIfDoesntExist(glm::ivec2 pos)
    {
        if (chunks.find(pos) == chunks.end())
        {
            chunks.insert(std::make_pair(pos, Chunk(this, pos)));
            m_ChunkGenerationQueue.push(pos);

            return false;
        }

        return true;
    }

    bool World::areNeighboursGenerated(glm::ivec2 pos)
    {
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                auto it = chunks.find(pos + glm::ivec2(i, j));

                if (it == chunks.end()) return false;
                if (!it->second.isGenerated()) return false;
            }
        }

        return true;
    }

    /**
     * is inefficient
     */
    BlockType World::getBlock(glm::ivec3 pos)
    {
        // Getting the chunk coords
        glm::ivec3 localPos = glm::ivec3(pos.x & (Chunk::SIZE - 1), pos.y, pos.z & (Chunk::SIZE - 1));
        glm::ivec2 chunkPos = glm::ivec2((pos.x - localPos.x) / Chunk::SIZE, (pos.z - localPos.z) / Chunk::SIZE);

        auto it = chunks.find(chunkPos);

        if (it == chunks.end())
            return BlockType::AIR;

        return it->second.getBlock(localPos);
    }


}
