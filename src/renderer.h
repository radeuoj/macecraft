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
    void Flush();
    void Delete();

private:
    std::vector<GLfloat> vertices;
    GLuint VAO{}, VBO{};
};
