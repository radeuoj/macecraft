#include "Chunk.h"

#include <iostream>
#include <cstring>

#include "World.h"

namespace Macecraft
{
    Chunk::Chunk(World* world, glm::i16vec2 position): m_World(world), m_Position(position)
    {
        m_Renderer.init();
    }

    // Chunk::Chunk(const Chunk& other):
    // m_Position(other.m_Position),
    // m_World(other.m_World),
    // m_ShouldRender(other.m_ShouldRender),
    // m_IsGenerated(other.m_IsGenerated)
    // {
    //     m_Renderer = std::move(other.m_Renderer);
    //     memcpy(m_Blocks, other.m_Blocks, SIZE * HEIGHT * SIZE);
    // }

    Chunk::Chunk(Chunk&& other) noexcept:
    m_Renderer(std::move(other.m_Renderer)),
    m_World(other.m_World),
    m_Position(other.m_Position)
    // m_Blocks(other.m_Blocks)
    {
        // m_Renderer = std::move(other.m_Renderer);
        // m_Position = other.m_Position;
        // m_World = other.m_World;
        // m_Blocks = std::move(other.m_Blocks);
        memcpy(m_Blocks, other.m_Blocks, SIZE * HEIGHT * SIZE);
    }

    bool Chunk::isGenerated()
    {
        return m_IsGenerated;
    }

    void Chunk::generate(TextureAtlas* atlas)
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

    void Chunk::renderWhenPossible()
    {
        m_ShouldRender = true;
    }

    void Chunk::renderIfNeeded(TextureAtlas* atlas)
    {
        if (m_ShouldRender)
            render(atlas);
    }

    void Chunk::render(TextureAtlas* atlas)
    {
        if (!m_IsGenerated) return;

        if (!m_World->areNeighboursGenerated(m_Position)) return;

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
                        if (getBlock({i, j, k - 1}) == BlockType::AIR)
                        {
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], ZN, 0, 0)) });
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], ZN, 0, 1)) });
                            m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], ZN, 1, 0)) });
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], ZN, 0, 1)) });
                            m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], ZN, 1, 1)) });
                            m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], ZN, 1, 0)) });
                        }

                        // FRONT
                        if (getBlock({i, j, k + 1}) == BlockType::AIR)
                        {
                            m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], ZP, 0, 0)) });
                            m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], ZP, 0, 1)) });
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], ZP, 1, 0)) });
                            m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], ZP, 0, 1)) });
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], ZP, 1, 1)) });
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], ZP, 1, 0)) });
                        }

                        //  BOTTOM
                        if (getBlock({i, j - 1, k}) == BlockType::AIR)
                        {
                            m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], YN, 0, 0)) });
                            m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], YN, 0, 1)) });
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], YN, 1, 0)) });
                            m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], YN, 0, 1)) });
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], YN, 1, 1)) });
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], YN, 1, 0)) });
                        }

                        // TOP
                        if (getBlock({i, j + 1, k}) == BlockType::AIR)
                        {
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], YP, 0, 0)) });
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], YP, 0, 1)) });
                            m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], YP, 1, 0)) });
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], YP, 0, 1)) });
                            m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], YP, 1, 1)) });
                            m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], YP, 1, 0)) });
                        }

                        // LEFT
                        if (getBlock({i - 1, j, k}) == BlockType::AIR)
                        {
                            m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], XN, 0, 0)) });
                            m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], XN, 0, 1)) });
                            m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], XN, 1, 0)) });
                            m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], XN, 0, 1)) });
                            m_Renderer.vertices.push_back({ blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], XN, 1, 1)) });
                            m_Renderer.vertices.push_back({ blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], XN, 1, 0)) });
                        }

                        // RIGHT
                        if (getBlock({i + 1, j, k}) == BlockType::AIR)
                        {
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], XP, 0, 0)) });
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], XP, 0, 1)) });
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], XP, 1, 0)) });
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], XP, 0, 1)) });
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], XP, 1, 1)) });
                            m_Renderer.vertices.push_back({ GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, GLubyte(atlas->getTextureLocation(m_Blocks[i][j][k], XP, 1, 0)) });
                        }
                    }
                }
            }
        }

        m_Renderer.bindVertices();
        // m_Renderer.flush();
    }

    void Chunk::flush(Shader &shader, TextureAtlas* atlas)
    {
        if (!m_IsGenerated) return;

        // if (m_ShouldRender) render(atlas);

        glUniform2i(glGetUniformLocation(shader.ID, "chunkPos"), m_Position.x, m_Position.y);
        m_Renderer.flush();
    }

    bool Chunk::isOnFrustum(const Frustum& frustum)
    {
        return isOnOrForwardOfLine(frustum.nearPlane)
        && isOnOrForwardOfLine(frustum.rightPlane)
        && isOnOrForwardOfLine(frustum.leftPlane);
    }

    bool Chunk::isOnOrForwardOfLine(const Line &line)
    {
        constexpr float EXTENT = float(SIZE); // TODO: magic number

        // https://gdbooks.gitbooks.io/3dcollisions/content/Chapter2/static_aabb_plane.html
        const float r = EXTENT * (abs(line.normal.x) + abs(line.normal.y));

        return -r <= line.getSignedDistanceToPlane(m_Position * short(SIZE));

    }

    BlockType Chunk::getBlock(glm::ivec3 pos)
    {
        using enum BlockType;

        if (!m_IsGenerated) return AIR;

        if (pos.y < 0 || pos.y >= HEIGHT)
            return AIR;

        if (pos.x < 0 || pos.x >= SIZE || pos.z < 0 || pos.z >= SIZE)
        {
            pos.x += m_Position.x * SIZE;
            pos.z += m_Position.y * SIZE;
            return m_World->getBlock(pos);
        }

        return this->m_Blocks[pos.x][pos.y][pos.z];
    }

    glm::i16vec2 Chunk::getPosition()
    {
        return m_Position;
    }


}
