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

    /**
     * !!!!!!!!!!!!!!!!!!!!!!DO NOT USE!!!!!!!!!!!!
     * @deprecated dont use
     */
    void renderVertices(VertexData vertices[], GLsizeiptr size);

    void renderTriangleStrip(GLfloat vertices[], GLsizeiptr size);

    /**
     *
     * @param vertices MUST HAVE 4 VERTICES OF SIZE 5 GLfloats: 3 for coords, 2 for texcoords
     * @param texture the texture
     */
    void renderSpriteVertices(VertexData vertices[]);

    void renderSprite(glm::vec3 position, glm::vec2 size, Texture& texture);

    void renderBlock(glm::vec3 position, glm::vec3 scale, Texture& texture);

    void flush();

    std::vector<VertexData> vertices;

private:
    GLuint m_VAO, m_VBO;
};
