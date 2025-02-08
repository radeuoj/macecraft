#include "Renderer.h"

void Renderer::init()
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glVertexAttribIPointer(0, 2, GL_SHORT, sizeof(VertexData), (void*) offsetof(VertexData, chunk_x));
    glEnableVertexAttribArray(0);

    glVertexAttribIPointer(1, 3, GL_UNSIGNED_BYTE, sizeof(VertexData), (void*) offsetof(VertexData, x));
    glEnableVertexAttribArray(1);

    glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(VertexData), (void*) offsetof(VertexData, texCoords));
    glEnableVertexAttribArray(2);

    // Unbinding
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::flush()
{

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), vertices.data(), GL_STATIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    vertices.clear();
}

Renderer::~Renderer()
{
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}



