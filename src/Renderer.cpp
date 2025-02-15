#include "Renderer.h"

namespace Macecraft
{
    void Renderer::init()
    {
        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);

        glGenBuffers(1, &m_VBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

        // glVertexAttribIPointer(0, 2, GL_SHORT, sizeof(VertexData), (void*) offsetof(VertexData, chunk_x));
        // glEnableVertexAttribArray(0);

        glVertexAttribIPointer(0, 3, GL_UNSIGNED_BYTE, sizeof(VertexData), (void*) offsetof(VertexData, x));
        glEnableVertexAttribArray(0);

        glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, sizeof(VertexData), (void*) offsetof(VertexData, texCoords));
        glEnableVertexAttribArray(1);

        // Unbinding
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        m_isInitialized = true;
    }

    Renderer::Renderer(Renderer&& other) noexcept
    {
        vertices = other.vertices;
        m_VAO = other.m_VAO;
        m_VBO = other.m_VBO;

        if (other.m_isInitialized)
        {
            m_isInitialized = true;
            other.m_isInitialized = false;
        }

    }

    void Renderer::bindVertices()
    {
        glBindVertexArray(m_VAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), vertices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Renderer::flush()
    {

        glBindVertexArray(m_VAO);

        // glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        // glBindVertexArray(0);
        // glBindBuffer(GL_ARRAY_BUFFER, 0);

    }

    Renderer::~Renderer()
    {
        if (!m_isInitialized) return;

        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
    }
}



