#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "texture.h"

class Renderer
{
public:
    Renderer();

    void RenderVertices(GLfloat vertices[], GLsizeiptr size);
    void RenderSprite(glm::vec3 position, glm::vec2 size, Texture& texture);
    void RenderBlock(glm::vec3 position, glm::vec3 scale, Texture& texture);
    void Delete();

private:
    GLuint VAO{}, VBO{};
};
