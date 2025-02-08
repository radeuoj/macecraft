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

}
