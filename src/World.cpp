#include "World.h"

namespace Macecraft
{

    void World::init()
    {
        constexpr int S = 4;
        for (int i = 0; i < S; i++)
        {
            for (int j = 0; j < S; j++)
            {
                // chunks.push_back(Chunk(*this, {i, j}));
                chunks.push_back(Chunk{*this, {i, j}});
            }
        }
    }

}
