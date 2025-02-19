#include "Chunk.h"

#include <iostream>
#include <cstring>

#include "TextureArray.h"

namespace Macecraft
{
Chunk::Chunk(glm::ivec2 position): m_Position(position)
{
    m_Renderer.LinkAttribIPointer(0, 3, GL_UNSIGNED_BYTE, sizeof(VertexData), offsetof(VertexData, x));
    m_Renderer.LinkAttribIPointer(1, 1, GL_UNSIGNED_BYTE, sizeof(VertexData), offsetof(VertexData, texCoords));
}

Chunk::Chunk(Chunk&& other) noexcept:
m_Renderer(std::move(other.m_Renderer)),
m_Position(other.m_Position)
{
    memcpy(m_Blocks, other.m_Blocks, SIZE * HEIGHT * SIZE);
}

bool Chunk::IsGenerated()
{
    return m_IsGenerated;
}

void Chunk::Generate()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            for (int k = 0; k < SIZE; k++)
            {
                SetBlock(i, j, k, BlockType::DIRT);
            }
        }
    }

    for (int i = 0; i < SIZE; i++)
    {
        for (int k = 0; k < SIZE; k++)
        {
            SetBlock(i, 5, k, BlockType::GRASS);
        }
    }

    m_IsGenerated = true;
}

void Chunk::RenderWhenPossible()
{
    m_ShouldRender = true;
}

bool Chunk::ShouldRender() const
{
    return m_ShouldRender;
}

void Chunk::RenderIfNeeded(const TextureArray* atlas, Chunk* nXP, Chunk* nXN, Chunk* nYP, Chunk* nYN)
{
    if (m_ShouldRender)
        Render(atlas, nXP, nXN, nYP, nYN);
}

void Chunk::Render(const TextureArray* atlas, Chunk* nXP, Chunk* nXN, Chunk* nYP, Chunk* nYN)
{
    if (!m_IsGenerated) return;

    m_ShouldRender = false;
    m_Renderer.vertices.clear();

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < HEIGHT; j++)
        {
            for (int k = 0; k < SIZE; k++)
            {
                if (GetBlock(i, j, k) != BlockType::AIR)
                {
                    glm::u8vec3 blockPosition = {i, j, k};

                    using enum BlockDirection;

                    // BACK
                    if (NeighbourGetBlock({i, j, k - 1}, nXP, nXN, nYP, nYN) == BlockType::AIR)
                    {
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), ZN, 0, 0)));
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), ZN, 0, 1)));
                        m_Renderer.vertices.emplace_back(blockPosition.x, blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), ZN, 1, 0)));
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), ZN, 0, 1)));
                        m_Renderer.vertices.emplace_back(blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), ZN, 1, 1)));
                        m_Renderer.vertices.emplace_back(blockPosition.x, blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), ZN, 1, 0)));
                    }

                    // FRONT
                    if (NeighbourGetBlock({i, j, k + 1}, nXP, nXN, nYP, nYN) == BlockType::AIR)
                    {
                        m_Renderer.vertices.emplace_back(blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), ZP, 0, 0)));
                        m_Renderer.vertices.emplace_back(blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), ZP, 0, 1)));
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), ZP, 1, 0)));
                        m_Renderer.vertices.emplace_back(blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), ZP, 0, 1)));
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), ZP, 1, 1)));
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), ZP, 1, 0)));
                    }

                    //  BOTTOM
                    if (NeighbourGetBlock({i, j - 1, k}, nXP, nXN, nYP, nYN) == BlockType::AIR)
                    {
                        m_Renderer.vertices.emplace_back(blockPosition.x, blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), YN, 0, 0)));
                        m_Renderer.vertices.emplace_back(blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), YN, 0, 1)));
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), YN, 1, 0)));
                        m_Renderer.vertices.emplace_back(blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), YN, 0, 1)));
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), YN, 1, 1)));
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), YN, 1, 0)));
                    }

                    // TOP
                    if (NeighbourGetBlock({i, j + 1, k}, nXP, nXN, nYP, nYN) == BlockType::AIR)
                    {
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), YP, 0, 0)));
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), YP, 0, 1)));
                        m_Renderer.vertices.emplace_back(blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), YP, 1, 0)));
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), YP, 0, 1)));
                        m_Renderer.vertices.emplace_back(blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), YP, 1, 1)));
                        m_Renderer.vertices.emplace_back(blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), YP, 1, 0)));
                    }

                    // LEFT
                    if (NeighbourGetBlock({i - 1, j, k}, nXP, nXN, nYP, nYN) == BlockType::AIR)
                    {
                        m_Renderer.vertices.emplace_back(blockPosition.x, blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), XN, 0, 0)));
                        m_Renderer.vertices.emplace_back(blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), XN, 0, 1)));
                        m_Renderer.vertices.emplace_back(blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), XN, 1, 0)));
                        m_Renderer.vertices.emplace_back(blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), XN, 0, 1)));
                        m_Renderer.vertices.emplace_back(blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), XN, 1, 1)));
                        m_Renderer.vertices.emplace_back(blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), XN, 1, 0)));
                    }

                    // RIGHT
                    if (NeighbourGetBlock({i + 1, j, k}, nXP, nXN, nYP, nYN) == BlockType::AIR)
                    {
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), XP, 0, 0)));
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), XP, 0, 1)));
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), XP, 1, 0)));
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), XP, 0, 1)));
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), XP, 1, 1)));
                        m_Renderer.vertices.emplace_back(GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, GLubyte(atlas->GetTextureLocation(GetBlock(i, j, k), XP, 1, 0)));
                    }
                }
            }
        }
    }

    m_Renderer.BindVertices();
    // m_Renderer.flush();
}

