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


void Renderer::RenderSprite(glm::vec3 position, glm::vec2 size, Texture& texture)
{
    texture.Bind();

    GLfloat vertices[] =
    {
        position.x,          position.y,          position.z, 0.0f, 0.0f,
        position.x + size.x, position.y,          position.z, 1.0f, 0.0f,
        position.x + size.x, position.y + size.y, position.z, 1.0f, 1.0f,
        position.x,          position.y,          position.z, 0.0f, 0.0f,
        position.x + size.x, position.y + size.y, position.z, 1.0f, 1.0f,
        position.x,          position.y + size.y, position.z, 0.0f, 1.0f,
    };

    this->RenderVertices(vertices, std::size(vertices));

    texture.Unbind();
}


// void Renderer::RenderVertex(glm::vec3 position, glm::vec2 texturePosition)
// {
//     this->vertices.insert(this->vertices.end(), { position.x, position.y, position.z, texturePosition.x, texturePosition.y });
// }
//
// void Renderer::Flush()
// {
//     glBindVertexArray(VAO);
//
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);
//     glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STATIC_DRAW);
//
//     glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
//
//     // this->vertices.clear();
// }

void Renderer::Delete()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}



