#include "Chunk.h"

#include <iostream>
#include <cstring>

#include "World.h"

namespace Macecraft
{
Chunk::Chunk(World* world, glm::i16vec2 position): m_World(world), m_Position(position)
{
}

Chunk::Chunk(Chunk&& other) noexcept:
m_Renderer(std::move(other.m_Renderer)),
m_World(other.m_World),
m_Position(other.m_Position)
{
    memcpy(m_Blocks, other.m_Blocks, SIZE * HEIGHT * SIZE);
}

bool Chunk::IsGenerated()
{
    return m_IsGenerated;
}

void Chunk::Generate(TextureAtlas* atlas)
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            for (int k = 0; k < SIZE; k++)
            {
                m_Blocks[i][j][k] = BlockType::DIRT;
            }
        }
    }

    for (int i = 0; i < SIZE; i++)
    {
        for (int k = 0; k < SIZE; k++)
        {
            this->m_Blocks[i][5][k] = BlockType::GRASS;
        }
    }

    m_IsGenerated = true;
}

void Chunk::RenderWhenPossible()
{
    m_ShouldRender = true;
}

void Chunk::RenderIfNeeded(TextureAtlas* atlas)
{
    if (m_ShouldRender)
        Render(atlas);
}

void Chunk::Render(TextureAtlas* atlas)
{
    if (!m_IsGenerated) return;

    if (!m_World->AreNeighboursGenerated(m_Position)) return;

    m_ShouldRender = false;
    m_Renderer.vertices.clear();

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            for (int k = 0; k < SIZE; k++)
            {
                // if (GetBlock({i, j, k}) != BlockType::AIR)
                if (this->m_Blocks[i][j][k] != BlockType::AIR)
                {
                    // glm::u8vec3 blockPosition = {this->position.x * 16 + i, j, this->position.y * 16 + k};
                    glm::u8vec3 blockPosition = {i, j, k};

                    using enum BlockDirection;

                    // BACK
                    if (GetBlock({i, j, k - 1}) == BlockType::AIR)
                    {
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], ZN, 0, 0)) });
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], ZN, 0, 1)) });
                        m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], ZN, 1, 0)) });
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], ZN, 0, 1)) });
                        m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], ZN, 1, 1)) });
                        m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], ZN, 1, 0)) });
                    }

                    // FRONT
                    if (GetBlock({i, j, k + 1}) == BlockType::AIR)
                    {
                        m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], ZP, 0, 0)) });
                        m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], ZP, 0, 1)) });
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], ZP, 1, 0)) });
                        m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], ZP, 0, 1)) });
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], ZP, 1, 1)) });
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], ZP, 1, 0)) });
                    }

                    //  BOTTOM
                    if (GetBlock({i, j - 1, k}) == BlockType::AIR)
                    {
                        m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], YN, 0, 0)) });
                        m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], YN, 0, 1)) });
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], YN, 1, 0)) });
                        m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], YN, 0, 1)) });
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], YN, 1, 1)) });
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], YN, 1, 0)) });
                    }

                    // TOP
                    if (GetBlock({i, j + 1, k}) == BlockType::AIR)
                    {
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], YP, 0, 0)) });
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], YP, 0, 1)) });
                        m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], YP, 1, 0)) });
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], YP, 0, 1)) });
                        m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], YP, 1, 1)) });
                        m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], YP, 1, 0)) });
                    }

                    // LEFT
                    if (GetBlock({i - 1, j, k}) == BlockType::AIR)
                    {
                        m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], XN, 0, 0)) });
                        m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], XN, 0, 1)) });
                        m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], XN, 1, 0)) });
                        m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], XN, 0, 1)) });
                        m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], XN, 1, 1)) });
                        m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], XN, 1, 0)) });
                    }

                    // RIGHT
                    if (GetBlock({i + 1, j, k}) == BlockType::AIR)
                    {
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], XP, 0, 0)) });
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], XP, 0, 1)) });
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], XP, 1, 0)) });
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], XP, 0, 1)) });
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], XP, 1, 1)) });
                        m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(m_Blocks[i][j][k], XP, 1, 0)) });
                    }
                }
            }
        }
    }

    m_Renderer.BindVertices();
    // m_Renderer.flush();
}

void Chunk::Flush(const Shader* shader, const TextureAtlas* atlas)
{
    if (!m_IsGenerated) return;

    // if (m_ShouldRender) render(atlas);

    glUniform2i(glGetUniformLocation(shader->GetID(), "chunkPos"), m_Position.x, m_Position.y);
    m_Renderer.Flush();
}

bool Chunk::IsOnFrustum(const Frustum& frustum)
{
    return IsOnOrForwardOfLine(frustum.nearPlane)
    && IsOnOrForwardOfLine(frustum.rightPlane)
    && IsOnOrForwardOfLine(frustum.leftPlane);
}

bool Chunk::IsOnOrForwardOfLine(const Plane &plane)
{
    constexpr float EXTENTXZ = float(SIZE / 2);
    constexpr float EXTENTY = float(HEIGHT / 2);

    // https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
    const float r = EXTENTXZ * abs(plane.normal.x) + EXTENTY * abs(plane.normal.y) + EXTENTXZ * abs(plane.normal.z);

    return -r <= plane.GetSignedDistanceToPlane(glm::vec3(m_Position.x * SIZE + EXTENTXZ, EXTENTY, m_Position.y * SIZE + EXTENTXZ));

}

BlockType Chunk::GetBlock(glm::ivec3 pos)
{
    using enum BlockType;

    if (!m_IsGenerated) return AIR;

    if (pos.y < 0 || pos.y >= HEIGHT)
        return AIR;

    if (pos.x < 0 || pos.x >= SIZE || pos.z < 0 || pos.z >= SIZE)
    {
        pos.x += m_Position.x * SIZE;
        pos.z += m_Position.y * SIZE;
        return m_World->GetBlock(pos);
    }

    return this->m_Blocks[pos.x][pos.y][pos.z];
}

glm::i16vec2 Chunk::GetPosition()
{
    return m_Position;
}


}
