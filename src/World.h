#pragma once

#include <vector>
#include <queue>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/hash.hpp>

#include "Chunk.h"
#include "FrustumCulling/Frustum.h"

namespace Macecraft
{
    
class World
{
public:
    static constexpr int CHUNK_GENERATION_LIMIT_PER_FRAME = 2;
    static constexpr int CHUNK_RENDER_DISTANCE = 30;
    static constexpr int CHUNK_RENDER_DISTANCE_ERROR = 2; // if a chunk is more than CHUNK_RENDER_DISTANCE + CHUNK_RENDER_DISTANCE_ERROR away from the player, it will be deleted

    std::unordered_map<glm::ivec2, Chunk> chunks;

    bool ENABLE_FRUSTUM_CULLING = true;
    int chunksFlushedThisFrame = 0;

    World(TextureAtlas* atlas);
    
    void GenerateChunksIfNeeded(const glm::vec3& playerPosition);
    bool AddChunkIfDoesntExist(const glm::ivec2& pos);
    bool AreNeighboursGenerated(const glm::ivec2& pos);
    void RenderChunks(const Shader* shader, const glm::vec3& playerPosition, const Frustum& frustum);
    void DeleteChunkIfExists(const glm::ivec2& pos);
    void SafeGenerateChunk(const glm::ivec2& pos);

    /**
     * @deprecated fix
     */
    [[deprecated]] void CleanupChunks(glm::vec3 playerPosition);

    template <typename T> static T SquareDistance(const glm::vec<2, T>& a, const glm::vec<2, T>& b);
    // static float SquareDistance(glm::vec2 a, glm::vec2 b);
    // static int SquareDistance(glm::ivec2 a, glm::ivec2 b);
    static glm::vec3 ChunkPosToWorldPos(const glm::ivec2& chunkPos, const glm::vec3& pos);
    static std::pair<glm::ivec2, glm::vec3> WorldPosToChunkPos(const glm::vec3& pos);

    [[deprecated]]
    BlockType GetBlock(const glm::ivec3& pos);

private:
    int m_ChunkGenerationCounter = 0;
    TextureAtlas* m_Atlas = nullptr;

};

}

