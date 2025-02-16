#pragma once

#include <array>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "VertexData.h"

namespace Macecraft
{
class Renderer
{
public:
    Renderer();
    Renderer(const Renderer& other) = delete;
    Renderer(Renderer&& other) noexcept;

    ~Renderer();

    void BindVertices();
    void Flush();

    std::vector<VertexData> vertices;

private:
    GLuint m_VAO = 0, m_VBO = 0;
    mutable bool m_isInitialized = false;
};
}
