#pragma once

#include <vector>

#include "chunk.h"

class World
{
public:
    World();

private:
    std::vector<Chunk> chunks;
};
