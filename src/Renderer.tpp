#pragma once
// #include "Renderer.h"

namespace Macecraft
{
template <typename VD>
Renderer<VD>::Renderer()
{
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    // glVertexAttribIPointer(0, 2, GL_SHORT, sizeof(VertexData), (void*) offsetof(VertexData, chunk_x));
    // glEnableVertexAttribArray(0);

    // glVertexAttribIPointer(0, 3, GL_UNSIGNED_BYTE, sizeof(VertexData), (void*) offsetof(VertexData, x));
    // glEnableVertexAttribArray(0);
    //
    // glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, sizeof(VertexData), (void*) offsetof(VertexData, texCoords));
    // glEnableVertexAttribArray(1);

    // Unbinding
    // glBindVertexArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_isInitialized = true;
}

template <typename VD>
void Renderer<VD>::LinkAttribPointer(GLuint layout, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset)
{
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glVertexAttribPointer(layout, size, type, normalized, stride, (void*) offset);
    glEnableVertexAttribArray(layout);
}

template <typename VD>
void Renderer<VD>::LinkAttribIPointer(GLuint layout, GLint size, GLenum type, GLsizei stride, size_t offset)
{
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glVertexAttribIPointer(layout, size, type, stride, (void*) offset);
    glEnableVertexAttribArray(layout);
}

template <typename VD>
Renderer<VD>::Renderer(Renderer&& other) noexcept
{
    vertices = other.vertices;
    m_VAO = other.m_VAO;
    m_VBO = other.m_VBO;
    m_EBO = other.m_EBO;

    if (other.m_isInitialized)
    {
        m_isInitialized = true;
        other.m_isInitialized = false;
    }

}

template <typename VD>
void Renderer<VD>::BindVertices()
{
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VD), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

    // glBindVertexArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

template <typename VD>
template <typename... Args>
uint32_t Renderer<VD>::PushVertex(Args&&... args)
{
    vertices.emplace_back(std::forward<Args>(args)...);
    return vertices.size() - 1;
}

template <typename VD>
template <typename... Args>
void Renderer<VD>::PushIndices(Args&&... args)
{
    for (auto& i: {args...})
    {
        indices.emplace_back(i);
    }
}


template <typename VD>
void Renderer<VD>::Flush(GLenum mode)
{

    glBindVertexArray(m_VAO);

    // glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // glDrawArrays(mode, 0, vertices.size());
    glDrawElements(mode, indices.size(), GL_UNSIGNED_INT, 0);
    
    // glBindVertexArray(0);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

}

template <typename VD>
Renderer<VD>::~Renderer()
{
    if (!m_isInitialized) return;

    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
}
}



