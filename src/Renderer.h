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
        Renderer(const Renderer& other) = delete;
        Renderer(Renderer&& other) noexcept;

        void init();

        ~Renderer();

        void bindVertices();
        void flush();

        std::vector<VertexData> vertices;

    private:
        GLuint m_VAO = 0, m_VBO = 0;
        mutable bool m_isInitialized = false;
    };
}
