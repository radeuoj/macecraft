#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

struct VertexData
{
    GLubyte x;
    GLubyte y;
    GLubyte z;
    GLubyte texCoords;
};

struct UIVertexData
{
    glm::vec2 position;
    glm::vec3 color;
};

