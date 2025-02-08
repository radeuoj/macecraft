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
        Renderer() = default;

        Renderer(const Renderer& other);

        void init();

        ~Renderer();

        void flush();

        std::vector<VertexData> vertices;

    private:
        GLuint m_VAO, m_VBO;
        mutable bool m_isInitialized = false;
    };
}
