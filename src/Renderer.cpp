#include "Renderer.h"

void Renderer::init()
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    // glBufferData(GL_ARRAY_BUFFER, this->vertices.size(), this->vertices.data(), GL_STATIC_DRAW);

    glVertexAttribIPointer(0, 2, GL_SHORT, sizeof(VertexData), (void*) offsetof(VertexData, chunk_x));
    glEnableVertexAttribArray(0);

    glVertexAttribIPointer(1, 3, GL_UNSIGNED_BYTE, sizeof(VertexData), (void*) offsetof(VertexData, x));
    glEnableVertexAttribArray(1);

    glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(VertexData), (void*) offsetof(VertexData, texCoords));
    glEnableVertexAttribArray(2);

    // glVertexAttribIPointer (0, 3, GL_INT, sizeof(VertexData), (void*) offsetof(VertexData, x));
    // glEnableVertexAttribArray(0);
    //
    // glVertexAttribIPointer(1, 2, GL_UNSIGNED_BYTE, sizeof(VertexData), (void*) offsetof(VertexData, texCoords));
    // glEnableVertexAttribArray(1);

    // Unbinding
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::renderVertices(VertexData vertices[], GLsizeiptr size)
{
    //TODO: fix / remove

    this->vertices.insert(this->vertices.end(), vertices, vertices + size);
}

void Renderer::renderSpriteVertices(VertexData vertices[])
{
    //TODO: fix / remove

    // this->spriteVertices.insert(this->spriteVertices.end(), vertices, vertices + 30);
    // this->spriteTextures.push_back(&texture);
}

void Renderer::renderSprite(glm::vec3 position, glm::vec2 size, Texture& texture)
{
    //TODO: fix / remove

    // VertexData vertices[] =
    // { /*                    COORDS                      //  TEXTURE COORDS */
    //     position.x,          position.y,          position.z, 0.0f, 0.0f,
    //     position.x + size.x, position.y,          position.z, 1.0f, 0.0f,
    //     position.x + size.x, position.y + size.y, position.z, 1.0f, 1.0f,
    //     position.x,          position.y,          position.z, 0.0f, 0.0f,
    //     position.x + size.x, position.y + size.y, position.z, 1.0f, 1.0f,
    //     position.x,          position.y + size.y, position.z, 0.0f, 1.0f,
    // };
    //
    // this->RenderSpriteVertices(vertices);
}

void Renderer::renderBlock(glm::vec3 position, glm::vec3 scale, Texture& texture)
{
    // GLfloat vertices[] =
    // { /*                    COORDS                                   //  TEXTURE COORDS */
    //     position.x,           position.y,           position.z,           0.0f, 0.0f,
    //     position.x + scale.x, position.y,           position.z,           1.0f, 0.0f,
    //     position.x + scale.x, position.y + scale.y, position.z,           1.0f, 1.0f,
    //     position.x,           position.y,           position.z,           0.0f, 0.0f,
    //     position.x + scale.x, position.y + scale.y, position.z,           1.0f, 1.0f,
    //     position.x,           position.y + scale.y, position.z,           0.0f, 1.0f,
    //
    //     position.x,           position.y,           position.z,           0.0f, 0.0f,
    //     position.x + scale.x, position.y,           position.z,           1.0f, 0.0f,
    //     position.x + scale.x, position.y,           position.z + scale.z, 1.0f, 1.0f,
    //     position.x,           position.y,           position.z,           0.0f, 0.0f,
    //     position.x + scale.x, position.y,           position.z + scale.z, 1.0f, 1.0f,
    //     position.x,           position.y,           position.z + scale.z, 0.0f, 1.0f,
    //
    //     position.x,           position.y,           position.z + scale.z, 0.0f, 0.0f,
    //     position.x + scale.x, position.y,           position.z + scale.z, 1.0f, 0.0f,
    //     position.x + scale.x, position.y + scale.y, position.z + scale.z, 1.0f, 1.0f,
    //     position.x,           position.y,           position.z + scale.z, 0.0f, 0.0f,
    //     position.x + scale.x, position.y + scale.y, position.z + scale.z, 1.0f, 1.0f,
    //     position.x,           position.y + scale.y, position.z + scale.z, 0.0f, 1.0f,
    //
    //     position.x,           position.y + scale.y, position.z,           0.0f, 0.0f,
    //     position.x + scale.x, position.y + scale.y, position.z,           1.0f, 0.0f,
    //     position.x + scale.x, position.y + scale.y, position.z + scale.z, 1.0f, 1.0f,
    //     position.x,           position.y + scale.y, position.z,           0.0f, 0.0f,
    //     position.x + scale.x, position.y + scale.y, position.z + scale.z, 1.0f, 1.0f,
    //     position.x,           position.y + scale.y, position.z + scale.z, 0.0f, 1.0f,
    //
    //     position.x,           position.y,           position.z,           0.0f, 0.0f,
    //     position.x,           position.y,           position.z + scale.z, 1.0f, 0.0f,
    //     position.x,           position.y + scale.y, position.z + scale.z, 1.0f, 1.0f,
    //     position.x,           position.y,           position.z,           0.0f, 0.0f,
    //     position.x,           position.y + scale.y, position.z + scale.z, 1.0f, 1.0f,
    //     position.x,           position.y + scale.y, position.z,           0.0f, 1.0f,
    //
    //     position.x + scale.x, position.y,           position.z,           1.0f, 0.0f,
    //     position.x + scale.x, position.y,           position.z + scale.z, 0.0f, 0.0f,
    //     position.x + scale.x, position.y + scale.y, position.z + scale.z, 0.0f, 1.0f,
    //     position.x + scale.x, position.y,           position.z,           1.0f, 0.0f,
    //     position.x + scale.x, position.y + scale.y, position.z + scale.z, 0.0f, 1.0f,
    //     position.x + scale.x, position.y + scale.y, position.z,           1.0f, 1.0f,
    // };
    //
    // this->spriteVertices.insert(this->spriteVertices.end(), vertices, vertices + std::size(vertices));

    // for (int i = 0; i < 6; i++)
    // {s
    //     this->RenderSpriteVertices(vertices + (i * 30), texture);
    // }
}

void Renderer::flush()
{

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(VertexData), vertices.data(), GL_DYNAMIC_DRAW);

    // spriteTextures[0]->Bind();
    glDrawArrays(GL_TRIANGLES, 0, this->vertices.size() * 5);

    // for (int i = 0; i < this->spriteTextures.size(); i++)
    // {
    //     spriteTextures[i]->Bind();
    //
    //     glDrawArrays(GL_TRIANGLES, i * 30, (i + 1) * 30);
    //
    //     spriteTextures[i]->Unbind();
    // }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // printf("%llu vs %llu\n", this->spriteVertices.capacity(), this->spriteVertices.size());

    this->vertices.clear();
    // this->spriteTextures.clear();
}

void Renderer::renderTriangleStrip(GLfloat vertices[], GLsizeiptr size)
{
    //TODO: fix / remove

    // glBindVertexArray(VAO);
    //
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    //
    // glDrawArrays(GL_TRIANGLE_STRIP, 0, size);
    // // glDrawArrays(GL_LINE, 0, size);
    //
    // glBindVertexArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}



