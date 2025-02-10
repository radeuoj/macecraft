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
        // constexpr int S = 16;
        // for (int i = 0; i < S; i++)
        // {
        //     for (int j = 0; j < S; j++)
        //     {
        //         auto it = std::ranges::find_if(chunks.begin(), chunks.end(), [&i, &j](Chunk& chunk) -> bool
        //         {
        //             return chunk.getPosition().x == i && chunk.getPosition().y == j;
        //         });
        //
        //         // If its not rendered yet
        //         if (it == chunks.end())
        //         {
        //             chunks.emplace_back(this, glm::i16vec2(i, j));
        //             m_ChunkGenerationQueue.push(chunks.size() - 1);
        //         }
        //     }
        // }
        //
        // if (!m_ChunkGenerationQueue.empty())
        // {
        //     chunks[m_ChunkGenerationQueue.front()].generate();
        //     m_ChunkGenerationQueue.pop();
        // }

        std::queue<glm::ivec2> bfs;
        bfs.emplace(0, 0);

        addChunkIfDoesntExist(glm::ivec2(0, 0));

        while (!bfs.empty() && chunks.size() < 128)
        {
            auto pos = bfs.front();
            bfs.pop();

            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    addChunkIfDoesntExist(pos + glm::ivec2(i, j));
                    bfs.push(pos + glm::ivec2(i, j));
                }
            }
        }

        if (!m_ChunkGenerationQueue.empty())
        {
            chunks[m_ChunkGenerationQueue.front()].generate();
            m_ChunkGenerationQueue.pop();
        }
    }

    void World::addChunkIfDoesntExist(glm::ivec2 pos)
    {
        auto it = std::ranges::find_if(chunks.begin(), chunks.end(), [&pos](Chunk& chunk) -> bool
        {
            return glm::ivec2(chunk.getPosition()) == pos;
        });

        if (it == chunks.end())
        {
            chunks.emplace_back(this, pos);
            m_ChunkGenerationQueue.push(chunks.size() - 1);
        }
    }

    bool World::areNeighboursGenerated(glm::ivec2 pos)
    {
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                auto it = std::ranges::find_if(chunks.begin(), chunks.end(), [&pos, &i, &j](Chunk& chunk) -> bool
                {
                    return glm::ivec2(chunk.getPosition()) == pos + glm::ivec2(i, j);
                });

                if (it == chunks.end()) return false;
                if (!it->isGenerated()) return false;
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

        auto it = std::ranges::find_if(chunks.begin(), chunks.end(), [&chunkPos](Chunk& chunk) -> bool
        {
            return chunk.getPosition().x == chunkPos.x && chunk.getPosition().y == chunkPos.y;
        });

        if (it == chunks.end())
            return BlockType::AIR;

        return it->getBlock(localPos);
    }


}