void Chunk::Flush(const Shader* shader)
{
    if (!m_IsGenerated) return;

    // if (m_ShouldRender) render(atlas);

    glUniform2i(glGetUniformLocation(shader->GetID(), "chunkPos"), m_Position.x, m_Position.y);
    m_Renderer.Flush();
}

bool Chunk::IsOnFrustum(const Frustum* frustum)
{
    return IsOnOrForwardOfPlane(frustum->nearPlane)
    && IsOnOrForwardOfPlane(frustum->rightPlane)
    && IsOnOrForwardOfPlane(frustum->leftPlane)
    && IsOnOrForwardOfPlane(frustum->bottomPlane)
    && IsOnOrForwardOfPlane(frustum->topPlane);
    // && IsOnOrForwardOfPlane(frustum.farPlane);
    
}

bool Chunk::IsOnOrForwardOfPlane(const Plane &plane)
{
    //glm::vec3 minp = glm::vec3(m_Position.x * SIZE, 0, m_Position.y * SIZE);
    //glm::vec3 maxp = minp + glm::vec3(SIZE, HEIGHT, SIZE);

    //glm::vec3 p = minp;

    //// This finds the point most in the direction of the planes normal
    //// this helps to find if the outmost point of the chunk is in front of the say plane
    //// in this case we consider the chunk to be in front of the plane
    //if (plane.normal.x >= 0)
    //    p.x = maxp.x;
    //if (plane.normal.y >= 0)
    //    p.y = maxp.y;
    //if (plane.normal.z >= 0)
    //    p.z = maxp.z;

    //return glm::dot(plane.normal, p) + plane.distance >= 0;

    return glm::dot(plane.normal, plane.chunkP + glm::vec3(m_Position.x * SIZE, 0, m_Position.y * SIZE)) + plane.distance >= 0;
}

// TODO: old implementation
BlockType Chunk::NeighbourGetBlock(glm::ivec3 pos, Chunk* nXP, Chunk* nXN, Chunk* nYP, Chunk* nYN)
{
    // using enum BlockType;
    //
    // if (!m_IsGenerated) return AIR;
    //
    // if (pos.y < 0 || pos.y >= HEIGHT)
    //     return AIR;
    //
    // if (pos.x < 0 || pos.x >= SIZE || pos.z < 0 || pos.z >= SIZE)
    // {
    //     pos.x += m_Position.x * SIZE;
    //     pos.z += m_Position.y * SIZE;
    //     return m_World->GetBlock(pos);
    // }
    //
    // return GetBlock(pos.x, pos.y, pos.z);

    if (pos.y < 0 || pos.y >= HEIGHT)
        return BlockType::AIR;

    if (pos.x >= SIZE)
        return nXP->GetBlock(pos.x - SIZE, pos.y, pos.z);
    if (pos.x < 0)
        return nXN->GetBlock(pos.x + SIZE, pos.y, pos.z);
    if (pos.z >= SIZE)
        return nYP->GetBlock(pos.x, pos.y, pos.z - SIZE);
    if (pos.z < 0)
        return nYN->GetBlock(pos.x, pos.y, pos.z + SIZE);

    return GetBlock(pos.x, pos.y, pos.z);
        
}

BlockType Chunk::GetBlock(int x, int y, int z) const
{
    return m_Blocks[SIZE * HEIGHT * z + SIZE * y + x];
}

void Chunk::SetBlock(int x, int y, int z, BlockType block)
{
    m_Blocks[SIZE * HEIGHT * z + SIZE * y + x] = block;
}


glm::ivec2 Chunk::GetPosition()
{
    return m_Position;
}


}
