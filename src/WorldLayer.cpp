#include "WorldLayer.h"

#include <iostream>

#include "Game.h"
#include "imgui.h"

#undef max
#undef min

namespace Macecraft
{

void WorldLayer::OnLoad()
{
}

void WorldLayer::OnUnload()
{
}

void WorldLayer::OnUpdate(float deltaTime)
{
    GenerateChunksIfNeeded(game->GetCamera()->position);
}

void WorldLayer::OnRender(float deltaTime)
{
    RenderChunks(game->GetShader(), game->GetCamera()->position, game->GetFrustum());
}

void WorldLayer::OnImGuiRender(float deltaTime)
{
    ImGui::Text("%llu chunks loaded (hopefully)", chunks.size());
    ImGui::Text("%d chunks flushed this frame", chunksFlushedThisFrame);
    ImGui::Checkbox("Enable frustum culling", &ENABLE_FRUSTUM_CULLING);
    ImGui::SliderInt("Render distance", &WorldLayer::CHUNK_RENDER_DISTANCE, 0, 100);
}

void WorldLayer::RenderChunkIfNeeded(const glm::ivec2& pos, Chunk& chunk)
{
    if (chunk.ShouldRender() && AreNeighboursGenerated(pos))
    {
        // TODO: make this multi threaded
        chunk.RenderIfNeeded(game->GetTextureArray(),
            &chunks.at(pos + glm::ivec2(1, 0)),
            &chunks.at(pos + glm::ivec2(-1, 0)),
            &chunks.at(pos + glm::ivec2(0, 1)),
            &chunks.at(pos + glm::ivec2(0, -1))
        );
    }
}

void WorldLayer::RenderChunks(const Shader* shader, const glm::vec3& playerPosition, const Frustum* frustum)
{
    chunksFlushedThisFrame = 0;
    int chunksCleanedThisFrame = 0;

    glm::ivec2 playerChunkPos = WorldPosToChunkPos(playerPosition).first;

    // TODO: maybe dont clean up chunks while rendering but ok

    for (auto it = chunks.begin(); it != chunks.end(); )
    {
        auto& pos = it->first;
        auto& chunk = it->second;
        
        if (chunksCleanedThisFrame < CHUNK_CLEAN_LIMIT_PER_FRAME
            && SquareDistance<int>(pos, playerChunkPos) > CHUNK_RENDER_DISTANCE + CHUNK_RENDER_DISTANCE_ERROR)
        {
            it = chunks.erase(it);
            chunksCleanedThisFrame++;
            continue;
        }
        
        RenderChunkIfNeeded(pos, chunk);

        if (ENABLE_FRUSTUM_CULLING)
        {
            if (chunk.IsOnFrustum(frustum))
            {
                chunk.Flush(shader);
                chunksFlushedThisFrame++;
            }
        }
        else
        {
            chunk.Flush(shader);
            chunksFlushedThisFrame++;
        }

        ++it;
    }
}

void WorldLayer::GenerateChunksIfNeeded(const glm::vec3& playerPosition)
{
    glm::ivec2 playerChunk = WorldPosToChunkPos(playerPosition).first;
    m_ChunkGenerationCounter = 0;

    SafeGenerateChunk(playerChunk);

    for (int dist = 1; dist <= CHUNK_RENDER_DISTANCE; dist++)
    {
        // 1 0
        SafeGenerateChunk(playerChunk + glm::ivec2(dist, 0));

        for (int i = 1; i < dist; i++)
        {
            SafeGenerateChunk(playerChunk + glm::ivec2(dist, i));
            SafeGenerateChunk(playerChunk + glm::ivec2(dist, -i));
        }

        // -1 0
        SafeGenerateChunk(playerChunk + glm::ivec2(-dist, 0));

        for (int i = 1; i < dist; i++)
        {
            SafeGenerateChunk(playerChunk + glm::ivec2(-dist, i));
            SafeGenerateChunk(playerChunk + glm::ivec2(-dist, -i));
        }

        // 0 1
        SafeGenerateChunk(playerChunk + glm::ivec2(0, dist));

        for (int i = 1; i < dist; i++)
        {
            SafeGenerateChunk(playerChunk + glm::ivec2(i, dist));
            SafeGenerateChunk(playerChunk + glm::ivec2(-i, dist));
        }

        // 0 -1
        SafeGenerateChunk(playerChunk + glm::ivec2(0, -dist));

        for (int i = 1; i < dist; i++)
        {
            SafeGenerateChunk(playerChunk + glm::ivec2(i, -dist));
            SafeGenerateChunk(playerChunk + glm::ivec2(-i, -dist));
        }

        SafeGenerateChunk(playerChunk + glm::ivec2(dist, dist));
        SafeGenerateChunk(playerChunk + glm::ivec2(-dist, dist));
        SafeGenerateChunk(playerChunk + glm::ivec2(dist, -dist));
        SafeGenerateChunk(playerChunk + glm::ivec2(-dist, -dist));
    }
}

void WorldLayer::SafeGenerateChunk(const glm::ivec2& pos)
{
    if (m_ChunkGenerationCounter < CHUNK_GENERATION_LIMIT_PER_FRAME && !AddChunkIfDoesntExist(pos))
    {
        chunks.at(pos).Generate();
        m_ChunkGenerationCounter++;
    }
}

template <typename T> T WorldLayer::SquareDistance(const glm::vec<2, T>& a, const glm::vec<2, T>& b)
{
    return std::max(abs(a.x - b.x), abs(a.y - b.y));
}

glm::vec3 WorldLayer::ChunkPosToWorldPos(const glm::ivec2& chunkPos, const glm::vec3& pos)
{
    return glm::vec3(chunkPos.x * Chunk::SIZE + pos.x, pos.y, chunkPos.y * Chunk::SIZE + pos.y);
}

std::pair<glm::ivec2, glm::vec3> WorldLayer::WorldPosToChunkPos(const glm::vec3& pos)
{
    glm::ivec2 chunkPos = glm::ivec2(0);
    glm::vec3 localPos = glm::vec3(0);
    
    chunkPos.x = int(pos.x) / Chunk::SIZE;
    chunkPos.y = int(pos.z) / Chunk::SIZE;
    
    if (pos.x < 0)
        chunkPos.x -= 1;
    
    if (pos.z < 0)
        chunkPos.y -= 1;
    
    localPos.x = pos.x - chunkPos.x * Chunk::SIZE;
    localPos.z = pos.z - chunkPos.y * Chunk::SIZE;
    localPos.y = pos.y;
    
    return std::make_pair(chunkPos, localPos);
}

std::pair<glm::ivec2, glm::ivec3> WorldLayer::WorldPosToChunkPos(const glm::ivec3& pos)
{
    glm::ivec3 localPos = glm::ivec3(pos.x & 15, pos.y, pos.z & 15);
    glm::ivec2 chunkPos = glm::ivec2((pos.x - localPos.x) / Chunk::SIZE, (pos.z - localPos.z) / Chunk::SIZE);
    return std::make_pair(chunkPos, localPos);
}

/**
 * @return if it already existed
 */
bool WorldLayer::AddChunkIfDoesntExist(const glm::ivec2& pos)
{
    if (chunks.find(pos) == chunks.end())
    {
        chunks.insert(std::make_pair(pos, Chunk(pos)));

        return false;
    }

    return true;
}

bool WorldLayer::AreNeighboursGenerated(const glm::ivec2& pos)
{
    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            auto it = chunks.find(pos + glm::ivec2(i, j));

            if (it == chunks.end()) return false;
            if (!it->second.IsGenerated()) return false;
        }
    }

    return true;
}

