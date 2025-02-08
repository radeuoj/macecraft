#pragma once

#include <array>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Texture.h"
#include "VertexData.h"

class Renderer
{
public:
    Renderer() = default;

    void init();

    ~Renderer();

    void flush();

    std::vector<VertexData> vertices;

private:
    GLuint m_VAO, m_VBO;
};
