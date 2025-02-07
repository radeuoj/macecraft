#include "World.h"

namespace Macecraft
{

    World::World()
    {
        constexpr int S = 4;
        for (int i = 0; i < S; i++)
        {
            for (int j = 0; j < S; j++)
            {
                chunks.push_back(Chunk(*this, {i, j}));
            }
        }
    }

}