bool WorldLayer::Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, BlockType* hitBlock, glm::ivec2* hitChunkPos, glm::ivec3* hitPosInChunk) const
{
    float directionLength = glm::length(direction);

    // NOTE: this might be broken
    glm::vec3 rayUnitStepSize = glm::vec3(0.0f);

    if (direction.x != 0.0f)
        rayUnitStepSize.x = directionLength * (1.0f / abs(direction.x));
    if (direction.y != 0.0f)
        rayUnitStepSize.y = directionLength * (1.0f / abs(direction.y));
    if (direction.z != 0.0f)
        rayUnitStepSize.z = directionLength * (1.0f / abs(direction.z));

    glm::ivec3 blockInWorldPos = glm::floor(origin);
    glm::vec3 rayLength1D = glm::vec3(0.0f);

    glm::ivec3 step = glm::ivec3(0);

    if (direction.x < 0.0f)
    {
        step.x = -1;
        rayLength1D.x = (origin.x - float(blockInWorldPos.x)) * rayUnitStepSize.x;
    }
    else if (direction.x > 0.0f)
    {
        step.x = 1;
        rayLength1D.x = (float(blockInWorldPos.x + 1) - origin.x) * rayUnitStepSize.x;
    }

    if (direction.y < 0.0f)
    {
        step.y = -1;
        rayLength1D.y = (origin.y - float(blockInWorldPos.y)) * rayUnitStepSize.y;
    }
    else if (direction.y > 0.0f)
    {
        step.y = 1;
        rayLength1D.y = (float(blockInWorldPos.y + 1) - origin.y) * rayUnitStepSize.y;
    }

    if (direction.z < 0.0f)
    {
        step.z = -1;
        rayLength1D.z = (origin.z - float(blockInWorldPos.z)) * rayUnitStepSize.z;
    }
    else if (direction.z > 0.0f)
    {
        step.z = 1;
        rayLength1D.z = (float(blockInWorldPos.z + 1) - origin.z) * rayUnitStepSize.z;
    }

    glm::ivec2 chunkPos = glm::ivec2(0);
    glm::ivec3 blockInChunkPos = glm::ivec3(0);
    BlockType blockType = BlockType::AIR;

    bool blockFound = false;
    float distance = 0.0f;
    while (!blockFound && distance < maxDistance)
    {
        // Checking if we got a hit
        auto chunkPosData = WorldPosToChunkPos(blockInWorldPos);
        chunkPos = chunkPosData.first;
        blockInChunkPos = chunkPosData.second;
        
        if (blockInChunkPos.y >= 0 && blockInChunkPos.y < Chunk::HEIGHT && chunks.find(chunkPos) != chunks.end())
        {
            blockType = chunks.at(chunkPos).GetBlock(blockInChunkPos.x, blockInChunkPos.y, blockInChunkPos.z);
            if (blockType != BlockType::AIR)
            {
                blockFound = true;
            }
        }
        
        // Walk
        if (step.x != 0  && rayLength1D.x < rayLength1D.y && rayLength1D.x < rayLength1D.z)
        {
            blockInWorldPos.x += step.x;
            distance = glm::max(distance, rayLength1D.x);
            rayLength1D.x += rayUnitStepSize.x;
        }
        else if (step.y != 0 && rayLength1D.y < rayLength1D.x && rayLength1D.y < rayLength1D.z)
        {
            blockInWorldPos.y += step.y;
            distance = glm::max(distance, rayLength1D.y);
            rayLength1D.y += rayUnitStepSize.y;
        }
        else if (step.z != 0)
        {
            blockInWorldPos.z += step.z;
            distance = glm::max(distance, rayLength1D.z);
            rayLength1D.z += rayUnitStepSize.z;
        }
    }

    if (blockFound)
    {
        *hitBlock = blockType;
        *hitChunkPos = chunkPos;
        *hitPosInChunk = blockInChunkPos;
        return true;
    }

    return false;
}


}
