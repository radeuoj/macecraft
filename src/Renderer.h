#pragma once

#include <array>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

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
    void Flush();

    std::vector<VD> vertices;

private:
    GLuint m_VAO = 0, m_VBO = 0;
    mutable bool m_isInitialized = false;
};
}

#include "Renderer.tpp"
