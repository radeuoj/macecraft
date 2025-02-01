#pragma once

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Renderer
{
public:
    Renderer();

    void RenderVertex(glm::vec3 position, glm::vec2 texturePosition);
    void Flush();

private:
    std::vector<GLfloat> vertices;
    GLuint VAO{}, VBO{};
};
