#include "renderer.h"

Renderer::Renderer()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size(), this->vertices.data(), GL_STREAM_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Renderer::RenderVertex(glm::vec3 position, glm::vec2 texturePosition)
{
    this->vertices.insert(this->vertices.end(), { position.x, position.y, position.z, texturePosition.x, texturePosition.y });
}

void Renderer::Flush()
{
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(float), this->vertices.data(), GL_STREAM_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());

    // this->vertices.clear();
}


