#include "chunk.h"


Chunk::Chunk(glm::uvec2 position) : position(position)
{
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 3; j++)
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
                    glm::vec3 blockPosition = {this->position.x * 16 + i, float(j), this->position.y * 16 + k};

                    // if (GetBlock({i, j, k - 1}) == BlockType::AIR)
                    // {
                    //     VertexData vertices[] =
                    //     {
                    //         { blockPosition.x,        blockPosition.y,        blockPosition.z,        0.0f, 0.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y,        blockPosition.z,        1.0f, 0.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y + 1.0f, blockPosition.z,        1.0f, 1.0f },
                    //         { blockPosition.x,        blockPosition.y,        blockPosition.z,        0.0f, 0.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y + 1.0f, blockPosition.z,        1.0f, 1.0f },
                    //         { blockPosition.x,        blockPosition.y + 1.0f, blockPosition.z,        0.0f, 1.0f },
                    //     };
                    //
                    //     renderer.RenderVertices(vertices, std::size(vertices));
                    // }
                    //
                    // if (GetBlock({i, j, k + 1}) == BlockType::AIR)
                    // {
                    //     VertexData vertices[] =
                    //     {
                    //         { blockPosition.x,        blockPosition.y,        blockPosition.z + 1.0f, 0.0f, 0.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y,        blockPosition.z + 1.0f, 1.0f, 0.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y + 1.0f, blockPosition.z + 1.0f, 1.0f, 1.0f },
                    //         { blockPosition.x,        blockPosition.y,        blockPosition.z + 1.0f, 0.0f, 0.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y + 1.0f, blockPosition.z + 1.0f, 1.0f, 1.0f },
                    //         { blockPosition.x,        blockPosition.y + 1.0f, blockPosition.z + 1.0f, 0.0f, 1.0f },
                    //     };
                    //
                    //     renderer.RenderVertices(vertices, std::size(vertices));
                    // }
                    //
                    // if (GetBlock({i, j - 1, k}) == BlockType::AIR)
                    // {
                    //     VertexData vertices[] =
                    //     {
                    //         { blockPosition.x,        blockPosition.y,        blockPosition.z,        0.0f, 0.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y,        blockPosition.z,        1.0f, 0.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y,        blockPosition.z + 1.0f, 1.0f, 1.0f },
                    //         { blockPosition.x,        blockPosition.y,        blockPosition.z,        0.0f, 0.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y,        blockPosition.z + 1.0f, 1.0f, 1.0f },
                    //         { blockPosition.x,        blockPosition.y,        blockPosition.z + 1.0f, 0.0f, 1.0f },
                    //     };
                    //
                    //     renderer.RenderVertices(vertices, std::size(vertices));
                    // }
                    //
                    // if (GetBlock({i, j + 1, k}) == BlockType::AIR)
                    // {
                    //     VertexData vertices[] =
                    //     {
                    //         { blockPosition.x,        blockPosition.y + 1.0f, blockPosition.z,        0.0f, 0.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y + 1.0f, blockPosition.z,        1.0f, 0.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y + 1.0f, blockPosition.z + 1.0f, 1.0f, 1.0f },
                    //         { blockPosition.x,        blockPosition.y + 1.0f, blockPosition.z,        0.0f, 0.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y + 1.0f, blockPosition.z + 1.0f, 1.0f, 1.0f },
                    //         { blockPosition.x,        blockPosition.y + 1.0f, blockPosition.z + 1.0f, 0.0f, 1.0f },
                    //     };
                    //
                    //     renderer.RenderVertices(vertices, std::size(vertices));
                    // }
                    //
                    // if (GetBlock({i - 1, j, k}) == BlockType::AIR)
                    // {
                    //     VertexData vertices[] =
                    //     {
                    //         { blockPosition.x,        blockPosition.y,        blockPosition.z,        0.0f, 0.0f },
                    //         { blockPosition.x,        blockPosition.y,        blockPosition.z + 1.0f, 1.0f, 0.0f },
                    //         { blockPosition.x,        blockPosition.y + 1.0f, blockPosition.z + 1.0f, 1.0f, 1.0f },
                    //         { blockPosition.x,        blockPosition.y,        blockPosition.z,        0.0f, 0.0f },
                    //         { blockPosition.x,        blockPosition.y + 1.0f, blockPosition.z + 1.0f, 1.0f, 1.0f },
                    //         { blockPosition.x,        blockPosition.y + 1.0f, blockPosition.z,        0.0f, 1.0f },
                    //     };
                    //
                    //     renderer.RenderVertices(vertices, std::size(vertices));
                    // }
                    //
                    // if (GetBlock({i + 1, j, k}) == BlockType::AIR)
                    // {
                    //     VertexData vertices[] =
                    //     {
                    //         { blockPosition.x + 1.0f, blockPosition.y,        blockPosition.z,        1.0f, 0.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y,        blockPosition.z + 1.0f, 0.0f, 0.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y + 1.0f, blockPosition.z + 1.0f, 0.0f, 1.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y,        blockPosition.z,        1.0f, 0.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y + 1.0f, blockPosition.z + 1.0f, 0.0f, 1.0f },
                    //         { blockPosition.x + 1.0f, blockPosition.y + 1.0f, blockPosition.z,        1.0f, 1.0f },
                    //     };
                    //
                    //     renderer.RenderVertices(vertices, std::size(vertices));
                    // }
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
