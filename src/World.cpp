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

    std::queue<glm::ivec2> queue;

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

        //constexpr int S = 32;
        //for (int i = 0; i < S; i++)
        //{
        //    for (int j = 0; j < S; j++)
        //    {
        //        // If its not rendered yet
        //        addChunkIfDoesntExist(glm::ivec2(i, j));
        //    }
        //}

        
        queue.emplace(0, 0); // This would basically be player position

        /*addChunkIfDoesntExist(glm::ivec2(0, 0));
        queue.emplace(1, 0);
        queue.emplace(-1, 0);
        queue.emplace(0, 1);
        queue.emplace(0, -1);*/

        int i = 2;
        while (i > 0 && !queue.empty() && chunks.size() < 3096)
        {
            glm::ivec2 pos = queue.front();
            queue.pop();

            bool alreadyExists = addChunkIfDoesntExist(pos);

            if (alreadyExists)
                continue;

            if (pos.x == 0 && pos.y == 0)
            {
                queue.emplace(1, pos.y);
                queue.emplace(-1, pos.y);
                queue.emplace(pos.x, 1);
                queue.emplace(pos.x, -1);
            }

            if (pos.x == 0 && pos.y > 0)
            {
                queue.emplace(pos.x, pos.y + 1);
                queue.emplace(pos.x - 1, pos.y);
                queue.emplace(pos.x + 1, pos.y);
            }

            if (pos.x == 0 && pos.y < 0)
            {
                queue.emplace(pos.x, pos.y - 1);
                queue.emplace(pos.x - 1, pos.y);
                queue.emplace(pos.x + 1, pos.y);
            }

            if (pos.x > 0 && pos.y == 0)
            {
                queue.emplace(pos.x + 1, pos.y);
                queue.emplace(pos.x - 1, pos.y - 1);
                queue.emplace(pos.x, pos.y + 1);
            }

            if (pos.x < 0 && pos.y == 0)
            {
                queue.emplace(pos.x - 1, pos.y);
                queue.emplace(pos.x, pos.y) - 1;
                queue.emplace(pos.x, pos.y + 1);
            }

            if (pos.x < 0 && pos.y > 0)
            {
                queue.emplace(pos.x - 1, pos.y);
                queue.emplace(pos.x, pos.y + 1);
            }

            if (pos.x > 0 && pos.y > 0)
            {
                queue.emplace(pos.x + 1, pos.y);
                queue.emplace(pos.x, pos.y + 1);
            }

            if (pos.x > 0 && pos.y < 0)
            {
                queue.emplace(pos.x + 1, pos.y);
                queue.emplace(pos.x, pos.y - 1);
            }

            if (pos.x < 0 && pos.y < 0)
            {
                queue.emplace(pos.x - 1, pos.y);
                queue.emplace(pos.x, pos.y - 1);
            }

            i--;
        }

        if (!m_ChunkGenerationQueue.empty())
        {
            chunks.at(m_ChunkGenerationQueue.front()).generate();
            m_ChunkGenerationQueue.pop();
        }
    }

    /**
     * @return wether it already existed
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
