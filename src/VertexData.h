#pragma once

#include <glad/glad.h>

struct VertexData
{
    GLshort chunk_x;
    GLshort chunk_y;
    GLubyte x;
    GLubyte y;
    GLubyte z;
    GLubyte texCoords;
};

inline GLubyte texcoords(GLubyte x, GLubyte y)
{
    // return (x * 16) + y;
    return x + 2 * y;
}

