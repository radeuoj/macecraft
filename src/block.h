#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

class Block
{
public:
    Block(glm::vec3 position, float length);

    void Render();

private:
    GLfloat vertices[180];
};
