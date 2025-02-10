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

    void World::renderChunks(Shader& shader, TextureAtlas* atlas)
    {
        for (Chunk& chunk : chunks)
        {
            chunk.flush(shader, atlas);
        }
    }

    void World::generateChunksIfNeeded()
    {
        constexpr int S = 16;
        for (int i = 0; i < S; i++)
        {
            for (int j = 0; j < S; j++)
            {
                auto it = std::ranges::find_if(chunks.begin(), chunks.end(), [&i, &j](Chunk& chunk) -> bool
                {
                    return chunk.getPosition().x == i && chunk.getPosition().y == j;
                });

                // If its not rendered yet
                if (it == chunks.end())
                {
                    chunks.emplace_back(this, glm::i16vec2(i, j));
                    m_ChunkGenerationQueue.push(chunks.size() - 1);
                }
            }
        }

        if (!m_ChunkGenerationQueue.empty())
        {
            chunks[m_ChunkGenerationQueue.front()].generate();
            m_ChunkGenerationQueue.pop();
        }
    }

    /**
     * is inefficient
     */
    BlockType World::getBlock(glm::ivec3 pos)
    {
        // Getting the chunk coords
        glm::ivec3 localPos = glm::ivec3(pos.x & (Chunk::SIZE - 1), pos.y, pos.z & (Chunk::SIZE - 1));
        glm::ivec2 chunkPos = glm::ivec2((pos.x - localPos.x) / Chunk::SIZE, (pos.z - localPos.z) / Chunk::SIZE);

        auto it = std::ranges::find_if(chunks.begin(), chunks.end(), [&chunkPos](Chunk& chunk) -> bool
        {
            return chunk.getPosition().x == chunkPos.x && chunk.getPosition().y == chunkPos.y;
        });

        if (it == chunks.end())
            return BlockType::AIR;

        return it->getBlock(localPos);
    }


}
