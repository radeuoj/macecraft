#pragma once

#include "FrustumCulling/Frustum.h"
#include <glm/glm.hpp>

#include "BlockType.h"
#include "Renderer.h"
#include "TextureAtlas.h"

namespace Macecraft
{
    
class World;

class Chunk
{
public:
    static constexpr int SIZE = 16;
    static constexpr int HEIGHT = 256;

    Chunk(World* world, glm::i16vec2 position);
    Chunk(const Chunk& other) = delete;
    Chunk(Chunk&& other) noexcept;

    bool IsGenerated();
    void Generate(TextureAtlas* atlas);
    void RenderWhenPossible();
    void RenderIfNeeded(TextureAtlas* atlas);
    void Flush(const Shader* shader, const TextureAtlas* atlas);
    BlockType GetBlock(glm::ivec3 pos);
    glm::i16vec2 GetPosition();
    bool IsOnFrustum(const Frustum& frustum);
    bool IsOnOrForwardOfLine(const Line& line);

    // TODO: not good
    Renderer m_Renderer;
    bool m_ShouldRender = true;
private:
    World* m_World;
    glm::i16vec2 m_Position = glm::i16vec2(0);
    BlockType m_Blocks[SIZE][HEIGHT][SIZE] { BlockType::AIR };
    bool m_IsGenerated = false;

    void Render(TextureAtlas* atlas);
};

}
