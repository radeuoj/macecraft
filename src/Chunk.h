#pragma once

#include "FrustumCulling/Frustum.h"
#include <glm/glm.hpp>

#include "BlockType.h"
#include "Camera.h"
#include "Renderer.h"
#include "TextureAtlas.h"

namespace Macecraft
{

class Chunk
{
public:
    static constexpr int SIZE = 16;
    static constexpr int HEIGHT = 256;

    Chunk(glm::ivec2 position);
    Chunk(const Chunk& other) = delete;
    Chunk(Chunk&& other) noexcept;

    bool IsGenerated();
    void Generate(const TextureAtlas* atlas);
    void RenderWhenPossible();
    bool ShouldRender() const;

    /**
     * 
     * @param nXP neighbour on positive x direction
     */
    void RenderIfNeeded(const TextureAtlas* atlas, Chunk* nXP, Chunk* nXN, Chunk* nYP, Chunk* nYN);
    
    void Flush(const Shader* shader, const TextureAtlas* atlas);
    glm::ivec2 GetPosition();
    bool IsOnFrustum(const Frustum* frustum);
    bool IsOnOrForwardOfPlane(const Plane& plane);

    BlockType NeighbourGetBlock(glm::ivec3 pos, Chunk* nXP, Chunk* nXN, Chunk* nYP, Chunk* nYN);
    
    /**
     * unsafe
     */
    BlockType GetBlock(int x, int y, int z) const;
    void SetBlock(int x, int y, int z, BlockType block);

private:
    glm::ivec2 m_Position = glm::ivec2(0);
    BlockType m_Blocks[SIZE * HEIGHT * SIZE] { BlockType::AIR };
    bool m_IsGenerated = false;
    Renderer<VertexData> m_Renderer;
    bool m_ShouldRender = true;

    void Render(const TextureAtlas* atlas, Chunk* nXP, Chunk* nXN, Chunk* nYP, Chunk* nYN);
};

}
