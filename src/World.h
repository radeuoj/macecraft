#pragma once

#include <vector>

#include "Chunk.h"

class World
{
public:
    World();

private:
    std::vector<Chunk> m_Chunks;
};
