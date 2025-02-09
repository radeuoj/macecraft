#include "Chunk.h"

#include <cstring>

#include "World.h"

namespace Macecraft
{
    Chunk::Chunk(World* world, glm::i16vec2 position): m_World(world), m_Position(position)
    {
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < 10; j++)
            {
                for (int k = 0; k < SIZE; k++)
                {
                    this->m_Blocks[i][j][k] = BlockType::DIRT;
                }
            }
        }

        m_Renderer.init();
    }

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


    void Chunk::render()
    {
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

                        // BACK
                        if (getBlock({i, j, k - 1}) == BlockType::AIR)
                        {
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, texcoords(0, 0) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(0, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, blockPosition.y, blockPosition.z, texcoords(1, 0) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(0, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(1, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, blockPosition.y, blockPosition.z, texcoords(1, 0) });
                        }

                        // FRONT
                        if (getBlock({i, j, k + 1}) == BlockType::AIR)
                        {
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(0, 0) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(1, 0) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(1, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(1, 0) });
                        }

                        //  BOTTOM
                        if (getBlock({i, j - 1, k}) == BlockType::AIR)
                        {
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, blockPosition.y, blockPosition.z, texcoords(0, 0) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, texcoords(1, 0) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(1, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, texcoords(1, 0) });
                        }

                        // TOP
                        if (getBlock({i, j + 1, k}) == BlockType::AIR)
                        {
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(0, 0) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(1, 0) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(1, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(1, 0) });
                        }

                        // LEFT
                        if (getBlock({i - 1, j, k}) == BlockType::AIR)
                        {
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, blockPosition.y, blockPosition.z, texcoords(0, 0) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(0, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(1, 0) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(0, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(1, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(1, 0) });
                        }

                        // RIGHT
                        if (getBlock({i + 1, j, k}) == BlockType::AIR)
                        {
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(0, 0) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, texcoords(1, 0) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(1, 1) });
                            m_Renderer.vertices.push_back({ this->m_Position.x, this->m_Position.y, GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, texcoords(1, 0) });
                        }
                    }
                }
            }
        }

        // m_Renderer.flush();
    }

    BlockType Chunk::getBlock(glm::ivec3 pos)
    {
        using enum BlockType;

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
