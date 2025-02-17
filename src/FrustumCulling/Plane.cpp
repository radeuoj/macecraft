#include "Plane.h"
#include "../Chunk.h"

namespace Macecraft 
{

void Plane::Normalize()
{
    float length = glm::length(normal);
    normal /= length;
    distance /= length;
}

void Plane::GenerateP()
{
    chunkP = glm::vec3(0);
    glm::vec3 maxp = chunkP + glm::vec3(Chunk::SIZE, Chunk::HEIGHT, Chunk::SIZE);

    // This finds the point most in the direction of the planes normal
    // this helps to find if the outmost point of the chunk is in front of the say plane
    // in this case we consider the chunk to be in front of the plane
    if (normal.x >= 0)
        chunkP.x = maxp.x;
    if (normal.y >= 0)
        chunkP.y = maxp.y;
    if (normal.z >= 0)
        chunkP.z = maxp.z;
}

}


