#include "chunk.h"


Chunk::Chunk(glm::i16vec2 position) : position(position)
{
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            for (int k = 0; k < 16; k++)
            {
                this->blocks[i][j][k] = BlockType::DIRT;
            }
        }
    }
}

void Chunk::Render(Renderer& renderer)
{
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 256; j++)
        {
            for (int k = 0; k < 16; k++)
            {
                // if (GetBlock({i, j, k}) != BlockType::AIR)
                if (this->blocks[i][j][k] != BlockType::AIR)
                {
                    // glm::u8vec3 blockPosition = {this->position.x * 16 + i, j, this->position.y * 16 + k};
                    glm::u8vec3 blockPosition = {i, j, k};

                    // BACK
                    if (GetBlock({i, j, k - 1}) == BlockType::AIR)
                    {
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, texcoords(0, 0) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(0, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, blockPosition.y, blockPosition.z, texcoords(1, 0) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(0, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(1, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, blockPosition.y, blockPosition.z, texcoords(1, 0) });
                    }

                    // FRONT
                    if (GetBlock({i, j, k + 1}) == BlockType::AIR)
                    {
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(0, 0) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(1, 0) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(1, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(1, 0) });
                    }

                    //  BOTTOM
                    if (GetBlock({i, j - 1, k}) == BlockType::AIR)
                    {
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, blockPosition.y, blockPosition.z, texcoords(0, 0) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, texcoords(1, 0) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(1, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, texcoords(1, 0) });
                    }

                    // TOP
                    if (GetBlock({i, j + 1, k}) == BlockType::AIR)
                    {
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(0, 0) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(1, 0) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(1, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(1, 0) });
                    }

                    // LEFT
                    if (GetBlock({i - 1, j, k}) == BlockType::AIR)
                    {
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, blockPosition.y, blockPosition.z, texcoords(0, 0) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(0, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(1, 0) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(0, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(1, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, blockPosition.x, blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(1, 0) });
                    }

                    // RIGHT
                    if (GetBlock({i + 1, j, k}) == BlockType::AIR)
                    {
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), blockPosition.y, GLubyte(blockPosition.z + 1), texcoords(0, 0) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, texcoords(1, 0) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), GLubyte(blockPosition.z + 1), texcoords(0, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), GLubyte(blockPosition.y + 1), blockPosition.z, texcoords(1, 1) });
                        renderer.vertices.push_back({ this->position.x, this->position.y, GLubyte(blockPosition.x + 1), blockPosition.y, blockPosition.z, texcoords(1, 0) });
                    }
                }
            }
        }
    }
}

BlockType Chunk::GetBlock(glm::uvec3 pos)
{
    using enum BlockType;

    if (pos.x < 0 || pos.x >= 16)
        return AIR;
    if (pos.y < 0 || pos.y >= 16)
        return AIR;
    if (pos.z < 0 || pos.z >= 16)
        return AIR;

    return this->blocks[pos.x][pos.y][pos.z];
}
