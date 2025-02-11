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
        for (auto& [pos, chunk] : chunks)
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

        // std::queue<glm::ivec2> bfs;
        // bfs.emplace(0, 0);
        //
        // addChunkIfDoesntExist(glm::ivec2(0, 0));
        //
        // while (!bfs.empty() && chunks.size() < 128)
        // {
        //     auto pos = bfs.front();
        //     bfs.pop();
        //
        //     for (int i = -1; i <= 1; i++)
        //     {
        //         for (int j = -1; j <= 1; j++)
        //         {
        //             addChunkIfDoesntExist(pos + glm::ivec2(i, j));
        //             bfs.push(pos + glm::ivec2(i, j));
        //         }
        //     }
        // }

        constexpr int S = 8;
        for (int i = 0; i < S; i++)
        {
            for (int j = 0; j < S; j++)
            {
                // If its not rendered yet
                if (chunks.find(glm::ivec2(i, j)) == chunks.end())
                {
                    chunks.insert(std::make_pair(glm::ivec2(i, j), Chunk(this, glm::ivec2(i, j))));
                    m_ChunkGenerationQueue.emplace(i, j);
                }
            }
        }

        if (!m_ChunkGenerationQueue.empty())
        {
            chunks.at(m_ChunkGenerationQueue.front()).generate();
            m_ChunkGenerationQueue.pop();
        }
    }

    // void World::addChunkIfDoesntExist(glm::ivec2 pos)
    // {
    //     auto it = std::ranges::find_if(chunks.begin(), chunks.end(), [&pos](auto& chunk) -> bool
    //     {
    //         return glm::ivec2(chunk.second.getPosition()) == pos;
    //     });
    //
    //     if (it == chunks.end())
    //     {
    //         // chunks[pos] = std::move(Chunk(this, pos));
    //         // chunks.insert(std::make_pair(pos, std::move(Chunk(this, pos))));
    //         // chunks.emplace(std::make_pair(pos, Chunk(this, pos)));
    //         // chunks[pos] = std::make_unique<Chunk>(this, pos);
    //         // chunks.insert(std::make_pair(pos, std::make_unique<Chunk>(this, pos)));
    //         // chunks.insert_or_assign(pos, Chunk(this, pos));
    //         chunks.insert(std::make_pair(pos.x, Chunk(this, pos)));
    //         m_ChunkGenerationQueue.push(pos);
    //     }
    // }

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
