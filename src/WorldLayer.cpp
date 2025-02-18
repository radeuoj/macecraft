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
    GenerateChunksIfNeeded(game.GetCamera()->position);
}

void WorldLayer::OnRender(float deltaTime)
{
    RenderChunks(game.GetShader(), game.GetCamera()->position, game.GetFrustum());
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
        chunk.RenderIfNeeded(game.GetAtlas(),
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

    // for (auto& [pos, chunk] : chunks)
    // {
    //     RenderChunkIfNeeded(pos, chunk);
    //
    //     if (ENABLE_FRUSTUM_CULLING)
    //     {
    //         if (chunk.IsOnFrustum(frustum))
    //         {
    //             chunk.Flush(shader, game.GetAtlas());
    //             chunksFlushedThisFrame++;
    //         }
    //     }
    //     else
    //     {
    //         chunk.Flush(shader, game.GetAtlas());
    //         chunksFlushedThisFrame++;
    //     }
    // }

    glm::ivec2 playerChunkPos = WorldPosToChunkPos(playerPosition).first;

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
                chunk.Flush(shader, game.GetAtlas());
                chunksFlushedThisFrame++;
            }
        }
        else
        {
            chunk.Flush(shader, game.GetAtlas());
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
        chunks.at(pos).Generate(game.GetAtlas());
        m_ChunkGenerationCounter++;
    }
}

void WorldLayer::CleanupChunks(glm::vec3 playerPosition)
{
    glm::ivec2 playerChunkPos = WorldPosToChunkPos(playerPosition).first;
    std::erase_if(chunks, [&playerChunkPos](auto& kv) -> bool
    {
        return SquareDistance<int>(kv.first, playerChunkPos) > CHUNK_RENDER_DISTANCE + CHUNK_RENDER_DISTANCE_ERROR;
    });
}

template <typename T> T WorldLayer::SquareDistance(const glm::vec<2, T>& a, const glm::vec<2, T>& b)
{
    return std::max(abs(a.x - b.x), abs(a.y - b.y));
}

// float World::SquareDistance(const glm::vec2 a, const glm::vec2 b)
// {
//     return std::max(abs(a.x - b.x), abs(a.y - b.y));
// }
//
// int World::SquareDistance(const glm::ivec2 a, const glm::ivec2 b)
// {
//     return std::max(abs(a.x - b.x), abs(a.y - b.y));
// }

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

void WorldLayer::DeleteChunkIfExists(const glm::ivec2& pos)
{
    chunks.erase(pos);
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

/**
 * @deprecated is inefficient
 */
BlockType WorldLayer::GetBlock(const glm::ivec3& pos)
{
    // Getting the chunk coords
    glm::ivec3 localPos = glm::ivec3(pos.x & (Chunk::SIZE - 1), pos.y, pos.z & (Chunk::SIZE - 1));
    glm::ivec2 chunkPos = glm::ivec2((pos.x - localPos.x) / Chunk::SIZE, (pos.z - localPos.z) / Chunk::SIZE);

    auto it = chunks.find(chunkPos);

    if (it == chunks.end())
        return BlockType::AIR;

    return it->second.GetBlock(localPos.x, localPos.y, localPos.z);
}


}
