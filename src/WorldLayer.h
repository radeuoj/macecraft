#pragma once

#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/hash.hpp>

#include "Chunk.h"
#include "Layer.h"
#include "FrustumCulling/Frustum.h"

namespace Macecraft
{
    
class WorldLayer: public Layer
{
public:
    using Layer::Layer;
    
    void OnLoad() override;
    void OnUnload() override;
    void OnUpdate(float deltaTime) override;
    void OnRender(float deltaTime) override;
    void OnImGuiRender(float deltaTime) override;
    
    static constexpr int CHUNK_GENERATION_LIMIT_PER_FRAME = 2;
    static inline int CHUNK_RENDER_DISTANCE = 30;
    static constexpr int CHUNK_CLEAN_LIMIT_PER_FRAME = 10;
    static constexpr int CHUNK_RENDER_DISTANCE_ERROR = 2; // if a chunk is more than CHUNK_RENDER_DISTANCE + CHUNK_RENDER_DISTANCE_ERROR away from the player, it will be deleted

    std::unordered_map<glm::ivec2, Chunk> chunks;

    bool ENABLE_FRUSTUM_CULLING = true;
    int chunksFlushedThisFrame = 0;

    template <typename T> static T SquareDistance(const glm::vec<2, T>& a, const glm::vec<2, T>& b);
    static glm::vec3 ChunkPosToWorldPos(const glm::ivec2& chunkPos, const glm::vec3& pos);
    static std::pair<glm::ivec2, glm::vec3> WorldPosToChunkPos(const glm::vec3& pos);
    static std::pair<glm::ivec2, glm::ivec3> WorldPosToChunkPos(const glm::ivec3& pos);

    /**
     * Casts a ray in the block world\n
     * <a href="https://www.youtube.com/watch?v=NbSee-XM7WA&t=1386s">Video where i got the algorithm from</a>
     * @param origin origin of the raycast
     * @param direction raycast direction
     * @param maxDistance max distance aka reach
     * @param hitBlock returns the block hit if the ray is successful
     * @param hitChunkPos returns the chunk in which the hit is
     * @param hitPosInChunk returns the position of the hit block inside the chunk
     * @return if the ray was successful aka if it even hit something
     */
    bool Raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance, BlockType* hitBlock, glm::ivec2* hitChunkPos, glm::ivec3* hitPosInChunk) const;
    
private:
    void GenerateChunksIfNeeded(const glm::vec3& playerPosition);
    bool AddChunkIfDoesntExist(const glm::ivec2& pos);
    bool AreNeighboursGenerated(const glm::ivec2& pos);
    void RenderChunks(const Shader* shader, const glm::vec3& playerPosition, const Frustum* frustum);
    void SafeGenerateChunk(const glm::ivec2& pos);
    void RenderChunkIfNeeded(const glm::ivec2& pos, Chunk& chunk);

    int m_ChunkGenerationCounter = 0;

};

}

