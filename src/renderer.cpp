#include "renderer.h"

Renderer::Renderer()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, this->vertices.size(), this->vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbinding
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::RenderVertices(GLfloat vertices[], GLsizeiptr size)
{
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, size);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::RenderSpriteVertices(GLfloat vertices[], Texture& texture)
{
    this->spriteVertices.insert(this->spriteVertices.end(), vertices, vertices + 30);
    this->spriteTextures.push_back(&texture);
}

void Renderer::RenderSprite(glm::vec3 position, glm::vec2 size, Texture& texture)
{
    GLfloat vertices[] =
    { /*                    COORDS                      //  TEXTURE COORDS */
        position.x,          position.y,          position.z, 0.0f, 0.0f,
        position.x + size.x, position.y,          position.z, 1.0f, 0.0f,
        position.x + size.x, position.y + size.y, position.z, 1.0f, 1.0f,
        position.x,          position.y,          position.z, 0.0f, 0.0f,
        position.x + size.x, position.y + size.y, position.z, 1.0f, 1.0f,
        position.x,          position.y + size.y, position.z, 0.0f, 1.0f,
    };

    this->RenderSpriteVertices(vertices, texture);
}

void Renderer::RenderBlock(glm::vec3 position, glm::vec3 scale, Texture& texture)
{
    GLfloat vertices[] =
    { /*                    COORDS                                   //  TEXTURE COORDS */
        position.x,           position.y,           position.z,           0.0f, 0.0f,
        position.x + scale.x, position.y,           position.z,           1.0f, 0.0f,
        position.x + scale.x, position.y + scale.y, position.z,           1.0f, 1.0f,
        position.x,           position.y,           position.z,           0.0f, 0.0f,
        position.x + scale.x, position.y + scale.y, position.z,           1.0f, 1.0f,
        position.x,           position.y + scale.y, position.z,           0.0f, 1.0f,

        position.x,           position.y,           position.z,           0.0f, 0.0f,
        position.x + scale.x, position.y,           position.z,           1.0f, 0.0f,
        position.x + scale.x, position.y,           position.z + scale.z, 1.0f, 1.0f,
        position.x,           position.y,           position.z,           0.0f, 0.0f,
        position.x + scale.x, position.y,           position.z + scale.z, 1.0f, 1.0f,
        position.x,           position.y,           position.z + scale.z, 0.0f, 1.0f,

        position.x,           position.y,           position.z + scale.z, 0.0f, 0.0f,
        position.x + scale.x, position.y,           position.z + scale.z, 1.0f, 0.0f,
        position.x + scale.x, position.y + scale.y, position.z + scale.z, 1.0f, 1.0f,
        position.x,           position.y,           position.z + scale.z, 0.0f, 0.0f,
        position.x + scale.x, position.y + scale.y, position.z + scale.z, 1.0f, 1.0f,
        position.x,           position.y + scale.y, position.z + scale.z, 0.0f, 1.0f,

        position.x,           position.y + scale.y, position.z,           0.0f, 0.0f,
        position.x + scale.x, position.y + scale.y, position.z,           1.0f, 0.0f,
        position.x + scale.x, position.y + scale.y, position.z + scale.z, 1.0f, 1.0f,
        position.x,           position.y + scale.y, position.z,           0.0f, 0.0f,
        position.x + scale.x, position.y + scale.y, position.z + scale.z, 1.0f, 1.0f,
        position.x,           position.y + scale.y, position.z + scale.z, 0.0f, 1.0f,

        position.x,           position.y,           position.z,           0.0f, 0.0f,
        position.x,           position.y,           position.z + scale.z, 1.0f, 0.0f,
        position.x,           position.y + scale.y, position.z + scale.z, 1.0f, 1.0f,
        position.x,           position.y,           position.z,           0.0f, 0.0f,
        position.x,           position.y + scale.y, position.z + scale.z, 1.0f, 1.0f,
        position.x,           position.y + scale.y, position.z,           0.0f, 1.0f,

        position.x + scale.x, position.y,           position.z,           1.0f, 0.0f,
        position.x + scale.x, position.y,           position.z + scale.z, 0.0f, 0.0f,
        position.x + scale.x, position.y + scale.y, position.z + scale.z, 0.0f, 1.0f,
        position.x + scale.x, position.y,           position.z,           1.0f, 0.0f,
        position.x + scale.x, position.y + scale.y, position.z + scale.z, 0.0f, 1.0f,
        position.x + scale.x, position.y + scale.y, position.z,           1.0f, 1.0f,
    };

    for (int i = 0; i < 6; i++)
    {
        this->RenderSpriteVertices(vertices + (i * 30), texture);
    }
}

void Renderer::Flush()
{

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, this->spriteVertices.size() * sizeof(GLfloat), spriteVertices.data(), GL_DYNAMIC_DRAW);

    spriteTextures[0]->Bind();
    glDrawArrays(GL_TRIANGLES, 0, this->spriteVertices.size());

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

    this->spriteVertices.clear();
    this->spriteTextures.clear();
}

void Renderer::RenderTriangleStrip(GLfloat vertices[], GLsizeiptr size)
{
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, size);
    // glDrawArrays(GL_LINE, 0, size);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::Delete()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}



