#pragma once

#include <vector>

#include "Chunk.h"

namespace Macecraft
{

    class World
    {
    public:
        std::vector<Chunk> chunks;

        World() = default;
        void init();

        BlockType getBlock(glm::ivec3 pos);

    };

}

