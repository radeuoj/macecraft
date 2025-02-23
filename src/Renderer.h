#pragma once

#include <array>
#include <vector>

#include <glad/glad.h>

#include "Texture.h"
#include "VertexData.h"

namespace Macecraft
{
template <typename VD>
class Renderer
{
public:
    Renderer();
    Renderer(const Renderer& other) = delete;
    Renderer(Renderer&& other) noexcept;

    ~Renderer();

    void LinkAttribPointer(GLuint layout, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t offset);
    void LinkAttribIPointer(GLuint layout, GLint size, GLenum type, GLsizei stride, size_t offset);

    void BindVertices();

    /**
     * this does not push it into the index buffer
     * @param args constructor arguments of vertex type
     * @return index in vertices vector
     */
    template <typename... Args>
    uint32_t PushVertex(Args&&... args);

    /**
     * will throw weird error if you dont
     * @tparam Args MUST BE OF TYPE UINT32_T
     * @param args indices
     */
    template <typename... Args>
    void PushIndices(Args&&... args);

    /**
     * 
     * @param mode GL_TRIANGLES GL_POINTS GL_LINES etc\n
     * default is GL_TRIANGLES
     */
    void Flush(GLenum mode = GL_TRIANGLES);

    std::vector<VD> vertices;
    std::vector<uint32_t> indices;

private:
    GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;
    mutable bool m_isInitialized = false;
};
}

#include "Renderer.tpp"
