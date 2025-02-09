#include "World.h"

namespace Macecraft
{

    void World::init()
    {
        // TODO: ADD COPY CONSTRUCTOR TO CHUNKS ASAP CUZ ITS MESSING EVERYTHING UPPP OHH I HATE THIS
        // chunks.reserve(65);
        constexpr int S = 16;
        for (int i = 0; i < S; i++)
        {
            for (int j = 0; j < S; j++)
            {
                // chunks.push_back(Chunk(*this, {i, j}));
                chunks.emplace_back(this, glm::i16vec2(i, j));
            }
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
